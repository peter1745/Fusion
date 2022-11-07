#pragma once

#include "Fusion/Renderer/Texture.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11Texture2D : public Texture2D
	{
	public:
		D3D11Texture2D(const Texture2DInfo& InCreateInfo);
		~D3D11Texture2D();

		virtual void Bind(uint32_t InSlot) const override;

		virtual Shared<Image2D> GetImage() const override { return nullptr; }
		virtual const Texture2DInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		Texture2DInfo m_CreateInfo;

		D3DComPtr<ID3D11Texture2D> m_TextureBuffer;
		D3DComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
		D3DComPtr<ID3D11SamplerState> m_SamplerState;

		D3D11_MAPPED_SUBRESOURCE m_MappedSubresource;
	};

}
