#pragma once

#include "WorldRenderer.hpp"

#include <Fusion/Renderer/Shader.hpp>
#include <Fusion/Renderer/Buffer.hpp>

#include <Fusion/Renderer/GraphicsPipeline.hpp>

namespace FusionEditor {

	class SimpleWorldRenderer : public WorldRenderer
	{
	public:
		SimpleWorldRenderer(const Fusion::Shared<Fusion::World>& InWorld);

		// TODO(Peter): The view matrix should eventually either be part of Camera or passed somewhere else.
		//				The reason I haven't put the view matrix in Camera is because in play mode it's just
		//				the inverse of the camera actor's transform
		void Begin(const Fusion::Camera& InCamera, const glm::mat4& InViewMatrix) override;
		void Render() override;
		void End() override;

	private:
		struct TransformData
		{
			glm::mat4 ViewProjection;
			glm::mat4 Transform;
			uint64_t ActorID;
		} m_TransformData;

		struct LightData
		{
			glm::vec4 Color;
		} m_LightData;

	private:
		Fusion::Shared<Fusion::World> m_World = nullptr;
		Fusion::Unique<Fusion::GraphicsPipeline> m_Pipeline = nullptr;
		std::vector<Fusion::Shared<Fusion::Buffer>> m_LightDataBuffers;
		float m_Intensity = 1.0f;
		float m_IntensityModifier = 0.025f;

	};

}