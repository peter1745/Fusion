#pragma once

#include "EditorWindow.h"

#include "Fusion/World/Actor.h"

namespace FusionEditor {

	class ActorDetailsWindow : public EditorWindow
	{
	public:
		ActorDetailsWindow();
		~ActorDetailsWindow();

		void SetCurrentActor(const Fusion::Shared<Fusion::Actor>& InActor) { m_CurrentActor = InActor; }

		virtual void RenderContents() override;

	private:
		Fusion::Shared<Fusion::Actor> m_CurrentActor = nullptr;
	};

}
