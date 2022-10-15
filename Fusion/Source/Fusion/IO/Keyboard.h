#pragma once

#include "KeyCodes.h"

#include <array>

namespace Fusion {

	enum class EKeyState
	{
		None = -1,
		Pressed,
		Held,
		Released
	};

	struct KeyData
	{
		EKeyState CurrentState = EKeyState::None;
		EKeyState OldState = EKeyState::None;
	};

	class Keyboard
	{
	public:
		const KeyData& GetKeyData(EKeyCode InKeyCode) const { return m_KeyStates[(size_t)InKeyCode]; }

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
