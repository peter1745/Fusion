#pragma once

#include "EditorWindow.hpp"
#include "IO/Selection/SelectionManager.hpp"

#include "Fusion/World/World.hpp"

#include <FTL/FunctionList.hpp>

namespace FusionEditor {

	class WorldOutlinerWindow : public EditorWindow
	{
	public:
		WorldOutlinerWindow(const Fusion::Shared<Fusion::World>& InWorld, const ActorSelectionManager& InSelectionCtx);
		virtual ~WorldOutlinerWindow();

		virtual constexpr EMenuBarLocation GetMenuBarLocation() const override { return EMenuBarLocation::View; }

	protected:
		virtual void RenderContents() override;

	private:
		void RenderActorNode(const Fusion::Shared<Fusion::Actor>& InActor);

	private:
		Fusion::Shared<Fusion::World> m_World = nullptr;
		ActorSelectionManager m_SelectionManager;

		Fusion::SharedActor m_DeletedActor = nullptr;
	};

}
