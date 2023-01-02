#include "ImGuiRenderTextureImageD3D12.hpp"

#include <Fusion/Renderer/GraphicsContext.hpp>
#include <Fusion/Renderer/Renderer.hpp>

namespace FusionEditor {

	ImGuiRenderTextureImageD3D12::ImGuiRenderTextureImageD3D12(const std::vector<Fusion::Shared<Fusion::RenderTexture>>& InRenderTextures)
	    : m_TargetTextures(InRenderTextures)
	{
		m_DescriptorHeap = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetDescriptorHeap(Fusion::EDescriptorHeapType::SRV_CBV_UAV);

		for (size_t Idx = 0; Idx < InRenderTextures.size(); Idx++)
			m_RTVAllocations.push_back(m_DescriptorHeap->AllocateShaderResourceView(m_TargetTextures[Idx], 0));
	}

	void ImGuiRenderTextureImageD3D12::DrawImage(const ImVec2& InSize)
	{
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();
		ImTextureID TexID = reinterpret_cast<ImTextureID>(m_DescriptorHeap->GetGPUDescriptorHandle(m_RTVAllocations[FrameIndex]));
		ImGui::Image(TexID, InSize);
	}

	void ImGuiRenderTextureImageD3D12::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		uint32_t FrameIndex = Fusion::Renderer::GetCurrent().GetCurrentFrame();

		m_DescriptorHeap->Deallocate(m_RTVAllocations[FrameIndex]);

		// TODO(Peter): Do this in a loop to make this generic
		m_TargetTextures[FrameIndex]->Resize(0, { InWidth, InHeight });
		m_TargetTextures[FrameIndex]->Resize(1, { InWidth, InHeight });

		m_RTVAllocations[FrameIndex] = m_DescriptorHeap->AllocateShaderResourceView(m_TargetTextures[FrameIndex], 0);
	}

}
