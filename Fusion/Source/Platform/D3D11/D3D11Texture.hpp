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

		virtual void CopyFrom(const Shared<Texture2D>& InOther) override;
		virtual void CopyRegion(const Shared<Texture2D>& InOther, const RegionCopyData& InRegion) override;
		
		virtual void MapRead() override;
		virtual TextureBuffer Read() override;
		virtual void Unmap() override;

	private:
		Texture2DInfo m_CreateInfo;

		D3DComPtr<ID3D11Texture2D> m_TextureBuffer;
		D3DComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
		D3DComPtr<ID3D11SamplerState> m_SamplerState;

		D3D11_MAPPED_SUBRESOURCE m_MappedSubresource;
	};

}
