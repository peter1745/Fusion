#include "FusionPCH.h"
#include "WorldRenderer.h"
#include "Renderer.h"

#include "Fusion/World/Components/AllComponents.h"

namespace Fusion {

	WorldRenderer::WorldRenderer(World* InWorld)
		: m_World(InWorld)
	{
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		/*m_Framebuffer->Bind();
		Renderer::Begin();

		Renderer::GetActiveCommandBuffer()->CmdBindShader(m_PBRShader);
		m_PBRShader->Set("InProjectionMatrix", InCamera.GetProjectionMatrix());
		m_PBRShader->Set("InViewMatrix", InViewMatrix);*/
	}

	void WorldRenderer::Render()
	{
		/*const auto& MeshActors = m_World->FindAllActorsWith<TransformComponent, MeshComponent>();

		for (auto Actor : MeshActors)
		{
			const MeshComponent* MeshComp = Actor->FindComponent<MeshComponent>();

			if (!MeshComp->IsActive || !MeshComp->Mesh)
				continue;

			const TransformComponent* TransformComp = Actor->FindComponent<TransformComponent>();
			const Shared<Mesh> ActorMesh = MeshComp->Mesh;

			Renderer::DrawIndexed(ActorMesh->GetVertexBuffer(), ActorMesh->GetIndexBuffer(), m_PBRShader);
		}*/
	}

	void WorldRenderer::End()
	{
		/*Renderer::End();
		m_Framebuffer->Unbind();*/
	}

}
