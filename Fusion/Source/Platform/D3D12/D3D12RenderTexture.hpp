#pragma once

#include "D3D12Common.hpp"
#include "D3D12Image2D.hpp"

#include "Fusion/Renderer/RenderTexture.hpp"

#include <vector>

namespace Fusion {

	struct D3D12RenderTextureAttachment { std::vector<Shared<D3D12Image2D>> Images; };

	class D3D12RenderTexture : public RenderTexture
	{
	public:
		D3D12RenderTexture(const RenderTextureInfo& InCreateInfo);

		virtual void Bind(CommandList* InCommandList) override;
		virtual void Unbind(CommandList* InCommandList) override;

		virtual void Clear() override;
		
		virtual void Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize) override;

		virtual uint32_t GetWidth() const override { return m_CreateInfo.Width; }
		virtual uint32_t GetHeight() const override { return m_CreateInfo.Height; }

		virtual Shared<Image2D> GetImage(uint32_t InAttachmentIndex, uint32_t InImageIndex) const override;

		virtual void TransitionImages(CommandList* InCommandList, EImageState InColorAttachmentState, EImageState InDepthStencilState) override;

		virtual void* GetColorTextureID(uint32_t InColorAttachmentIdx) const override;

		virtual const RenderTextureInfo& GetInfo() const override { return m_CreateInfo; }

		auto& GetColorAttachments() { return m_Attachments; }
		const auto& GetColorAttachments() const { return m_Attachments; }

	private:
		RenderTextureInfo m_CreateInfo{};
		std::vector<D3D12RenderTextureAttachment> m_Attachments;
		D3D12RenderTextureAttachment m_DepthStencilAttachment{};
		bool m_HasDepthStencilAttachment = false;

		D3DComPtr<ID3D12DescriptorHeap> m_RenderTargetDescriptorHeap;
		D3DComPtr<ID3D12DescriptorHeap> m_DepthStencilDescriptorHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_RenderTargetViewHandle{};
		uint32_t m_RenderTargetViewHandleIncrementSize = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE m_DepthStencilViewHandle{};
		uint32_t m_DepthStencilViewHandleIncrementSize = 0;

	};

}
