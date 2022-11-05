#include "FusionPCH.hpp"
#include "WorldRenderer.hpp"
#include "Renderer.hpp"
#include "Fusion/Core/Application.hpp"

#include "Fusion/World/Components/AllComponents.hpp"

#include <Fusion/Renderer/GraphicsPipeline.hpp>

namespace Fusion {

	Unique<PipelineLayout> Layout;
	Unique<GraphicsPipeline> Pipeline;
	Shared<VertexBuffer> VertexBuf;

	struct Vertex1
	{
		glm::vec4 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoordinate;
	};

	static Vertex1 s_Vertices[] = {
		{ { 0.0f, 0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { 0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } }
	};

	WorldRenderer::WorldRenderer(DescriptorHeap* InDescriptorHeap, const Shared<World>& InWorld)
		: m_World(InWorld)
	{
		PipelineLayoutInfo LayoutInfo = {};
		LayoutInfo.Flags |= PipelineLayoutFlags::AllowInputAssemblerInputLayout;

		PipelineLayoutDescriptorRange ConstantBufferRange = {};
		ConstantBufferRange.Type = EPipelineLayoutDescriptorRangeType::ConstantBufferView;
		ConstantBufferRange.NumDescriptors = 1;
		ConstantBufferRange.Binding = 0;
		ConstantBufferRange.Space = 0;
		ConstantBufferRange.Offset = 0;
		ConstantBufferRange.Flags = DescriptorRangeFlags::StaticData;

		PipelineLayoutParameter ConstantBufferParam = {};
		ConstantBufferParam.Type = EPipelineParameterType::ConstantBufferView;
		ConstantBufferParam.Visibility = EShaderVisibility::All;
		ConstantBufferParam.Value = PipelineLayoutDescriptor{ 0, 0 };
		LayoutInfo.Parameters.push_back(ConstantBufferParam);

		Layout = PipelineLayout::Create(LayoutInfo);

		GraphicsPipelineInfo PipelineInfo = {};
		PipelineInfo.Layout = Layout.get();
		PipelineInfo.Inputs = {
			{ "POSITION", 0, EFormat::RGB32Float, 0, AppendAlignedElement, 0 },
			{ "NORMAL",   0, EFormat::RGB32Float, 0, AppendAlignedElement, 0 },
			{ "TEXCOORD", 0, EFormat::RG32Float,  0, AppendAlignedElement, 0 },
		};

		PipelineInfo.PipelineShader = Shader::Create({ "Resources/Shaders/FusionPBR.hlsl" });
		PipelineInfo.PrimitiveTopology = EPrimitiveTopology::Triangles;
		PipelineInfo.WindingOrder = EWindingOrder::CounterClockwise;
		PipelineInfo.RenderTargetCount = 2;
		PipelineInfo.RenderTargetFormats[0] = EFormat::RGBA8Unorm;
		PipelineInfo.RenderTargetFormats[1] = EFormat::RG32UInt;
		PipelineInfo.DepthStencilFormat = EFormat::D24UnormS8UInt;
		Pipeline = GraphicsPipeline::Create(PipelineInfo);

		VertexBufferInfo VertexBufferCreateInfo = {};
		VertexBufferCreateInfo.BufferSize = 3 * sizeof(Vertex1);
		VertexBufferCreateInfo.Data = s_Vertices;
		VertexBufferCreateInfo.Stride = sizeof(Vertex1);
		VertexBuf = VertexBuffer::Create(VertexBufferCreateInfo);

		ConstantBufferInfo TransformBufferInfo = {};
		TransformBufferInfo.Size = sizeof(TransformData);
		TransformBufferInfo.Usage = EBufferUsage::Dynamic;
		TransformBufferInfo.BindPoint = EShaderBindPoint::Both;
		m_TransformBuffer = UniformBuffer::Create(InDescriptorHeap, TransformBufferInfo);
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		m_TransformData.ViewProjection = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_TransformData.Transform = glm::mat4(1.0f);
	}

	void WorldRenderer::Render()
	{
		auto* CmdList = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentCommandList();
		Pipeline->Bind();

		const auto& MeshActors = m_World->FindAllActorsWith<TransformComponent, MeshComponent>();

		for (auto Actor : MeshActors)
		{
			const MeshComponent* MeshComp = Actor->FindComponent<MeshComponent>();

			if (!MeshComp->IsActive || !MeshComp->MeshHandle.IsValid())
				continue;

			const TransformComponent* TransformComp = Actor->FindComponent<TransformComponent>();
			const AssetContainer<MeshAsset> ActorMesh = Application::Get().GetAssetStorage()->GetAsset<MeshAsset>(MeshComp->MeshHandle);

			m_TransformData.Transform = glm::translate(glm::mat4(1.0f), TransformComp->Location)
				* glm::toMat4(TransformComp->GetRotation())
				* glm::scale(glm::mat4(1.0f), TransformComp->Scale);
			m_TransformData.ActorID = Actor->GetActorID();
			m_TransformBuffer->SetData(&m_TransformData);
			//MeshComp->Texture->Bind(0);

			CmdList->SetConstantBuffer(0, m_TransformBuffer);

			ActorMesh->GetMesh()->GetVertexBuffer()->Bind();

			CmdList->DrawIndexed(ActorMesh->GetMesh()->GetIndexBuffer());
		}
	}

	void WorldRenderer::End()
	{
		
	}

}
