#include "SimpleWorldRenderer.hpp"

#include <Fusion/Core/Application.hpp>
#include <Fusion/World/Components/AllComponents.hpp>
#include <Fusion/Renderer/Renderer.hpp>
#include <Fusion/IO/TextureLoader.hpp>

#include <Fusion/Renderer/ShaderCompiler.hpp>

#include <glm/gtx/quaternion.hpp>

namespace FusionEditor {

	
	Fusion::Shared<Fusion::Texture2D> Texture;
	Fusion::Unique<Fusion::ShaderCompiler> Compiler;

	SimpleWorldRenderer::SimpleWorldRenderer(const Fusion::Shared<Fusion::World>& InWorld)
	    : m_World(InWorld)
	{
		Compiler = Fusion::MakeUnique<Fusion::ShaderCompiler>();

		Fusion::GraphicsPipelineInfo PipelineInfo = {};
		PipelineInfo.PipelineShader = Compiler->CompileFromFile("Resources/Shaders/FusionPBR.glsl");
		PipelineInfo.PrimitiveTopology = Fusion::EPrimitiveTopology::Triangles;
		PipelineInfo.WindingOrder = Fusion::EWindingOrder::CounterClockwise;
		PipelineInfo.RenderTargetCount = 2;
		PipelineInfo.RenderTargetFormats[0] = Fusion::EFormat::RGBA8Unorm;
		PipelineInfo.RenderTargetFormats[1] = Fusion::EFormat::RG32UInt;
		PipelineInfo.DepthStencilFormat = Fusion::EFormat::D24UnormS8UInt;
		m_Pipeline = MakeUnique<Fusion::GraphicsPipeline>(Fusion::GraphicsContext::Get(), PipelineInfo);

		uint32_t FramesInFlight = Fusion::Renderer::GetCurrent().GetFramesInFlight();
		m_LightDataBuffers.resize(FramesInFlight);

		Fusion::BufferInfo LightDataBufferInfo = {};
		LightDataBufferInfo.Usage = Fusion::EBufferUsage::UniformBuffer;
		LightDataBufferInfo.Size = sizeof(LightData);
		for (uint32_t Idx = 0; Idx < FramesInFlight; Idx++)
			m_LightDataBuffers[Idx] = Fusion::Shared<Fusion::Buffer>::Create(LightDataBufferInfo);

		Texture = Fusion::TextureLoader::LoadFromFile("Resources/Textures/Test.png");
	}

	void SimpleWorldRenderer::Begin(const Fusion::Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		m_TransformData.ViewProjection = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_TransformData.Transform = glm::mat4(1.0f);
	}

	void SimpleWorldRenderer::Render()
	{
		uint32_t CurrentFrame = Fusion::Renderer::GetCurrent().GetCurrentFrame();
		auto* CmdList = Fusion::Renderer::GetCurrent().GetCurrentCommandList();

		m_LightData.Color = { 0.25f, 0.3f, 0.8f, 1.0f };
		void* BufferData = m_LightDataBuffers[CurrentFrame]->Map();
		memcpy(BufferData, &m_LightData, sizeof(LightData));
		m_LightDataBuffers[CurrentFrame]->Unmap();

		CmdList->SetTexture(m_Pipeline.get(), "InTexture", Texture, CurrentFrame);
		CmdList->SetUniformBuffer(m_Pipeline.get(), "InLightData", m_LightDataBuffers[CurrentFrame], CurrentFrame);

		m_Pipeline->Bind(CmdList);

		const auto& MeshActors = m_World->FindAllActorsWith<Fusion::TransformComponent, Fusion::MeshComponent>();

		for (auto Actor : MeshActors)
		{
			const Fusion::MeshComponent* MeshComp = Actor->FindComponent<Fusion::MeshComponent>();

			if (!MeshComp->IsActive || !MeshComp->MeshHandle.IsValid())
				continue;

			const Fusion::TransformComponent* TransformComp = Actor->FindComponent<Fusion::TransformComponent>();
			const auto ActorMesh = Fusion::Application::Get().GetAssetStorage()->GetAsset<Fusion::MeshAsset>(MeshComp->MeshHandle);

			if (ActorMesh == nullptr)
				continue;

			m_TransformData.Transform = glm::translate(glm::mat4(1.0f), TransformComp->Location) * glm::toMat4(TransformComp->GetRotation()) * glm::scale(glm::mat4(1.0f), TransformComp->Scale);
			m_TransformData.ActorID = Actor->GetActorID();
			CmdList->SetPushConstants(m_Pipeline.get(), Fusion::EShaderStage::Vertex, sizeof(TransformData), &m_TransformData);

			Fusion::VertexBufferView View = {};
			View.VertexBuffer = ActorMesh->GetMesh()->GetVertexBuffer();
			View.VertexStride = sizeof(Fusion::Vertex);
			CmdList->SetVertexBuffer(View);

			Fusion::IndexBufferView IndexView = {};
			IndexView.IndexBuffer = ActorMesh->GetMesh()->GetIndexBuffer();
			IndexView.IndexFormat = Fusion::EFormat::R32UInt;
			CmdList->DrawIndexed(IndexView);
		}
	}

	void SimpleWorldRenderer::End()
	{
	}


}
