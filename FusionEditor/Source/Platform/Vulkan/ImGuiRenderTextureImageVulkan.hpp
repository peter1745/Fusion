#pragma once

#include "UI/ImGuiRenderTextureImage.hpp"

#include <vulkan/vulkan.h>

namespace FusionEditor {

	class ImGuiRenderTextureImageVulkan : public ImGuiRenderTextureImage
	{
	public:
		ImGuiRenderTextureImageVulkan(const Fusion::Shared<Fusion::RenderTexture>& InRenderTexture);

		void DrawImage(const ImVec2& InSize) override;
		void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		Fusion::Shared<Fusion::RenderTexture> m_TargetTexture = nullptr;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		VkSampler m_Sampler = VK_NULL_HANDLE;
	};

}
