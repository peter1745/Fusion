#include "FusionPCH.h"
#include "WorldRenderer.h"
#include "Renderer.h"
#include "Fusion/Core/Application.h"

#include "Fusion/World/Components/AllComponents.h"

namespace Fusion {

	WorldRenderer::WorldRenderer(const Shared<World>& InWorld)
		: m_World(InWorld)
	{
		ShaderSpecification PBRSpec;
		PBRSpec.FilePath = "Resources/Shaders/FusionPBR.hlsl";
		PBRSpec.InputLayout = {
			{ "POSITION", ShaderDataType::Float3, 0 },
			{ "NORMAL", ShaderDataType::Float3, 0 },
			{ "TEXCOORD", ShaderDataType::Float2, 0 }
		};
		m_PBRShader = Shader::Create(PBRSpec);

		m_Renderer = Application::Get().GetRenderer();
		m_CameraDataBuffer = UniformBuffer::Create(sizeof(glm::mat4), EShaderBindPoint::VertexShader);
		m_TransformDataBuffer = UniformBuffer::Create(sizeof(glm::mat4), EShaderBindPoint::VertexShader);
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		glm::mat4 ViewProjectionMatrix = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_CameraDataBuffer->SetData(&ViewProjectionMatrix);

		m_PBRShader->Bind();
		m_PBRShader->Set(0, m_CameraDataBuffer);
	}

	void WorldRenderer::Render()
	{
		const auto& MeshActors = m_World->FindAllActorsWith<TransformComponent, MeshComponent>();

		for (auto Actor : MeshActors)
		{
			const MeshComponent* MeshComp = Actor->FindComponent<MeshComponent>();

			if (!MeshComp->IsActive || !MeshComp->Mesh)
				continue;

			const TransformComponent* TransformComp = Actor->FindComponent<TransformComponent>();
			const Shared<Mesh> ActorMesh = MeshComp->Mesh;

			glm::mat4 Transform = glm::translate(glm::mat4(1.0f), TransformComp->Location)
				* glm::toMat4(TransformComp->Rotation)
				* glm::scale(glm::mat4(1.0f), TransformComp->Scale);
			m_TransformDataBuffer->SetData(&Transform);
			m_PBRShader->Set(1, m_TransformDataBuffer);

			m_Renderer->DrawIndexed(ActorMesh->GetVertexBuffer(), ActorMesh->GetIndexBuffer(), m_PBRShader);
		}
	}

	void WorldRenderer::End()
	{
		
	}

}
