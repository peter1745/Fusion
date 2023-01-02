#pragma once

#include "D3D12Common.hpp"
#include "D3D12Image2D.hpp"

#include "Fusion/Renderer/RenderTexture.hpp"

#include <vector>

namespace Fusion {

	class D3D12RenderTexture : public RenderTexture
	{
	public:
		D3D12RenderTexture(const RenderTextureInfo& InCreateInfo);

		void Bind(CommandList* InCommandList) override;
		void Unbind(CommandList* InCommandList) override;

		void Clear() override;
		
		void Resize(uint32_t InAttachmentIndex, const ImageSize& InSize) override;

		uint32_t GetWidth() const override { return m_CreateInfo.Width; }
		uint32_t GetHeight() const override { return m_CreateInfo.Height; }

		Shared<Image2D> GetImage(uint32_t InAttachmentIndex) const override { return m_Attachments[InAttachmentIndex]; }

		void TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState) override;

		void* GetColorTextureID(uint32_t InColorAttachmentIdx) const override;

		const RenderTextureInfo& GetInfo() const override { return m_CreateInfo; }

		void Release() override;

		auto& GetColorAttachments() { return m_Attachments; }
		const auto& GetColorAttachments() const { return m_Attachments; }

	private:
		RenderTextureInfo m_CreateInfo{};
		std::vector<Shared<D3D12Image2D>> m_Attachments;
		Shared<D3D12Image2D> m_DepthStencilAttachment = nullptr;
		bool m_HasDepthStencilAttachment = false;

		D3DComPtr<ID3D12DescriptorHeap> m_RenderTargetDescriptorHeap;
		D3DComPtr<ID3D12DescriptorHeap> m_DepthStencilDescriptorHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_RenderTargetViewHandle{};
		uint32_t m_RenderTargetViewHandleIncrementSize = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE m_DepthStencilViewHandle{};
		uint32_t m_DepthStencilViewHandleIncrementSize = 0;

	};

}
