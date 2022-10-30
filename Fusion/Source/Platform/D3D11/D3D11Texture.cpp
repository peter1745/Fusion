#include "FusionPCH.hpp"
#include "D3D11Texture.hpp"
#include "D3D11Context.hpp"

#include <stb_image/stb_image.h>

namespace Fusion {

	static DXGI_FORMAT TextureFormatToDXGIFormat(ETextureFormat InFormat)
	{
		switch (InFormat)
		{
		case ETextureFormat::None: return DXGI_FORMAT_UNKNOWN;
		case ETextureFormat::RGBA8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ETextureFormat::R32UInt: return DXGI_FORMAT_R32_UINT;
		case ETextureFormat::R32G32UInt: return DXGI_FORMAT_R32G32_UINT;
		case ETextureFormat::D24UnormS8UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	static D3D11_USAGE TextureUsageToD3D11Usage(ETextureUsage InUsage)
	{
		switch (InUsage)
		{
		case ETextureUsage::Default: return D3D11_USAGE_DEFAULT;
		case ETextureUsage::Staging: return D3D11_USAGE_STAGING;
		}

		return D3D11_USAGE_DEFAULT;
	}

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
		TextureDesc.Format = TextureFormatToDXGIFormat(InCreateInfo.Format);
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.Usage = TextureUsageToD3D11Usage(InCreateInfo.Usage);

		if (InCreateInfo.Usage == ETextureUsage::Default)
			TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (InCreateInfo.IsRenderTextureAttachment ? D3D11_BIND_RENDER_TARGET : 0);



		switch (InCreateInfo.Usage)
		{
		case ETextureUsage::Default:
		{
			TextureDesc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitialData;
			ZeroMemory(&InitialData, sizeof(D3D11_SUBRESOURCE_DATA));
			if (InCreateInfo.Data != nullptr)
			{
				InitialData.pSysMem = InCreateInfo.Data;
				InitialData.SysMemPitch = InCreateInfo.Width * InCreateInfo.Channels;
				InitialData.SysMemSlicePitch = InitialData.SysMemPitch * InCreateInfo.Height;
			}

			D3DContext->GetDevice()->CreateTexture2D(&TextureDesc, &InitialData, &m_TextureBuffer);
			m_CreateInfo.Data = nullptr;
			break;
		}
		case ETextureUsage::Staging:
		{
			TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			D3DContext->GetDevice()->CreateTexture2D(&TextureDesc, nullptr, &m_TextureBuffer);
			break;
		}
		}

		if (TextureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
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
		}
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

	void D3D11Texture2D::CopyFrom(const Shared<Texture2D>& InOther)
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		Shared<D3D11Texture2D> D3DTexture = InOther.As<D3D11Texture2D>();
		DeviceContext->CopyResource(m_TextureBuffer, D3DTexture->m_TextureBuffer);
	}

	void D3D11Texture2D::CopyRegion(const Shared<Texture2D>& InOther, const RegionCopyData& InRegion)
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		Shared<D3D11Texture2D> D3DTexture = InOther.As<D3D11Texture2D>();

		D3D11_BOX SourceRegion;
		SourceRegion.left = InRegion.SourceX;
		SourceRegion.right = InRegion.SourceX + InRegion.SourceWidth;
		SourceRegion.top = InRegion.SourceY;
		SourceRegion.bottom = InRegion.SourceY + InRegion.SourceHeight;

		DeviceContext->CopySubresourceRegion(m_TextureBuffer, 0, InRegion.DestinationX, InRegion.DestinationY, 0, D3DTexture->m_TextureBuffer, 0, &SourceRegion);
	}

	void D3D11Texture2D::MapRead()
	{
		FUSION_CORE_VERIFY(m_MappedSubresource.pData == nullptr, "Cannot map texture while it's already mapped!");
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->Map(m_TextureBuffer, 0, D3D11_MAP_READ, 0, &m_MappedSubresource);
	}

	TextureBuffer D3D11Texture2D::Read()
	{
		TextureBuffer Result;
		FUSION_CORE_VERIFY(m_MappedSubresource.pData, "Cannot read from an unmapped texture!");
		Result.Data = reinterpret_cast<Byte*>(m_MappedSubresource.pData);
		Result.RowPitch = m_MappedSubresource.RowPitch;
		return Result;
	}

	void D3D11Texture2D::Unmap()
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->Unmap(m_TextureBuffer, 0);
		ZeroMemory(&m_MappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	}

}
