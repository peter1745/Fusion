#include "FusionPCH.hpp"
#include "Keyboard.hpp"

#include "Fusion/Core/Application.hpp"

namespace Fusion {

	Keyboard& Keyboard::Get()
	{
		static Keyboard s_Instance;
		return s_Instance;
	}
		
	void Keyboard::TransitionHeldKeys()
	{
		for (KeyData& Data : m_KeyStates)
		{
			if (Data.CurrentState == EButtonState::Pressed)
			{
				Data.OldState = Data.CurrentState;
				Data.CurrentState = EButtonState::Held;
			}
		}
	}

	void Keyboard::ResetReleasedKeys()
	{
		for (KeyData& Data : m_KeyStates)
		{
			if (Data.CurrentState == EButtonState::Released)
			{
				Data.OldState = Data.CurrentState;
				Data.CurrentState = EButtonState::None;
			}
		}
	}

}
