#include "FusionPCH.hpp"
#include "D3D11RenderTexture.hpp"
#include "D3D11Context.hpp"

#include <thread>
using namespace std::chrono_literals;

namespace Fusion {

	/*static D3D11_USAGE AttachmentUsageToD3D11Usage(ERenderTextureAttachmentUsage InUsage)
	{
		switch (InUsage)
		{
		case ERenderTextureAttachmentUsage::Default: return D3D11_USAGE_DEFAULT;
		case ERenderTextureAttachmentUsage::Staging: return D3D11_USAGE_STAGING;
		}

		FUSION_CORE_VERIFY(false);
		return D3D11_USAGE_DEFAULT;
	}*/

	D3D11RenderTexture::D3D11RenderTexture(const RenderTextureInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Invalidate();
	}

	D3D11RenderTexture::~D3D11RenderTexture()
	{
		FUSION_RELEASE_COM(m_DepthStencilView);
		FUSION_RELEASE_COM(m_DepthStencilTexture);

		for (auto& [AttachmentIdx, StagingTexture] : m_StagingTextures)
			FUSION_RELEASE_COM(StagingTexture);
		m_StagingTextures.clear();

		for (size_t Idx = 0; Idx < m_ColorAttachmentTextures.size(); Idx++)
		{
			FUSION_RELEASE_COM(m_ColorAttachmentViews[Idx]);
			FUSION_RELEASE_COM(m_ColorAttachmentTextures[Idx]);
		}
	}

