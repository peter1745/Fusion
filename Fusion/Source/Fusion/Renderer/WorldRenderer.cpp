#include "FusionPCH.h"
#include "WorldRenderer.h"
#include "Renderer.h"

#include "Fusion/World/Components/AllComponents.h"

namespace Fusion {

	WorldRenderer::WorldRenderer(World* InWorld, uint32_t InViewportWidth, uint32_t InViewportHeight)
		: m_World(InWorld)
	{
		{
			// TODO(Peter): The framebuffer should be owned by the viewport in the future
			FramebufferSpecification FramebufferSpec;
			FramebufferSpec.Width = InViewportWidth;
			FramebufferSpec.Height = InViewportHeight;

			FramebufferAttachment ColorAttachment = { EFramebufferAttachmentFormat::RGBA8 };
			FramebufferSpec.ColorAttachments.push_back(ColorAttachment);

			FramebufferSpec.DepthAttachment.Format = EFramebufferAttachmentFormat::Depth24Stencil8;

			m_Framebuffer = Framebuffer::Create(FramebufferSpec);
		}

		{
			ShaderSpecification ShaderSpec;
			ShaderSpec.FilePath = "Resources/Shaders/PBR.glsl";
			m_PBRShader = Shader::Create(ShaderSpec);
		}
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		m_Framebuffer->Bind();
		Renderer::Begin();

		Renderer::GetActiveCommandBuffer()->CmdBindShader(m_PBRShader);
		m_PBRShader->Set("InProjectionMatrix", InCamera.GetProjectionMatrix());
		m_PBRShader->Set("InViewMatrix", InViewMatrix);
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

			Renderer::DrawIndexed(ActorMesh->GetVertexBuffer(), ActorMesh->GetIndexBuffer(), m_PBRShader);
		}
	}

	void WorldRenderer::End()
	{
		Renderer::End();
		m_Framebuffer->Unbind();
	}

	void WorldRenderer::SetViewportSize(uint32_t InWidth, uint32_t InHeight)
	{
		m_Framebuffer->Resize(InWidth, InHeight);
	}

}
