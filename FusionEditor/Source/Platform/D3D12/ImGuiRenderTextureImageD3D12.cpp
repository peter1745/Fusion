#include "ImGuiRenderTextureImageD3D12.hpp"

#include <Fusion/Renderer/GraphicsContext.hpp>
#include <Fusion/Renderer/Renderer.hpp>

namespace FusionEditor {

	ImGuiRenderTextureImageD3D12::ImGuiRenderTextureImageD3D12(const Fusion::Shared<Fusion::RenderTexture>& InRenderTexture)
	    : m_TargetTexture(InRenderTexture)
	{
		m_DescriptorHeap = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetDescriptorHeap(Fusion::EDescriptorHeapType::SRV_CBV_UAV);
		m_RTVAllocations = m_DescriptorHeap->AllocateShaderResourceViews(m_TargetTexture, 0);
	}

	void ImGuiRenderTextureImageD3D12::DrawImage(const ImVec2& InSize) const
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
		m_TargetTexture->Resize(0, FrameIndex, { InWidth, InHeight });
		m_TargetTexture->Resize(1, FrameIndex, { InWidth, InHeight });

		m_RTVAllocations[FrameIndex] = m_DescriptorHeap->AllocateShaderResourceView(m_TargetTexture, 0, FrameIndex);
	}

}
