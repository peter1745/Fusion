#pragma once

#include "Fusion/Core/Core.hpp"

#include "CommandList.hpp"
#include "Image.hpp"

namespace Fusion {

	struct StagingBufferInfo
	{
		EFormat Format;
		uint32_t Size;
	};

	struct CopyRegionInfo
	{
		uint32_t Left;
		uint32_t Top;
		uint32_t Right;
		uint32_t Bottom;
	};

	class StagingBuffer
	{
	public:
		virtual ~StagingBuffer() = default;

		virtual void CopyFrom(CommandList* InCmdList, const Shared<Image2D>& InImage, const CopyRegionInfo& InCopyRegion) = 0;

		virtual Byte* Map() = 0;
		virtual void Unmap(Byte* InPtr) = 0;

	public:
		static Unique<StagingBuffer> Create(const StagingBufferInfo& InCreateInfo);
	};

}