	void D3D11RenderTexture::Bind()
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->RSSetViewports(1, &m_Viewport);
		DeviceContext->OMSetRenderTargets(UINT(m_ColorAttachmentViews.size()), m_ColorAttachmentViews.data(), m_DepthStencilView);
	}

	void D3D11RenderTexture::Unbind()
	{
	}

	void D3D11RenderTexture::Clear()
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();

		for (size_t Idx = 0; Idx < m_ColorAttachmentViews.size(); Idx++)
			DeviceContext->ClearRenderTargetView(m_ColorAttachmentViews[Idx], glm::value_ptr(m_CreateInfo.ColorAttachments[Idx].ClearColor));

		DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void D3D11RenderTexture::Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const AttachmentSize& InSize)
	{
		/*if (InWidth == 0 || InHeight == 0)
			return;

		m_CreateInfo.Width = InWidth;
		m_CreateInfo.Height = InHeight;

		Invalidate();*/
	}

	uint64_t D3D11RenderTexture::ReadPixel(uint32_t InAttachmentIdx, uint32_t InX, uint32_t InY)
	{
		FUSION_CORE_VERIFY(m_StagingTextures.find(InAttachmentIdx) != m_StagingTextures.end());

		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();

		ID3D11Texture2D* StagingTexture = m_StagingTextures[InAttachmentIdx];
		ID3D11Texture2D* AttachmentTexture = m_ColorAttachmentTextures[InAttachmentIdx];
		DeviceContext->CopyResource(StagingTexture, AttachmentTexture);

		D3D11_MAPPED_SUBRESOURCE MappedResource;
		ZeroMemory(&MappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		DeviceContext->Map(StagingTexture, 0, D3D11_MAP_READ, 0, &MappedResource);
		uint64_t PixelValue = *(reinterpret_cast<uint64_t*>(MappedResource.pData) + (InX + InY * MappedResource.RowPitch / 8));
		DeviceContext->Unmap(StagingTexture, 0);

		return PixelValue;
	}

	void D3D11RenderTexture::Invalidate()
	{
		ID3D11Device* Device = GraphicsContext::Get<D3D11Context>()->GetDevice();

		FUSION_RELEASE_COM(m_DepthStencilView);
		FUSION_RELEASE_COM(m_DepthStencilTexture);

		for (auto& [AttachmentIdx, StagingTexture] : m_StagingTextures)
			FUSION_RELEASE_COM(StagingTexture);
		m_StagingTextures.clear();

		for (size_t Idx = 0; Idx < m_ColorAttachmentTextures.size(); Idx++)
		{
			FUSION_RELEASE_COM(m_ColorAttachmentViews[Idx]);
			FUSION_RELEASE_COM(m_ColorAttachmentTextures[Idx]);
		}
		m_ColorAttachmentTextures.clear();


		m_ColorAttachmentTextures.resize(m_CreateInfo.ColorAttachments.size());
		m_ColorAttachmentViews.resize(m_CreateInfo.ColorAttachments.size());
		m_ColorAttachmentResourceViews.resize(m_CreateInfo.ColorAttachments.size());

		for (size_t Idx = 0; Idx < m_CreateInfo.ColorAttachments.size(); Idx++)
		{
			const auto& AttachmentInfo = m_CreateInfo.ColorAttachments[Idx];

			D3D11_TEXTURE2D_DESC ColorAttachmentDesc;
			ZeroMemory(&ColorAttachmentDesc, sizeof(D3D11_TEXTURE2D_DESC));
			ColorAttachmentDesc.Width = m_CreateInfo.Width;
			ColorAttachmentDesc.Height = m_CreateInfo.Height;
			ColorAttachmentDesc.MipLevels = 1;
			ColorAttachmentDesc.ArraySize = 1;
			//ColorAttachmentDesc.Format = AttachmentFormatToDXGIFormat(AttachmentInfo.Format);
			ColorAttachmentDesc.SampleDesc.Count = 1;
			ColorAttachmentDesc.Usage = D3D11_USAGE_DEFAULT;// AttachmentUsageToD3D11Usage(AttachmentInfo.Usage);
			ColorAttachmentDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			ColorAttachmentDesc.CPUAccessFlags = 0;
			ColorAttachmentDesc.MiscFlags = 0;
			Device->CreateTexture2D(&ColorAttachmentDesc, nullptr, &m_ColorAttachmentTextures[Idx]);

			D3D11_RENDER_TARGET_VIEW_DESC ColorAttachmentViewDesc;
			ZeroMemory(&ColorAttachmentViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
			ColorAttachmentViewDesc.Format = ColorAttachmentDesc.Format;
			ColorAttachmentViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			ColorAttachmentViewDesc.Texture2D.MipSlice = 0;
			Device->CreateRenderTargetView(m_ColorAttachmentTextures[Idx], &ColorAttachmentViewDesc, &m_ColorAttachmentViews[Idx]);

			D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
			ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			ShaderResourceViewDesc.Format = ColorAttachmentDesc.Format;
			ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			ShaderResourceViewDesc.Texture2D.MipLevels = 1;
			Device->CreateShaderResourceView(m_ColorAttachmentTextures[Idx], &ShaderResourceViewDesc, &m_ColorAttachmentResourceViews[Idx]);

			/*if (AttachmentInfo.RequiresCPUAccess)
			{
				D3D11_TEXTURE2D_DESC StagingTextureDesc;
				ZeroMemory(&StagingTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
				StagingTextureDesc.Width = m_CreateInfo.Width;
				StagingTextureDesc.Height = m_CreateInfo.Height;
				StagingTextureDesc.MipLevels = 1;
				StagingTextureDesc.ArraySize = 1;
				StagingTextureDesc.Format = ColorAttachmentDesc.Format;
				StagingTextureDesc.SampleDesc.Count = 1;
				StagingTextureDesc.Usage = D3D11_USAGE_STAGING;
				StagingTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				StagingTextureDesc.MiscFlags = 0;
				Device->CreateTexture2D(&StagingTextureDesc, nullptr, &m_StagingTextures[Idx]);
			}*/
		}

		// Create depth stencil buffer if necessary
		//if (m_CreateInfo.DepthAttachment.Format == ERenderTextureAttachmentFormat::Depth24)
		//{
		//	D3D11_TEXTURE2D_DESC DepthStencilDesc;
		//	ZeroMemory(&DepthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
		//	DepthStencilDesc.Width = m_CreateInfo.Width;
		//	DepthStencilDesc.Height = m_CreateInfo.Height;
		//	DepthStencilDesc.MipLevels = 1;
		//	DepthStencilDesc.ArraySize = 1;
		//	DepthStencilDesc.Format = AttachmentFormatToDXGIFormat(m_CreateInfo.DepthAttachment.Format);
		//	DepthStencilDesc.SampleDesc.Count = 1;
		//	DepthStencilDesc.SampleDesc.Quality = 0;
		//	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;// AttachmentUsageToD3D11Usage(m_CreateInfo.DepthAttachment.Usage);
		//	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//	DepthStencilDesc.CPUAccessFlags = 0;
		//	DepthStencilDesc.MiscFlags = 0;

		//	Device->CreateTexture2D(&DepthStencilDesc, NULL, &m_DepthStencilTexture);
		//	Device->CreateDepthStencilView(m_DepthStencilTexture, NULL, &m_DepthStencilView);
		//}

		ZeroMemory(&m_Viewport, sizeof(D3D11_VIEWPORT));
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = static_cast<float>(m_CreateInfo.Width);
		m_Viewport.Height = static_cast<float>(m_CreateInfo.Height);
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
	}

}
