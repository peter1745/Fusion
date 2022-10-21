#include "FusionPCH.h"
#include "D3D11Texture.h"
#include "D3D11Context.h"

#include <stb_image/stb_image.h>

namespace Fusion {

	D3D11Texture2D::D3D11Texture2D(const Texture2DInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Shared<D3D11Context> D3DContext = GraphicsContext::Get<D3D11Context>();

		D3D11_TEXTURE2D_DESC TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		TextureDesc.Width = InCreateInfo.Width;
		TextureDesc.Height = InCreateInfo.Height;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Replace with format from file
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitialData;
		ZeroMemory(&InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
		if (InCreateInfo.Data != nullptr)
		{
			InitialData.pSysMem = InCreateInfo.Data;
			InitialData.SysMemPitch = InCreateInfo.Width * InCreateInfo.Channels;
			InitialData.SysMemSlicePitch = InitialData.SysMemPitch * InCreateInfo.Height;
		}

		D3DContext->GetDevice()->CreateTexture2D(&TextureDesc, &InitialData, &m_TextureBuffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		ShaderResourceViewDesc.Format = TextureDesc.Format;
		ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceViewDesc.Texture2D.MipLevels = 1;
		D3DContext->GetDevice()->CreateShaderResourceView(m_TextureBuffer, &ShaderResourceViewDesc, &m_ShaderResourceView);

		D3D11_SAMPLER_DESC TextureSamplerDesc;
		ZeroMemory(&TextureSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
		TextureSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		TextureSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		TextureSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		TextureSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		TextureSamplerDesc.MipLODBias = 0.0f;
		TextureSamplerDesc.MaxAnisotropy = 1;
		TextureSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		TextureSamplerDesc.BorderColor[0] = 0.0f;
		TextureSamplerDesc.BorderColor[1] = 0.0f;
		TextureSamplerDesc.BorderColor[2] = 0.0f;
		TextureSamplerDesc.BorderColor[3] = 0.0f;
		TextureSamplerDesc.MinLOD = -FLT_MAX;
		TextureSamplerDesc.MaxLOD = FLT_MAX;
		D3DContext->GetDevice()->CreateSamplerState(&TextureSamplerDesc, &m_SamplerState);

		stbi_image_free(m_CreateInfo.Data);
		m_CreateInfo.Data = nullptr;
	}

	D3D11Texture2D::~D3D11Texture2D()
	{
		FUSION_RELEASE_COM(m_SamplerState);
		FUSION_RELEASE_COM(m_ShaderResourceView);
		FUSION_RELEASE_COM(m_TextureBuffer);
	}

	void D3D11Texture2D::Bind(uint32_t InSlot) const
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->PSSetShaderResources(InSlot, 1, &m_ShaderResourceView);
		DeviceContext->PSSetSamplers(InSlot, 1, &m_SamplerState);
	}

}
