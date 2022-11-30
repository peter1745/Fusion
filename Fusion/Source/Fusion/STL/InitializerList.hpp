#pragma once

#include <initializer_list>
#include <stdlib.h>

namespace Fusion {

	template<typename T>
	class InitializerList
	{
	public:
		InitializerList(const std::initializer_list<T>& InList)
			: m_List(InList)
		{
		}

		size_t Size() const { return m_List.size(); }

		T operator[](size_t InIndex) const { return *(m_List.begin() + InIndex); }

	private:
		std::initializer_list<T> m_List;
	};

}
