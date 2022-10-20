#pragma once

#include "KeyCodes.h"
#include "EButtonState.h"

#include <array>

namespace Fusion {

	struct KeyData
	{
		EButtonState CurrentState = EButtonState::None;
		EButtonState OldState = EButtonState::None;
	};

	class Keyboard
	{
	public:
		const KeyData& GetKeyData(EKeyCode InKeyCode) const { return m_KeyStates[(size_t)InKeyCode]; }
		
		bool IsKeyPressed(EKeyCode InKeyCode) const { return m_KeyStates[(size_t)InKeyCode].CurrentState == EButtonState::Pressed; }
		bool IsKeyHeld(EKeyCode InKeyCode) const { return m_KeyStates[(size_t)InKeyCode].CurrentState == EButtonState::Held; }
		bool IsKeyReleased(EKeyCode InKeyCode) const { return m_KeyStates[(size_t)InKeyCode].CurrentState == EButtonState::Released; }

		static Keyboard& Get();

	private:
		KeyData& GetKeyData(EKeyCode InKeyCode) { return m_KeyStates[(size_t)InKeyCode]; }

		void TransitionHeldKeys();
		void ResetReleasedKeys();

	private:
		std::array<KeyData, (size_t)EKeyCode::COUNT> m_KeyStates;

		friend class Application;
		friend class Window;
	};

}
