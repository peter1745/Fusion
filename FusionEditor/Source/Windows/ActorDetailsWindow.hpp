#pragma once

#include "EditorWindow.hpp"
#include "IO/Selection/SelectionManager.hpp"

#include "Fusion/World/Actor.hpp"

namespace FusionEditor {

	class ActorDetailsWindow : public EditorWindow
	{
	public:
		ActorDetailsWindow(const ActorSelectionManager& InSelectionManager);
		~ActorDetailsWindow();

		virtual constexpr EMenuBarLocation GetMenuBarLocation() const override { return EMenuBarLocation::View; }

	protected:
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

		void OnActorSelected(Fusion::Shared<Fusion::Actor> InActor);
		void OnActorDeselected(Fusion::Shared<Fusion::Actor> InActor);

	private:
		ActorSelectionManager m_SelectionManager = nullptr;
		Fusion::Shared<Fusion::Actor> m_CurrentActor = nullptr;
	};

}
