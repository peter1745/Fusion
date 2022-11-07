#pragma once

#include "Fusion/Renderer/Texture.hpp"
#include "Fusion/Renderer/DescriptorHeap.hpp"

#include "D3D12Image2D.hpp"
#include "D3D12Buffer.hpp"

namespace Fusion {

	class D3D12Texture2D : public Texture2D
	{
	public:
		D3D12Texture2D(const Texture2DInfo& InCreateInfo);

		virtual void Bind(uint32_t InSlot) const override;

		virtual Shared<Image2D> GetImage() const override { return m_Image; }
		virtual const Texture2DInfo& GetInfo() const override { return m_CreateInfo; }

	private:
		Texture2DInfo m_CreateInfo;
		Shared<D3D12Image2D> m_Image;
		Unique<D3D12Buffer> m_UploadBuffer;
		DescriptorHeapAllocation m_ShaderResourceViewAllocation;
	};

}
