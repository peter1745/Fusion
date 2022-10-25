#pragma once

#include <Fusion/Renderer/Mesh.hpp>

namespace Fusion {

	struct GLTFData
	{
		std::string Name = "Mesh";
		std::vector<Vertex> Vertices;
		std::vector<Index> Indices;
	};

	class GLTFLoader
	{
	public:
		static bool LoadGLTFMesh(const std::filesystem::path& InFilePath, GLTFData& OutData);
	};

}
