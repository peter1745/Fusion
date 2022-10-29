#include "FusionPCH.hpp"
#include "WorldRenderer.hpp"
#include "Renderer.hpp"
#include "Fusion/Core/Application.hpp"

#include "Fusion/World/Components/AllComponents.hpp"

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
		m_TransformDataBuffer = UniformBuffer::Create(sizeof(glm::mat4) + sizeof(uint64_t), EShaderBindPoint::Both);
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		glm::mat4 ViewProjectionMatrix = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_CameraDataBuffer->SetData(&ViewProjectionMatrix);

		m_PBRShader->Bind();
		m_PBRShader->Set(1, m_CameraDataBuffer);
	}

	void WorldRenderer::Render()
	{
		const auto& MeshActors = m_World->FindAllActorsWith<TransformComponent, MeshComponent>();

		for (auto Actor : MeshActors)
		{
			const MeshComponent* MeshComp = Actor->FindComponent<MeshComponent>();

			if (!MeshComp->IsActive || !MeshComp->MeshHandle.IsValid())
				continue;

			const TransformComponent* TransformComp = Actor->FindComponent<TransformComponent>();
			const AssetContainer<MeshAsset> ActorMesh = Application::Get().GetAssetStorage()->GetAsset<MeshAsset>(MeshComp->MeshHandle);

			uint32_t EnttID = m_World->GetEnttID(Actor->GetActorID());

			struct TransformData
			{
				glm::mat4 Transform;
				uint64_t EnttID;
			} TData;

			TData.Transform = glm::translate(glm::mat4(1.0f), TransformComp->Location)
				* glm::toMat4(TransformComp->GetRotation())
				* glm::scale(glm::mat4(1.0f), TransformComp->Scale);
			TData.EnttID = Actor->GetActorID();
			m_TransformDataBuffer->SetData(&TData);
			m_PBRShader->Set(0, m_TransformDataBuffer);
			//MeshComp->Texture->Bind(0);

			m_Renderer->DrawIndexed(ActorMesh->GetMesh()->GetVertexBuffer(), ActorMesh->GetMesh()->GetIndexBuffer(), m_PBRShader);
		}
	}

	void WorldRenderer::End()
	{
		
	}

}
