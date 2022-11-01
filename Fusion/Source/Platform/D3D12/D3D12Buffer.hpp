#pragma once

#include "Fusion/Renderer/Buffer.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	using EBufferState = Flags<>;

	enum class EHeapType { Default, Upload };

	namespace BufferStates
	{
		static constexpr EBufferState Common = (1 << 0);
		static constexpr EBufferState VertexAndConstant = (1 << 1);
		static constexpr EBufferState Index = (1 << 2);
		static constexpr EBufferState UnorderedAccess = (1 << 3);
		static constexpr EBufferState NonPixelShaderResource = (1 << 4);
		static constexpr EBufferState IndirectArgument = (1 << 5);
		static constexpr EBufferState CopyDestination = (1 << 6);
		static constexpr EBufferState CopySource = (1 << 7);
	}

	struct BufferInfo
	{
		EHeapType HeapType;
		EBufferState State;
		uint64_t Alignment;
		uint64_t Size;

		void* InitialData = nullptr;
	};

	class D3D12Buffer
	{
	public:
		D3D12Buffer(const BufferInfo& InCreateInfo);
		~D3D12Buffer();

		auto GetGPUBufferLocation() { return m_Buffer->GetGPUVirtualAddress(); }

	private:
		BufferInfo m_CreateInfo;
		D3D12ComPtr<ID3D12Resource2> m_Buffer;
	};

}
