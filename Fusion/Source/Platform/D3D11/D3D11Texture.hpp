#pragma once

#include "Fusion/Renderer/Texture.hpp"

namespace Fusion {

	class D3D11Texture2D : public Texture2D
	{
	public:
		D3D11Texture2D(const Texture2DInfo& InCreateInfo);
		~D3D11Texture2D();

		virtual void Bind(uint32_t InSlot) const override;

	private:
		Texture2DInfo m_CreateInfo;

		ID3D11Texture2D* m_TextureBuffer = nullptr;
		ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
		ID3D11SamplerState* m_SamplerState = nullptr;
	};

}
