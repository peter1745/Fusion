#include "ProjectPropertiesWindow.hpp"
#include "UI/UIWidgets.hpp"

namespace FusionEditor {

	ProjectPropertiesWindow::ProjectPropertiesWindow(const std::shared_ptr<Project>& InProject)
	    : EditorWindow("ProjectPropertiesWindow"), m_CurrentProject(InProject)
	{
		SetTitle("Project Properties");
	}

	void ProjectPropertiesWindow::RenderContents()
	{
		if (m_CurrentProject == nullptr)
			return;

		std::string DefaultWorldFilePath = m_CurrentProject->DefaultWorldPath.string();
		UI::InputText("Default World Path", DefaultWorldFilePath);
	}

}
