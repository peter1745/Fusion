#pragma once

#include "CommandList.hpp"

namespace Fusion {

	struct CommandAllocatorInfo
	{
		ECommandListType ListType;
		size_t InitialListCount = 0;
	};

	// NOTE(Peter): CommandAllocator should own the command list abstraction instance
	class CommandAllocator : public SharedObject
	{
	public:
		virtual ~CommandAllocator() = default;

		virtual CommandList* AllocateCommandList() = 0;
		virtual std::vector<CommandList*> AllocateCommandLists(size_t InCount) = 0;
		virtual CommandList* GetCommandList(size_t InIndex) const = 0;

		virtual void Reset() = 0;
	};

}
