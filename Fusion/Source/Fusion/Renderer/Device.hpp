#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "CommandList.hpp"

namespace Fusion {

	class Device : public SharedObject
	{
	public:
		virtual ~Device() = default;

		virtual void Wait() const = 0;

		virtual void Release() = 0;
	};

}
