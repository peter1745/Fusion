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
		Compiler = MakeUnique<ShaderCompiler>();

		GraphicsPipelineInfo PipelineInfo = {};
		PipelineInfo.PipelineShader = Compiler->CreateShader(GraphicsContext::Get()->GetDevice(), "Resources/Shaders/FusionPBR.glsl");
		PipelineInfo.PrimitiveTopology = EPrimitiveTopology::Triangles;
		PipelineInfo.WindingOrder = EWindingOrder::CounterClockwise;
		PipelineInfo.RenderTargetCount = 2;
		PipelineInfo.RenderTargetFormats[0] = EFormat::RGBA8Unorm;
		PipelineInfo.RenderTargetFormats[1] = EFormat::RG32UInt;
		PipelineInfo.DepthStencilFormat = EFormat::D24UnormS8UInt;
		m_Pipeline = MakeUnique<GraphicsPipeline>(GraphicsContext::Get(), PipelineInfo);

		uint32_t FramesInFlight = Renderer::GetCurrent().GetFramesInFlight();
		m_LightDataBuffers.resize(FramesInFlight);

		BufferInfo LightDataBufferInfo = {};
		LightDataBufferInfo.Usage = EBufferUsage::UniformBuffer;
		LightDataBufferInfo.Size = sizeof(LightData);
		for (uint32_t Idx = 0; Idx < FramesInFlight; Idx++)
			m_LightDataBuffers[Idx] = Shared<Buffer>::Create(LightDataBufferInfo);

		Texture = TextureLoader::LoadFromFile("Resources/Textures/Test.png");
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		m_TransformData.ViewProjection = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_TransformData.Transform = glm::mat4(1.0f);
	}

	void WorldRenderer::Render()
	{
		uint32_t CurrentFrame = Renderer::GetCurrent().GetCurrentFrame();
		auto* CmdList = Renderer::GetCurrent().GetCurrentCommandList();

		m_LightData.Color = { 0.25f, 0.3f, 0.8f, 1.0f };
		void* BufferData = m_LightDataBuffers[CurrentFrame]->Map();
		memcpy(BufferData, &m_LightData, sizeof(LightData));
		m_LightDataBuffers[CurrentFrame]->Unmap();

		CmdList->SetTexture(m_Pipeline.get(), "InTexture", Texture, CurrentFrame);
		CmdList->SetUniformBuffer(m_Pipeline.get(), "InLightData", m_LightDataBuffers[CurrentFrame], CurrentFrame);

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
			CmdList->SetPushConstants(m_Pipeline.get(), EShaderStage::Vertex, sizeof(TransformData), &m_TransformData);

			VertexBufferView View = {};
			View.VertexBuffer = ActorMesh->GetMesh()->GetVertexBuffer();
			View.VertexStride = sizeof(Vertex);
			CmdList->SetVertexBuffer(View);

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
