#include "FusionPCH.hpp"
#include "D3D11RenderTexture.hpp"
#include "D3D11Context.hpp"

#include <thread>
using namespace std::chrono_literals;

namespace Fusion {

	D3D11RenderTexture::D3D11RenderTexture(const RenderTextureInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Invalidate();
	}

	D3D11RenderTexture::~D3D11RenderTexture()
	{
		m_DepthStencilView.Release();
		m_DepthStencilImage.Reset();

		m_AttachmentRenderTargetViews.clear();
		m_AttachmentImages.clear();
	}

	void D3D11RenderTexture::Bind(CommandList* InCommandList)
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();

		std::vector<ID3D11RenderTargetView*> RenderTargetViews(m_AttachmentRenderTargetViews.size());
		for (size_t Idx = 0; Idx < m_AttachmentRenderTargetViews.size(); Idx++)
			RenderTargetViews[Idx] = m_AttachmentRenderTargetViews[Idx];
		DeviceContext->OMSetRenderTargets(UINT(RenderTargetViews.size()), RenderTargetViews.data(), m_DepthStencilView);

		for (size_t Idx = 0; Idx < m_AttachmentRenderTargetViews.size(); Idx++)
			DeviceContext->ClearRenderTargetView(m_AttachmentRenderTargetViews[Idx], glm::value_ptr(m_CreateInfo.ColorAttachments[Idx].ClearColor));

		if (m_HasDepthStencilAttachment)
			DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void D3D11RenderTexture::Unbind(CommandList* InCommandList)
	{
	}

	void D3D11RenderTexture::Clear()
	{
	}

	void D3D11RenderTexture::Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const ImageSize& InSize)
	{
		auto Device = GraphicsContext::Get<D3D11Context>()->GetDevice();

		m_CreateInfo.Width = InSize.Width;
		m_CreateInfo.Height = InSize.Height;

		m_AttachmentRenderTargetViews[InAttachmentIndex].Release();
		m_AttachmentImages[InAttachmentIndex]->Resize(InSize);

		const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[InAttachmentIndex];

		D3D11_RENDER_TARGET_VIEW_DESC ColorAttachmentViewDesc;
		ZeroMemory(&ColorAttachmentViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		ColorAttachmentViewDesc.Format = EFormatToDXGIFormat(AttachmentInfo.Format);
		ColorAttachmentViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		ColorAttachmentViewDesc.Texture2D.MipSlice = 0;
		Device->CreateRenderTargetView(m_AttachmentImages[InAttachmentIndex]->GetResource(), &ColorAttachmentViewDesc, m_AttachmentRenderTargetViews[InAttachmentIndex]);

		if (m_HasDepthStencilAttachment)
		{
			m_DepthStencilView.Release();
			m_DepthStencilImage->Resize(InSize);
			Device->CreateDepthStencilView(m_DepthStencilImage->GetResource(), nullptr, m_DepthStencilView);
		}
	}

	Shared<Image2D> D3D11RenderTexture::GetImage(uint32_t InAttachmentIndex, uint32_t InImageIndex) const
	{
		return m_AttachmentImages[InAttachmentIndex];
	}

	void D3D11RenderTexture::Invalidate()
	{
		auto Context = GraphicsContext::Get<D3D11Context>();
		auto Device = Context->GetDevice();

		m_DepthStencilView.Release();
		m_DepthStencilImage.Reset();

		m_AttachmentRenderTargetViews.clear();
		m_AttachmentImages.clear();

		m_AttachmentRenderTargetViews.resize(m_CreateInfo.ColorAttachments.size());
		m_AttachmentImages.reserve(m_CreateInfo.ColorAttachments.size());

		for (size_t Idx = 0; Idx < m_CreateInfo.ColorAttachments.size(); Idx++)
		{
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[Idx];

			Image2DInfo ImageInfo = {};
			ImageInfo.Size = { m_CreateInfo.Width, m_CreateInfo.Height };
			ImageInfo.Usage = EImageUsage::Attachment;
			ImageInfo.Format = AttachmentInfo.Format;
			ImageInfo.Flags = AttachmentInfo.Flags;
			ImageInfo.InitialState = AttachmentInfo.InitialState;
			ImageInfo.ClearColor = AttachmentInfo.ClearColor;
			m_AttachmentImages.push_back(Shared<D3D11Image2D>::Create(ImageInfo));

			D3D11_RENDER_TARGET_VIEW_DESC ColorAttachmentViewDesc;
			ZeroMemory(&ColorAttachmentViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
			ColorAttachmentViewDesc.Format = EFormatToDXGIFormat(AttachmentInfo.Format);
			ColorAttachmentViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			ColorAttachmentViewDesc.Texture2D.MipSlice = 0;
			Device->CreateRenderTargetView(m_AttachmentImages[Idx]->GetResource(), &ColorAttachmentViewDesc, m_AttachmentRenderTargetViews[Idx]);

			/*D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
			ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			ShaderResourceViewDesc.Format = ColorAttachmentDesc.Format;
			ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			ShaderResourceViewDesc.Texture2D.MipLevels = 1;
			Device->CreateShaderResourceView(m_ColorAttachmentTextures[Idx], &ShaderResourceViewDesc, &m_ColorAttachmentResourceViews[Idx]);*/
		}

		m_HasDepthStencilAttachment = IsDepthFormat(m_CreateInfo.DepthAttachment.Format);

		// Create depth stencil buffer if necessary
		if (m_HasDepthStencilAttachment)
		{
			const auto& AttachmentInfo = m_CreateInfo.DepthAttachment;

			Image2DInfo DepthImageInfo = {};
			DepthImageInfo.Size = { m_CreateInfo.Width, m_CreateInfo.Height };
			DepthImageInfo.Usage = EImageUsage::Attachment;
			DepthImageInfo.Format = AttachmentInfo.Format;
			DepthImageInfo.Flags = AttachmentInfo.Flags;
			DepthImageInfo.InitialState = AttachmentInfo.InitialState;
			DepthImageInfo.ClearColor = AttachmentInfo.ClearColor;
			m_DepthStencilImage = Shared<D3D11Image2D>::Create(DepthImageInfo);
			Device->CreateDepthStencilView(m_DepthStencilImage->GetResource(), nullptr, m_DepthStencilView);
		}
	}

}
