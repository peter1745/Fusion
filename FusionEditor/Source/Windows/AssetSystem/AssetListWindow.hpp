#pragma once

#include "Windows/EditorWindow.hpp"

namespace FusionEditor {

	class AssetListWindow : public EditorWindow
	{
	public:
		AssetListWindow(const std::string& InTitle)
		    : EditorWindow(InTitle) {}

		virtual constexpr EMenuBarLocation GetMenuBarLocation() const override { return EMenuBarLocation::View; }

		protected:
			virtual void RenderContents() override;
	};

}
