#include "FusionPCH.hpp"
#include "D3D11Texture.hpp"
#include "D3D11Context.hpp"

#include <stb_image/stb_image.h>

namespace Fusion {

	D3D11Texture2D::D3D11Texture2D(const Texture2DInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Shared<D3D11Context> D3DContext = GraphicsContext::Get<D3D11Context>();

		Image2DInfo TextureInfo = {};
		TextureInfo.Size.Width = InCreateInfo.Width;
		TextureInfo.Size.Height = InCreateInfo.Height;
		TextureInfo.Usage = EImageUsage::Texture;
		TextureInfo.Format = InCreateInfo.Format;
		TextureInfo.Flags = 0;
		TextureInfo.InitialData = InCreateInfo.Data;
		TextureInfo.Channels = InCreateInfo.Channels;
		m_Image = Shared<D3D11Image2D>::Create(TextureInfo);

		D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDesc;
		ZeroMemory(&ShaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		ShaderResourceViewDesc.Format = EFormatToDXGIFormat(InCreateInfo.Format);
		ShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ShaderResourceViewDesc.Texture2D.MipLevels = 1;
		D3DContext->GetDevice()->CreateShaderResourceView(m_Image->GetResource(), &ShaderResourceViewDesc, m_ShaderResourceView);
	}

	D3D11Texture2D::~D3D11Texture2D()
	{
	}

	void D3D11Texture2D::Bind(uint32_t InSlot)
	{
		ID3D11DeviceContext* DeviceContext = GraphicsContext::Get<D3D11Context>()->GetDeviceContext();
		DeviceContext->PSSetShaderResources(InSlot, 1, m_ShaderResourceView);
	}

}
