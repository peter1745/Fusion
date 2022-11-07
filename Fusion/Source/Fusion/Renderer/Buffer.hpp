#pragma once

#include "Fusion/Core/Core.hpp"
#include "Fusion/Core/Enum.hpp"
#include "Fusion/Memory/Shared.hpp"

namespace Fusion {

	using EBufferState = Flags<>;

	enum class EHeapType { Default, Upload, Readback };

	namespace BufferStates
	{
		static constexpr EBufferState Common = (1 << 0);
		static constexpr EBufferState Vertex = (1 << 1);
		static constexpr EBufferState Constant = (1 << 2);
		static constexpr EBufferState Index = (1 << 3);
		static constexpr EBufferState UnorderedAccess = (1 << 4);
		static constexpr EBufferState NonPixelShaderResource = (1 << 5);
		static constexpr EBufferState IndirectArgument = (1 << 6);
		static constexpr EBufferState CopyDestination = (1 << 7);
		static constexpr EBufferState CopySource = (1 << 8);
	}

	struct BufferInfo
	{
		EHeapType HeapType;
		EBufferState State;
		uint64_t Size;
		uint64_t Alignment = 1;

		void* InitialData = nullptr;
	};

	class CommandList;

	class Buffer : public SharedObject
	{
	public:
		virtual ~Buffer() = default;

		virtual Byte* Map() = 0;
		virtual void Unmap(Byte* InPtr) = 0;

		virtual void Transition(CommandList* InCmdList, EBufferState InState) = 0;

		virtual void SetData(CommandList* InCmdList, const void* InData, const Shared<Buffer>& InUploadBuffer) = 0;

		virtual const BufferInfo& GetInfo() const = 0;
		virtual uint64_t GetSize() const = 0;

	public:
		static Shared<Buffer> Create(const BufferInfo& InCreateInfo);
	};

}
