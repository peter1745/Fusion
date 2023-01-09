#include "FusionPCH.hpp"
#include "WorldRenderer.hpp"
#include "Renderer.hpp"

#include "Fusion/Core/Application.hpp"
#include "Fusion/World/Components/AllComponents.hpp"
#include "Fusion/IO/TextureLoader.hpp"

#include "ShaderCompiler.hpp"

namespace Fusion {

	Shared<Texture2D> Texture;
	Unique<ShaderCompiler> Compiler;

	WorldRenderer::WorldRenderer(const Shared<World>& InWorld)
	    : m_World(InWorld)
	{
		Compiler = ShaderCompiler::Create();

		GraphicsPipelineInfo PipelineInfo = {};
		PipelineInfo.PipelineShader = Compiler->CreateShader("Resources/Shaders/FusionPBR.glsl");
		PipelineInfo.PrimitiveTopology = EPrimitiveTopology::Triangles;
		PipelineInfo.WindingOrder = EWindingOrder::CounterClockwise;
		PipelineInfo.RenderTargetCount = 2;
		PipelineInfo.RenderTargetFormats[0] = EFormat::RGBA8Unorm;
		PipelineInfo.RenderTargetFormats[1] = EFormat::RG32UInt;
		PipelineInfo.DepthStencilFormat = EFormat::D24UnormS8UInt;
		m_Pipeline = GraphicsPipeline::Create(PipelineInfo);

		//BufferInfo TransformDataBufferInfo = {};
		//TransformDataBufferInfo.HeapType = EHeapType::Default;
		//TransformDataBufferInfo.State = BufferStates::Constant;
		//TransformDataBufferInfo.Size = sizeof(TransformData);
		//TransformDataBufferInfo.Alignment = 16;
		//m_TransformBuffer = Buffer::Create(TransformDataBufferInfo);

		//BufferInfo TransformUploadBufferInfo = {};
		//TransformUploadBufferInfo.HeapType = EHeapType::Upload;
		//TransformUploadBufferInfo.State = BufferStates::Constant;
		//TransformUploadBufferInfo.Size = sizeof(TransformData);
		//TransformUploadBufferInfo.Alignment = 16;
		//m_TransformUploadBuffer = Buffer::Create(TransformUploadBufferInfo);

		//Texture = TextureLoader::LoadFromFile("Resources/Textures/Test.png");
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		m_TransformData.ViewProjection = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_TransformData.Transform = glm::mat4(1.0f);
	}

	void WorldRenderer::Render()
	{
		auto* CmdList = Renderer::GetCurrent().GetCurrentCommandList();
		m_Pipeline->Bind(CmdList);

		const auto& MeshActors = m_World->FindAllActorsWith<TransformComponent, MeshComponent>();

		for (auto Actor : MeshActors)
		{
			const MeshComponent* MeshComp = Actor->FindComponent<MeshComponent>();

			if (!MeshComp->IsActive || !MeshComp->MeshHandle.IsValid())
				continue;

			const TransformComponent* TransformComp = Actor->FindComponent<TransformComponent>();
			const AssetContainer<MeshAsset> ActorMesh = Application::Get().GetAssetStorage()->GetAsset<MeshAsset>(MeshComp->MeshHandle);

			m_TransformData.Transform = glm::translate(glm::mat4(1.0f), TransformComp->Location) * glm::toMat4(TransformComp->GetRotation()) * glm::scale(glm::mat4(1.0f), TransformComp->Scale);
			m_TransformData.ActorID = Actor->GetActorID();
			CmdList->SetPushConstants(m_Pipeline.get(), EShaderType::Vertex, sizeof(TransformData), &m_TransformData);

			VertexBufferView View = {};
			View.VertexBuffer = ActorMesh->GetMesh()->GetVertexBuffer();
			View.VertexStride = sizeof(Vertex);
			CmdList->SetVertexBuffer(View);

			//CmdList->SetTexture(m_Pipeline.get(), "InTexture", Texture);

			IndexBufferView IndexView = {};
			IndexView.IndexBuffer = ActorMesh->GetMesh()->GetIndexBuffer();
			IndexView.IndexFormat = EFormat::R32UInt;
			CmdList->DrawIndexed(IndexView);
		}
	}

	void WorldRenderer::End()
	{
	}

}
