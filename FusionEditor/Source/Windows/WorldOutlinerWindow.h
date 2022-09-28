#pragma once

#include "EditorWindow.h"

#include "Fusion/World/World.h"

namespace FusionEditor {

	class WorldOutlinerWindow : public EditorWindow
	{
	public:
		WorldOutlinerWindow(Fusion::World* InWorld);
		~WorldOutlinerWindow();

		virtual void RenderContents() override;

	private:
		void RenderActorNode(const Fusion::Shared<Fusion::Actor>& InActor);

	private:
		Fusion::World* m_World = nullptr;
	};

}
