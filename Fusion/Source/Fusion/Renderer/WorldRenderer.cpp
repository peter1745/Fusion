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

	WorldRenderer::WorldRenderer(const Shared<World>& InWorld)
		: m_World(InWorld)
	{
		/*ShaderSpecification PBRSpec;
		PBRSpec.FilePath = "Resources/Shaders/FusionPBR.hlsl";
		PBRSpec.InputLayout = {
			{ "POSITION", ShaderDataType::Float3, 0 },
			{ "NORMAL", ShaderDataType::Float3, 0 },
			{ "TEXCOORD", ShaderDataType::Float2, 0 }
		};
		m_PBRShader = Shader::Create(PBRSpec);

		m_Renderer = Application::Get().GetRenderer();
		m_CameraDataBuffer = UniformBuffer::Create(sizeof(glm::mat4), EShaderBindPoint::VertexShader);
		m_TransformDataBuffer = UniformBuffer::Create(sizeof(glm::mat4) + sizeof(uint64_t), EShaderBindPoint::Both);*/

		PipelineLayoutInfo LayoutInfo = {};
		LayoutInfo.Flags |= PipelineLayoutFlags::AllowInputAssemblerInputLayout;
		Layout = PipelineLayout::Create(LayoutInfo);

		GraphicsPipelineInfo PipelineInfo = {};
		PipelineInfo.Layout = Layout.get();
		PipelineInfo.Inputs = {
			{ "POSITION", 0, EFormat::RGBA32Float, 0, AppendAlignedElement, 0 },
			{ "NORMAL",   0, EFormat::RGB32Float, 0, AppendAlignedElement, 0 },
			{ "TEXCOORD", 0, EFormat::RG32Float,  0, AppendAlignedElement, 0 },
		};

		PipelineInfo.PipelineShader = Shader::Create({ "Resources/Shaders/D3D12.hlsl" });
		PipelineInfo.PrimitiveTopology = EPrimitiveTopology::Triangles;
		PipelineInfo.WindingOrder = EWindingOrder::CounterClockwise;
		PipelineInfo.RenderTargetCount = 1;
		PipelineInfo.RenderTargetFormats[0] = EFormat::RGBA8Unorm;
		PipelineInfo.DepthStencilFormat = EFormat::D24UnormS8UInt;
		Pipeline = GraphicsPipeline::Create(PipelineInfo);

		VertexBufferInfo VertexBufferCreateInfo = {};
		VertexBufferCreateInfo.BufferSize = 3 * sizeof(Vertex1);
		VertexBufferCreateInfo.Data = s_Vertices;
		VertexBufferCreateInfo.Stride = sizeof(Vertex1);
		VertexBuf = VertexBuffer::Create(VertexBufferCreateInfo);
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		/*glm::mat4 ViewProjectionMatrix = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_CameraDataBuffer->SetData(&ViewProjectionMatrix);

		m_PBRShader->Bind();
		m_PBRShader->Set(1, m_CameraDataBuffer);*/
	}

	void WorldRenderer::Render()
	{
		auto* CmdList = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentCommandList();
		Pipeline->Bind();
		VertexBuf->Bind();
		CmdList->DrawInstanced(3, 1, 0, 0);

		//const auto& MeshActors = m_World->FindAllActorsWith<TransformComponent, MeshComponent>();

		//for (auto Actor : MeshActors)
		//{
		//	const MeshComponent* MeshComp = Actor->FindComponent<MeshComponent>();

		//	if (!MeshComp->IsActive || !MeshComp->MeshHandle.IsValid())
		//		continue;

		//	const TransformComponent* TransformComp = Actor->FindComponent<TransformComponent>();
		//	const AssetContainer<MeshAsset> ActorMesh = Application::Get().GetAssetStorage()->GetAsset<MeshAsset>(MeshComp->MeshHandle);

		//	uint32_t EnttID = m_World->GetEnttID(Actor->GetActorID());

		//	struct TransformData
		//	{
		//		glm::mat4 Transform;
		//		uint64_t EnttID;
		//	} TData;

		//	TData.Transform = glm::translate(glm::mat4(1.0f), TransformComp->Location)
		//		* glm::toMat4(TransformComp->GetRotation())
		//		* glm::scale(glm::mat4(1.0f), TransformComp->Scale);
		//	TData.EnttID = Actor->GetActorID();
		//	m_TransformDataBuffer->SetData(&TData);
		//	m_PBRShader->Set(0, m_TransformDataBuffer);
		//	//MeshComp->Texture->Bind(0);

		//	m_Renderer->DrawIndexed(ActorMesh->GetMesh()->GetVertexBuffer(), ActorMesh->GetMesh()->GetIndexBuffer(), m_PBRShader);
		//}
	}

	void WorldRenderer::End()
	{
		
	}

}
