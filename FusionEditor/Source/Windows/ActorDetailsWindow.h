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
		void RenderComponentMenu();

		template<typename TComponent>
		void RenderComponentMenuItem(std::string_view InDisplayName)
		{
			if (m_CurrentActor->FindComponent<TComponent>() != nullptr)
				return;

			if (ImGui::MenuItem(InDisplayName.data()))
				m_CurrentActor->AddComponent<TComponent>();
		}

		template<typename TComponent, typename TFunc>
		void RenderComponentMenuItem(std::string_view InDisplayName, TFunc InFunc)
		{
			if (m_CurrentActor->FindComponent<TComponent>() != nullptr)
				return;

			if (ImGui::MenuItem(InDisplayName.data()))
			{
				TComponent* Comp = m_CurrentActor->AddComponent<TComponent>();
				InFunc(Comp);
			}
		}

	private:
		Fusion::Shared<Fusion::Actor> m_CurrentActor = nullptr;
	};

}
