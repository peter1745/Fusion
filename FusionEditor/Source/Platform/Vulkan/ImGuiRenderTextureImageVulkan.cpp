#include "ImGuiRenderTextureImageVulkan.hpp"

#include <Fusion/Renderer/Renderer.hpp>

#include <Platform/Vulkan/VulkanContext.hpp>
#include <Platform/Vulkan/VulkanRenderTexture.hpp>

#include "ImGui/backends/imgui_impl_vulkan.h"

namespace FusionEditor {

	ImGuiRenderTextureImageVulkan::ImGuiRenderTextureImageVulkan(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures)
	    : m_TargetTextures(InRenderTextures)
	{
		auto Device = Fusion::GraphicsContext::Get<Fusion::VulkanContext>()->GetDevice().As<Fusion::VulkanDevice>();

		VkSamplerCreateInfo SamplerInfo = {};
		SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		SamplerInfo.magFilter = VK_FILTER_LINEAR;
		SamplerInfo.minFilter = VK_FILTER_LINEAR;
		SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		vkCreateSampler(Device->GetLogicalDevice(), &SamplerInfo, nullptr, &m_Sampler);

		m_DescriptorSets.resize(InRenderTextures.size(), VK_NULL_HANDLE);
	}

	void ImGuiRenderTextureImageVulkan::DrawImage(const ImVec2& InSize)
	{
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();

		if (m_DescriptorSets[FrameIndex] == VK_NULL_HANDLE)
		{
			auto VulkanRT = m_TargetTextures[FrameIndex].As<Fusion::VulkanRenderTexture>();
			Fusion::EImageState State = VulkanRT->GetImage(0)->GetState();
			VkImageView View = VulkanRT->GetImageView(0);
			VkImageLayout Layout = Fusion::ImageStatesToVkImageLayout(State);
			m_DescriptorSets[FrameIndex] = ImGui_ImplVulkan_AddTexture(m_Sampler, View, Layout);
		}

		ImGui::Image(m_DescriptorSets[FrameIndex], InSize);
	}

	void ImGuiRenderTextureImageVulkan::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();
		Fusion::Renderer::GetCurrent().SubmitResourceForDestruction(3, [InInstance = this, InFrameIndex = FrameIndex]()
	    {
			if (InInstance->m_DescriptorSets[InFrameIndex] == VK_NULL_HANDLE)
				return;

			ImGui_ImplVulkan_RemoveTexture(InInstance->m_DescriptorSets[InFrameIndex]);
			InInstance->m_DescriptorSets[InFrameIndex] = VK_NULL_HANDLE;
		});

		m_TargetTextures[FrameIndex]->Resize(0, { InWidth, InHeight });
	}

}
