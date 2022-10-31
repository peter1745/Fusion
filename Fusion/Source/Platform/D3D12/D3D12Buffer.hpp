#pragma once

#include "Fusion/Renderer/Buffer.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	enum class EHeapType { Default, Upload };

	enum class EBufferState
	{
		Common,
		VertexAndConstant,
		Index,
		UnorderedAccess,
		NonPixelShaderResource,
		IndirectArgument,
		CopyDestination,
		CopySource
	};

	struct BufferInfo
	{
		EHeapType HeapType;
		Enum<EBufferState> State;
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
