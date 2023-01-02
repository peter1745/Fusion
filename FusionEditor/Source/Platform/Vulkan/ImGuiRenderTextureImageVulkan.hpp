#pragma once

#include "UI/ImGuiRenderTextureImage.hpp"

#include <vulkan/vulkan.h>

namespace FusionEditor {

	class ImGuiRenderTextureImageVulkan : public ImGuiRenderTextureImage
	{
	public:
		ImGuiRenderTextureImageVulkan(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures);

		void DrawImage(const ImVec2& InSize) override;
		void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		std::vector<Fusion::Shared<Fusion::RenderTexture>> m_TargetTextures;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		VkSampler m_Sampler = VK_NULL_HANDLE;
	};

}
