#pragma once

#include "EditorWindow.hpp"

#include "Project/Project.hpp"

namespace FusionEditor {

	class ProjectPropertiesWindow : public EditorWindow
	{
	public:
		ProjectPropertiesWindow(const std::shared_ptr<Project>& InProject);

		void OnProjectChanged(std::shared_ptr<Project> InProject) override
		{
			m_CurrentProject = InProject;
		}

		constexpr EMenuBarLocation GetMenuBarLocation() const override { return EMenuBarLocation::Edit; };
		bool IsDisabled() const override { return m_CurrentProject == nullptr; };

	protected:
		virtual void RenderContents() override;

	private:
		std::shared_ptr<Project> m_CurrentProject;
	};

}
