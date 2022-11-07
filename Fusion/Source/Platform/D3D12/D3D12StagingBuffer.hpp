#pragma once

#include "Fusion/Renderer/StagingBuffer.hpp"

#include "D3D12Common.hpp"
#include "D3D12Buffer.hpp"

namespace Fusion {

	class D3D12StagingBuffer : public StagingBuffer
	{
	public:
		D3D12StagingBuffer(const StagingBufferInfo& InCreateInfo);
		~D3D12StagingBuffer();

		virtual void CopyFrom(CommandList* InCmdList, const Shared<Image2D>& InImage, const CopyRegionInfo& InCopyRegion) override;

		virtual Byte* Map() override { return m_Buffer->Map(); }
		virtual void Unmap(Byte* InPtr) override { m_Buffer->Unmap(InPtr); }

	private:
		StagingBufferInfo m_CreateInfo;
		Unique<D3D12Buffer> m_Buffer;
	};

}