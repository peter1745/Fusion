#pragma once

namespace Fusion {

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual uint32_t GetCount() const = 0;

	public:
		static Shared<IndexBuffer> Create(uint32_t InSize, void* InData);
	};

}
