#pragma once

#include "EditorWindow.h"

#include "Fusion/World/World.h"

namespace FusionEditor {

	class WorldOutlinerWindow : public EditorWindow
	{
	public:
		WorldOutlinerWindow(const Fusion::Shared<Fusion::World>& InWorld);
		~WorldOutlinerWindow();

	protected:
		virtual void RenderContents() override;

	private:
		void RenderActorNode(const Fusion::Shared<Fusion::Actor>& InActor);

	private:
		Fusion::Shared<Fusion::World> m_World = nullptr;
	};

}
