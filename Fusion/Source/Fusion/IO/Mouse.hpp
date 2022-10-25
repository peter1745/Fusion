#pragma once

#include "EButtonState.hpp"

#include <glm/glm.hpp>
#include <array>

namespace Fusion {

	enum class EMouseButton
	{
		Button0,
		Button1,
		Button2,
		Button3,
		Button4,
		Button5,
		Button6,
		Button7,
		COUNT,

		Left = Button0,
		Right = Button1,
		Middle = Button2
	};

	struct MouseButtonState
	{
		EButtonState CurrentState = EButtonState::None;
		EButtonState OldState = EButtonState::None;
	};

	enum class EMouseVisibility
	{
		Visible, Hidden, Locked
	};

	class Mouse
	{
	public:
		const MouseButtonState& GetMouseButtonState(EMouseButton InButton) const { return m_ButtonStates[(size_t)InButton]; }

		bool IsButtonPressed(EMouseButton InButton) const { return m_ButtonStates[(size_t)InButton].CurrentState == EButtonState::Pressed; }
		bool IsButtonHeld(EMouseButton InButton) const { return m_ButtonStates[(size_t)InButton].CurrentState == EButtonState::Held; }
		bool IsButtonReleased(EMouseButton InButton) const { return m_ButtonStates[(size_t)InButton].CurrentState == EButtonState::Released; }

		EMouseVisibility GetVisibility() const { return m_Visibility; }
		void SetVisibility(EMouseVisibility InVisibility) { m_Visibility = InVisibility; }

		const glm::vec2& GetPosition() const { return m_Position; }

		static Mouse& Get();

	private:
		void TransitionHeldButtons();
		void ResetReleasedButtons();

	private:
		glm::vec2 m_Position;
		std::array<MouseButtonState, (size_t)EMouseButton::COUNT> m_ButtonStates;
		EMouseVisibility m_Visibility = EMouseVisibility::Visible;

		friend class Application;
		friend class Window;
	};

}
