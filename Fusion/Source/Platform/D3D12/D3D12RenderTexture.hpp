#pragma once

#include "Fusion/Core/Enum.hpp"
#include "Fusion/Renderer/RenderTexture.hpp"

#include "D3D12Common.hpp"

#include <vector>

namespace Fusion {

	struct D3D12RenderTextureAttachment
	{
		std::vector<D3D12ComPtr<ID3D12Resource2>> Images;
		std::vector<EImageState> States;
		std::vector<AttachmentSize> Sizes;
	};

	class D3D12RenderTexture : public RenderTexture
	{
	public:
		D3D12RenderTexture(const RenderTextureInfo& InCreateInfo);

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Clear() override;

		virtual AttachmentSize GetImageSize(uint32_t InAttachmentIndex, uint32_t InFrameIndex) const override;
		virtual void Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const AttachmentSize& InSize) override;

		virtual uint64_t ReadPixel(uint32_t InAttachmentIdx, uint32_t InX, uint32_t InY) override { return 0; }

		virtual uint32_t GetWidth() const override { return m_CreateInfo.Width; }
		virtual uint32_t GetHeight() const override { return m_CreateInfo.Height; }

		virtual void TransitionImages(EImageState InColorAttachmentState, EImageState InDepthStencilState) override;

		virtual void* GetColorTextureID(uint32_t InColorAttachmentIdx) const override;

		virtual const RenderTextureInfo& GetInfo() const override { return m_CreateInfo; }

		auto& GetColorAttachments() { return m_Attachments; }
		const auto& GetColorAttachments() const { return m_Attachments; }

	private:
		RenderTextureInfo m_CreateInfo{};
		std::vector<D3D12RenderTextureAttachment> m_Attachments;
		D3D12RenderTextureAttachment m_DepthStencilAttachment{};

		D3D12ComPtr<ID3D12DescriptorHeap> m_RenderTargetDescriptorHeap;
		D3D12ComPtr<ID3D12DescriptorHeap> m_DepthStencilDescriptorHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_RenderTargetViewHandle{};
		uint32_t m_RenderTargetViewHandleIncrementSize = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE m_DepthStencilViewHandle{};
		uint32_t m_DepthStencilViewHandleIncrementSize = 0;

	};

}
