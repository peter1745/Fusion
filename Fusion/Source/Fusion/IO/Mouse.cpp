#include "FusionPCH.hpp"
#include "Mouse.hpp"

namespace Fusion {

	Mouse& Mouse::Get()
	{
		static Mouse s_Instance;
		return s_Instance;
	}

	void Mouse::TransitionHeldButtons()
	{
		for (MouseButtonState& Data : m_ButtonStates)
		{
			if (Data.CurrentState == EButtonState::Pressed)
			{
				Data.OldState = Data.CurrentState;
				Data.CurrentState = EButtonState::Held;
			}
		}
	}

	void Mouse::ResetReleasedButtons()
	{
		for (MouseButtonState& Data : m_ButtonStates)
		{
			if (Data.CurrentState == EButtonState::Released)
			{
				Data.OldState = Data.CurrentState;
				Data.CurrentState = EButtonState::None;
			}
		}
	}

}
