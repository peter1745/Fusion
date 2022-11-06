#pragma once

#include "Fusion/Renderer/StagingBuffer.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12StagingBuffer : public StagingBuffer
	{
	public:
		D3D12StagingBuffer(const StagingBufferInfo& InCreateInfo);
		~D3D12StagingBuffer();

		virtual void CopyFrom(CommandList* InCmdList, const Shared<Image2D>& InImage, const CopyRegionInfo& InCopyRegion) override;

		virtual Byte* Map() override;
		virtual void Unmap(Byte* InPtr) override;

	private:
		StagingBufferInfo m_CreateInfo;
		D3DComPtr<ID3D12Resource2> m_Buffer;
	};

}
