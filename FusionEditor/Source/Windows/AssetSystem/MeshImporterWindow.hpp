#pragma once

#include "AssetImporterWindow.hpp"

namespace FusionEditor {

	class MeshImporterWindow : public AssetImporterWindow
	{
	public:
		MeshImporterWindow();

	private:
		void Import();

	protected:
		virtual void RenderContents() override;

	};

}
