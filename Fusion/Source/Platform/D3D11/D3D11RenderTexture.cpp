#include "FusionPCH.h"
#include "D3D11RenderTexture.h"
#include "D3D11Context.h"

namespace Fusion {

	D3D11RenderTexture::D3D11RenderTexture(const RenderTextureInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Invalidate();
	}

	D3D11RenderTexture::~D3D11RenderTexture()
	{
		FUSION_RELEASE_COM(m_DepthStencilView);
		FUSION_RELEASE_COM(m_DepthStencilBuffer);
		FUSION_RELEASE_COM(m_ShaderResourceView);
		FUSION_RELEASE_COM(m_RenderTargetView);
		FUSION_RELEASE_COM(m_RenderTargetTexture);
	}

	void D3D11RenderTexture::Bind()
	{
		Shared<D3D11Context> D3DContext = GraphicsContext::Get<D3D11Context>();

		// TODO(Peter): We shouldn't always use depth stencil view here. In fact the render texture should support creating it's own depth stencil
		D3DContext->SetRenderTargets(m_RenderTargetView, m_DepthStencilView);
	}

	void D3D11RenderTexture::Unbind()
	{
		Shared<D3D11Context> D3DContext = GraphicsContext::Get<D3D11Context>();
		D3DContext->SetRenderTargets(D3DContext->GetBackBufferView(), nullptr);
	}

	void D3D11RenderTexture::Clear(const glm::vec4& InColor)
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->ClearRenderTargetView(m_RenderTargetView, glm::value_ptr(InColor));
		DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void D3D11RenderTexture::Resize(uint32_t InWidth, uint32_t InHeight)
	{
		if (InWidth == 0 || InHeight == 0)
			return;

		m_CreateInfo.Width = InWidth;
		m_CreateInfo.Height = InHeight;

		Invalidate();
	}

	void D3D11RenderTexture::Invalidate()
	{
		ID3D11Device* Device = GraphicsContext::Get<D3D11Context>()->GetDevice();

		FUSION_RELEASE_COM(m_DepthStencilView);
		FUSION_RELEASE_COM(m_DepthStencilBuffer);
		FUSION_RELEASE_COM(m_ShaderResourceView);
		FUSION_RELEASE_COM(m_RenderTargetView);
		FUSION_RELEASE_COM(m_RenderTargetTexture);

		// Color Buffer
		{
			D3D11_TEXTURE2D_DESC RenderTextureDesc;
			ZeroMemory(&RenderTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
			RenderTextureDesc.Width = m_CreateInfo.Width;
			RenderTextureDesc.Height = m_CreateInfo.Height;
			RenderTextureDesc.MipLevels = 1;
			RenderTextureDesc.ArraySize = 1;
			RenderTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			RenderTextureDesc.SampleDesc.Count = 1;
			RenderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			RenderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			RenderTextureDesc.CPUAccessFlags = 0;
			RenderTextureDesc.MiscFlags = 0;
			Device->CreateTexture2D(&RenderTextureDesc, nullptr, &m_RenderTargetTexture);

			D3D11_RENDER_TARGET_VIEW_DESC RenderTargetViewDesc;
			ZeroMemory(&RenderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
			RenderTargetViewDesc.Format = RenderTextureDesc.Format;
			RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			RenderTargetViewDesc.Texture2D.MipSlice = 0;
			Device->CreateRenderTargetView(m_RenderTargetTexture, &RenderTargetViewDesc, &m_RenderTargetView);

			D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
			ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			ShaderResourceViewDesc.Format = RenderTextureDesc.Format;
			ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			ShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			ShaderResourceViewDesc.Texture2D.MipLevels = 1;
			Device->CreateShaderResourceView(m_RenderTargetTexture, &ShaderResourceViewDesc, &m_ShaderResourceView);
		}

		// Depth Buffer
		{
			D3D11_TEXTURE2D_DESC DepthStencilDesc;
			ZeroMemory(&DepthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
			DepthStencilDesc.Width = m_CreateInfo.Width;
			DepthStencilDesc.Height = m_CreateInfo.Height;
			DepthStencilDesc.MipLevels = 1;
			DepthStencilDesc.ArraySize = 1;
			DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			DepthStencilDesc.SampleDesc.Count = 1;
			DepthStencilDesc.SampleDesc.Quality = 0;
			DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
			DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			DepthStencilDesc.CPUAccessFlags = 0;
			DepthStencilDesc.MiscFlags = 0;

			Device->CreateTexture2D(&DepthStencilDesc, NULL, &m_DepthStencilBuffer);
			Device->CreateDepthStencilView(m_DepthStencilBuffer, NULL, &m_DepthStencilView);
		}
	}

}
