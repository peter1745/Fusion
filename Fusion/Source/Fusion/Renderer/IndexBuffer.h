#pragma once

namespace Fusion {

	class IndexBuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual uint32_t GetCount() const = 0;

	};

}
