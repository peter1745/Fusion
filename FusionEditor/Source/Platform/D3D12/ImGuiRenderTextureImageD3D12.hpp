#pragma once

#include "UI/ImGuiRenderTextureImage.hpp"

#include <Fusion/Renderer/DescriptorHeap.hpp>

namespace FusionEditor {

	class ImGuiRenderTextureImageD3D12 : public ImGuiRenderTextureImage
	{
	public:
		ImGuiRenderTextureImageD3D12(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures);

		void DrawImage(const ImVec2& InSize) override;
		void Resize(uint32_t InWidth, uint32_t InHeight) override;

	private:
		std::vector<Fusion::Shared<Fusion::RenderTexture>> m_TargetTextures;

		Fusion::Shared<Fusion::DescriptorHeap> m_DescriptorHeap = nullptr;
		std::vector<Fusion::DescriptorHeapAllocation> m_RTVAllocations;

	};

}
