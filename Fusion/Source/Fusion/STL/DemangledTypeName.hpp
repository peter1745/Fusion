#pragma once

#if __has_include(<cxxabi.h>)
#define FUSION_ABI_SUPPORTED
#include <cxxabi.h>
#endif

#include <type_traits>
#include <string>
#include <string_view>

namespace Fusion {

	// NOTE(Peter): StringType is used because on non-MSVC compilers we have to make a copy of the buffer we get back from abi::__cxa_demangle
	//				This is because that function allocates a heap buffer that we're responsible for freeing.
	//				But on Windows we can simply return a std::string_view since no heap allocation is necessary.
	//				We could of course handle non-MSVC compilers explicitly just like we do with MSVC, but it's not worth it imo. I may end up merging this with Jay's TypeDescriptor
	//				file if he thinks they fit together.

	template <typename T>
	struct DemangledTypeName
	{
	public:
		DemangledTypeName()
		    : m_DemangledName(DemangleTypeName(typeid(T).name()))
		{
		}

		auto Name() { return m_DemangledName; }
		const auto& Name() const { return m_DemangledName; }

		uint32_t HashCode() const
		{
			const uint32_t FNV_PRIME = 16777619u;
			const uint32_t OFFSET_BASIS = 2166136261u;

			const size_t length = m_DemangledName.length();
			const char* data = m_DemangledName.data();

			uint32_t hash = OFFSET_BASIS;
			
			for (size_t i = 0; i < length; ++i)
			{
				hash ^= *data++;
				hash *= FNV_PRIME;
			}

			hash ^= '\0';
			hash *= FNV_PRIME;

			return hash;
		}

	private:
#if defined(FUSION_ABI_SUPPORTED)
		std::string DemangleTypeName(const std::string& InTypeName) const
		{
			size_t BufferLength = 0;
			int Status = 0;
			char* Buffer = abi::__cxa_demangle(InTypeName.c_str(), NULL, &BufferLength, &Status);
			std::string Result = std::string(Buffer, BufferLength);
			free(Buffer);
			return Result;
		}
#else
		std::string_view DemangleTypeName(std::string_view InTypeName) const
		{
			size_t SpacePos = InTypeName.find(' ');
			if (SpacePos != std::string_view::npos)
				InTypeName.remove_prefix(SpacePos + 1);
			return InTypeName;
		}
#endif

	private:
#if defined(FUSION_ABI_SUPPORTED)
		std::string m_DemangledName;
#else
		std::string_view m_DemangledName;
#endif
	};
}
