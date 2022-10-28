#pragma once

#include "BaseComponent.hpp"

#include <type_traits>

namespace Fusion {

	template<typename TComponent>
	struct ComponentAttributes
	{
		using ComponentType = TComponent;

		static constexpr bool CanDisable = std::is_base_of_v<BaseComponent, TComponent>;
		static constexpr bool Removable = true;
	};

}
