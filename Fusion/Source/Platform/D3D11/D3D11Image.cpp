#include "FusionPCH.hpp"
#include "D3D11Image.hpp"
#include "D3D11Context.hpp"

namespace Fusion {

	D3D11Image2D::D3D11Image2D(const Image2DInfo& InCreateInfo)
		: m_CreateInfo(InCreateInfo)
	{
		Invalidate();
	}

	D3D11Image2D::~D3D11Image2D()
	{
	}

	void D3D11Image2D::Invalidate()
	{
		m_Image.Release();

		D3D11_TEXTURE2D_DESC ImageDesc = {};
		ImageDesc.Width = m_CreateInfo.Size.Width;
		ImageDesc.Height = m_CreateInfo.Size.Height;
		ImageDesc.MipLevels = 1;
		ImageDesc.ArraySize = 1;
		ImageDesc.Format = EFormatToDXGIFormat(m_CreateInfo.Format);
		ImageDesc.SampleDesc.Count = 1;
		ImageDesc.Usage = D3D11_USAGE_DEFAULT;
		ImageDesc.BindFlags = 0;
		ImageDesc.CPUAccessFlags = 0;
		ImageDesc.MiscFlags = 0;

		switch (m_CreateInfo.Usage)
		{
		case EImageUsage::Texture:
		{
			ImageDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			break;
		}
		case EImageUsage::Attachment:
		{
			if (IsDepthFormat(m_CreateInfo.Format))
				ImageDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			else
				ImageDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			break;
		}
		}

		GraphicsContext::Get<D3D11Context>()->GetDevice()->CreateTexture2D(&ImageDesc, nullptr, m_Image);
	}

}
