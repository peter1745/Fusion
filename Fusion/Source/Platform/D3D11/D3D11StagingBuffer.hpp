#pragma once

#include "Fusion/Renderer/StagingBuffer.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11StagingBuffer : public StagingBuffer
	{
	public:
		D3D11StagingBuffer(const StagingBufferInfo& InCreateInfo);
		~D3D11StagingBuffer();

		virtual void CopyFrom(CommandList* InCmdList, const Shared<Image2D>& InImage, const CopyRegionInfo& InCopyRegion) override;

		virtual Byte* Map() override;
		virtual void Unmap(Byte* InPtr) override;

	private:
		StagingBufferInfo m_CreateInfo;
		D3DComPtr<ID3D11Texture2D> m_Buffer;
		//D3DComPtr<ID3D11Buffer> m_Buffer;
	};

}
