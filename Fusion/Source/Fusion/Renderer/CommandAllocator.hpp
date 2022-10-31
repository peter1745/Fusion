#pragma once

#include "CommandList.hpp"

namespace Fusion {

	struct CommandAllocatorInfo
	{
		ECommandListType ListType;
		size_t InitialListCount = 0;
	};

	class CommandAllocator : public SharedObject
	{
	public:
		virtual ~CommandAllocator() = default;

		virtual Shared<CommandList> AllocateCommandList() = 0;
		virtual std::vector<Shared<CommandList>> AllocateCommandLists(size_t InCount) = 0;
		virtual Shared<CommandList> GetCommandList(size_t InIndex) const = 0;

		virtual void Reset() = 0;
	};

}
