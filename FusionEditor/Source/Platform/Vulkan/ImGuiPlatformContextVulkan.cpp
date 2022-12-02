#include "ImGuiPlatformContextVulkan.hpp"

#include <Platform/Vulkan/VulkanContext.hpp>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_vulkan.h>

#include <GLFW/glfw3.h>

namespace FusionEditor {

	void ImGuiPlatformContextVulkan::InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain)
	{
		auto VkContext = InContext.As<Fusion::VulkanContext>();
		auto* NativeWindow = static_cast<GLFWwindow*>(InWindow->GetWindowHandle());
		ImGui_ImplGlfw_InitForVulkan(NativeWindow, true);

		auto Device = VkContext->GetDevice().As<Fusion::VulkanDevice>();
		const auto& QueueInfo = Device->GetQueueInfo();

		VkDescriptorPoolSize PoolSizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo DescriptorPoolInfo = {};
		DescriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		DescriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		DescriptorPoolInfo.maxSets = 1000 * IM_ARRAYSIZE(PoolSizes);
		DescriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(PoolSizes));
		DescriptorPoolInfo.pPoolSizes = PoolSizes;
		FUSION_CORE_VERIFY(vkCreateDescriptorPool(Device->GetLogicalDevice(), &DescriptorPoolInfo, nullptr, &m_FontDescriptorPool) == VK_SUCCESS);

		ImGui_ImplVulkan_InitInfo ImGuiInitInfo = {};
		ImGuiInitInfo.Instance = VkContext->GetInstance();
		ImGuiInitInfo.PhysicalDevice = Device->GetPhysicalDevice();
		ImGuiInitInfo.Device = Device->GetLogicalDevice();
		ImGuiInitInfo.QueueFamily = QueueInfo.QueueFamily;
		ImGuiInitInfo.Queue = QueueInfo.Queue;
		ImGuiInitInfo.PipelineCache = nullptr;
		ImGuiInitInfo.DescriptorPool = m_FontDescriptorPool;
		ImGuiInitInfo.Subpass = 0;
		ImGuiInitInfo.MinImageCount = InSwapChain.As<Fusion::VulkanSwapChain>()->GetMinImageCount();
		ImGuiInitInfo.ImageCount = InSwapChain.As<Fusion::VulkanSwapChain>()->GetImageCount();
		ImGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGuiInitInfo.Allocator = nullptr;
		ImGui_ImplVulkan_Init(&ImGuiInitInfo, InSwapChain.As<Fusion::VulkanSwapChain>()->GetRenderPass());

		// Upload Fonts
		{
			auto CommandAllocator = VkContext->GetTemporaryCommandAllocator();
			auto CommandList = CommandAllocator->AllocateCommandList();
			auto CmdList = static_cast<Fusion::VulkanCommandList*>(CommandList)->GetBuffer();

			vkResetCommandBuffer(CmdList, 0);

			VkCommandBufferBeginInfo BeginInfo = {};
			BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vkBeginCommandBuffer(CmdList, &BeginInfo);

			ImGui_ImplVulkan_CreateFontsTexture(CmdList);

			vkEndCommandBuffer(CmdList);

			VkSubmitInfo SubmitInfo = {};
			SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			SubmitInfo.commandBufferCount = 1;
			SubmitInfo.pCommandBuffers = &CmdList;
			vkQueueSubmit(QueueInfo.Queue, 1, &SubmitInfo, VK_NULL_HANDLE);

			VkContext->GetDevice()->Wait();

			ImGui_ImplVulkan_DestroyFontUploadObjects();

			CommandAllocator->DestroyCommandList(CommandList);
		}
	}

	void ImGuiPlatformContextVulkan::BeginFramePlatform()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiPlatformContextVulkan::EndFramePlatform(Fusion::CommandList* InCommandList)
	{
		auto Ctx = Fusion::GraphicsContext::Get<Fusion::VulkanContext>();
		auto CmdList = dynamic_cast<Fusion::VulkanCommandList*>(InCommandList)->GetBuffer();

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CmdList);

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* ContextBackup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(ContextBackup);
		}
	}

	void ImGuiPlatformContextVulkan::ShutdownPlatform()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		auto Ctx = Fusion::GraphicsContext::Get<Fusion::VulkanContext>();
		vkDestroyDescriptorPool(Ctx->GetDevice().As<Fusion::VulkanDevice>()->GetLogicalDevice(), m_FontDescriptorPool, nullptr);
	}
}
