#pragma once

#include "EditorWindow.hpp"

#include "Fusion/World/World.hpp"
#include "Fusion/STL/FunctionList.hpp"

namespace FusionEditor {

	class WorldOutlinerWindow : public EditorWindow
	{
	public:
		using SelectionCallbackList = Fusion::FunctionList<void(Fusion::Shared<Fusion::Actor>)>;
	public:
		WorldOutlinerWindow(const Fusion::Shared<Fusion::World>& InWorld);
		~WorldOutlinerWindow();

		SelectionCallbackList& GetSelectionCallbackList() { return m_SelectionCallbacks; }

	protected:
		virtual void RenderContents() override;

	private:
		void RenderActorNode(const Fusion::Shared<Fusion::Actor>& InActor);

	private:
		Fusion::Shared<Fusion::World> m_World = nullptr;
		SelectionCallbackList m_SelectionCallbacks;
	};

}
