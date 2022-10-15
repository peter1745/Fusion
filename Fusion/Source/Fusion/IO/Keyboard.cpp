#include "FusionPCH.h"
#include "Keyboard.h"
#include "GLFWKeyMappings.h"

#include "Fusion/Core/Application.h"

namespace Fusion {

	Keyboard& Keyboard::Get()
	{
		static Keyboard* s_Instance = new Keyboard();
		return *s_Instance;
	}
		
	void Keyboard::TransitionHeldKeys()
	{
		for (KeyData& Data : m_KeyStates)
		{
			if (Data.CurrentState == EKeyState::Pressed)
			{
				Data.OldState = Data.CurrentState;
				Data.CurrentState = EKeyState::Held;
			}
		}
	}

	void Keyboard::ResetReleasedKeys()
	{
		for (KeyData& Data : m_KeyStates)
		{
			if (Data.CurrentState == EKeyState::Released)
			{
				Data.OldState = Data.CurrentState;
				Data.CurrentState = EKeyState::None;
			}
		}
	}

}
