#include "ImGuiPlatformContextVulkan.hpp"

#include <Fusion/Core/Assert.hpp>
#include <Fusion/Renderer/GraphicsContext.hpp>
#include <Fusion/Renderer/Renderer.hpp>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_vulkan.h>

#include <GLFW/glfw3.h>

namespace FusionEditor {

	void ImGuiPlatformContextVulkan::InitPlatform(const Fusion::Unique<Fusion::Window>& InWindow, const Fusion::Shared<Fusion::GraphicsContext>& InContext, const Fusion::Shared<Fusion::SwapChain>& InSwapChain)
	{
		m_SwapChain = InSwapChain.As<Fusion::SwapChain>();

		auto* NativeWindow = static_cast<GLFWwindow*>(InWindow->GetWindowHandle());
		ImGui_ImplGlfw_InitForVulkan(NativeWindow, true);

		const auto& QueueInfo = InContext->GetDevice()->GetQueueInfo();

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
		Fusion::CoreVerify(vkCreateDescriptorPool(InContext->GetDevice()->GetLogicalDevice(), &DescriptorPoolInfo, nullptr, &m_FontDescriptorPool) == VK_SUCCESS);

		VkAttachmentDescription ColorAttachmentDesc = {};
		ColorAttachmentDesc.flags = 0;
		ColorAttachmentDesc.format = m_SwapChain->GetImageFormat();
		ColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		ColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		ColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		ColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference ColorAttachmentReference = {};
		ColorAttachmentReference.attachment = 0;
		ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription SubpassDesc = {};
		SubpassDesc.flags = 0;
		SubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDesc.colorAttachmentCount = 1;
		SubpassDesc.pColorAttachments = &ColorAttachmentReference;

		VkSubpassDependency SubpassDependency = {};
		SubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		SubpassDependency.dstSubpass = 0;
		SubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		SubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		SubpassDependency.srcAccessMask = 0;
		SubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		SubpassDependency.dependencyFlags = 0;

		VkRenderPassCreateInfo RenderPassInfo = {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = 1;
		RenderPassInfo.pAttachments = &ColorAttachmentDesc;
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &SubpassDesc;
		RenderPassInfo.dependencyCount = 1;
		RenderPassInfo.pDependencies = &SubpassDependency;
		vkCreateRenderPass(InContext->GetDevice()->GetLogicalDevice(), &RenderPassInfo, nullptr, &m_RenderPass);

		ImGui_ImplVulkan_InitInfo ImGuiInitInfo = {};
		ImGuiInitInfo.Instance = InContext->GetInstance();
		ImGuiInitInfo.PhysicalDevice = InContext->GetDevice()->GetPhysicalDevice();
		ImGuiInitInfo.Device = InContext->GetDevice()->GetLogicalDevice();
		ImGuiInitInfo.QueueFamily = QueueInfo.QueueFamily;
		ImGuiInitInfo.Queue = QueueInfo.Queue;
		ImGuiInitInfo.PipelineCache = nullptr;
		ImGuiInitInfo.DescriptorPool = m_FontDescriptorPool;
		ImGuiInitInfo.Subpass = 0;
		ImGuiInitInfo.MinImageCount = InContext->GetDevice()->GetSurfaceProperties(InContext->GetSurface()).Capabilities.minImageCount;
		ImGuiInitInfo.ImageCount = m_SwapChain->GetImageCount();
		ImGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGuiInitInfo.Allocator = nullptr;
		ImGui_ImplVulkan_Init(&ImGuiInitInfo, m_RenderPass);

		// Upload Fonts
		{
			auto CommandAllocator = InContext->GetTemporaryCommandAllocator();
			auto CommandList = CommandAllocator->AllocateCommandBuffer();
			auto CmdList = dynamic_cast<Fusion::CommandBuffer*>(CommandList)->GetBuffer();

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

			InContext->GetDevice()->Wait();

			ImGui_ImplVulkan_DestroyFontUploadObjects();

			CommandAllocator->DestroyCommandBuffer(CommandList);
		}

		m_FrameBuffers.resize(m_SwapChain->GetImageCount(), VK_NULL_HANDLE);

		OnSwapChainInvalidated(*m_SwapChain);

		m_SwapChainCallbackHandle = m_SwapChain->RegisterOnInvalidatedCallback(FUSION_BIND_FUNC(ImGuiPlatformContextVulkan::OnSwapChainInvalidated));
	}

	void ImGuiPlatformContextVulkan::BeginFramePlatform(Fusion::CommandBuffer* InCommandList)
	{
		auto CmdList = dynamic_cast<Fusion::CommandBuffer*>(InCommandList)->GetBuffer();
		
		VkClearValue ClearValue = {};

		VkRenderPassBeginInfo RenderPassBeginInfo = {};
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.renderPass = m_RenderPass;
		RenderPassBeginInfo.framebuffer = m_FrameBuffers[m_SwapChain->GetCurrentImage()];
		RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		RenderPassBeginInfo.renderArea.extent = m_SwapChain->GetImageExtent();
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearValue;
		vkCmdBeginRenderPass(CmdList, &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiPlatformContextVulkan::EndFramePlatform(Fusion::CommandBuffer* InCommandList)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), InCommandList->GetBuffer());

		vkCmdEndRenderPass(InCommandList->GetBuffer());

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
		auto Device = Fusion::GraphicsContext::Get()->GetDevice();

		vkQueueWaitIdle(Device->GetQueueInfo().Queue);

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		vkDestroyRenderPass(Device->GetLogicalDevice(), m_RenderPass, nullptr);
		for (auto Framebuffer : m_FrameBuffers)
			vkDestroyFramebuffer(Device->GetLogicalDevice(), Framebuffer, nullptr);
		m_FrameBuffers.clear();

		vkDestroyDescriptorPool(Device->GetLogicalDevice(), m_FontDescriptorPool, nullptr);

		m_SwapChain->UnregisterOnInvalidatedCallback(m_SwapChainCallbackHandle);
	}

	void ImGuiPlatformContextVulkan::OnSwapChainInvalidated(const Fusion::SwapChain& InSwapChain)
	{
		auto Device = Fusion::GraphicsContext::Get()->GetDevice().As<Fusion::Device>();

		for (uint32_t Idx = 0; Idx < InSwapChain.GetImageCount(); Idx++)
		{
			vkDestroyFramebuffer(Device->GetLogicalDevice(), m_FrameBuffers[Idx], nullptr);

			VkImageView AttachmentImageView = InSwapChain.GetImageView(Idx);
			VkExtent2D ImageExtent = InSwapChain.GetImageExtent();

			VkFramebufferCreateInfo FramebufferInfo = {};
			FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FramebufferInfo.flags = 0;
			FramebufferInfo.renderPass = m_RenderPass;
			FramebufferInfo.attachmentCount = 1;
			FramebufferInfo.pAttachments = &AttachmentImageView;
			FramebufferInfo.width = ImageExtent.width;
			FramebufferInfo.height = ImageExtent.height;
			FramebufferInfo.layers = 1;

			vkCreateFramebuffer(Device->GetLogicalDevice(), &FramebufferInfo, nullptr, &m_FrameBuffers[Idx]);
		}
	}

}
