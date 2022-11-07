#include "FusionPCH.hpp"
#include "WorldRenderer.hpp"
#include "Fusion/Core/Application.hpp"

#include "Fusion/World/Components/AllComponents.hpp"
#include "Fusion/IO/TextureLoader.hpp"

namespace Fusion {

	Shared<Texture2D> Texture;

	WorldRenderer::WorldRenderer(const Shared<World>& InWorld)
		: m_World(InWorld)
	{
		PipelineLayoutInfo LayoutInfo = {};
		LayoutInfo.Flags |= PipelineLayoutFlags::AllowInputAssemblerInputLayout;

		PipelineLayoutParameter Param = {};
		Param.Type = EPipelineParameterType::ConstantBufferView;
		Param.Visibility = EShaderVisibility::All;
		Param.Value = PipelineLayoutDescriptor{ 0, 0 };
		LayoutInfo.Parameters.push_back(Param);

		PipelineLayoutDescriptorTable Table = {};
		PipelineLayoutDescriptorRange Descriptor1 = {};
		Descriptor1.Type = EPipelineLayoutDescriptorRangeType::ShaderResourceView;
		Descriptor1.NumDescriptors = 1;
		Descriptor1.Binding = 0;
		Descriptor1.Space = 0;
		Descriptor1.Offset = 0;
		Descriptor1.Flags = DescriptorRangeFlags::StaticData;
		Table.Ranges.push_back(Descriptor1);

		PipelineLayoutParameter T = {};
		T.Type = EPipelineParameterType::DescriptorTable;
		T.Visibility = EShaderVisibility::Pixel;
		T.Value = Table;
		LayoutInfo.Parameters.push_back(T);

		/*PipelineLayoutParameter Param2 = {};
		Param2.Type = EPipelineParameterType::ShaderResourceView;
		Param2.Visibility = EShaderVisibility::Pixel;
		Param2.Value = PipelineLayoutDescriptor{ 1, 0 };
		LayoutInfo.Parameters.push_back(Param2);*/

		PipelineStaticSampler Sampler = {};
		Sampler.MinFilter = EFilterMode::Nearest;
		Sampler.MagFilter = EFilterMode::Nearest;
		Sampler.AddressModeU = EImageAddressMode::Clamp;
		Sampler.AddressModeV = EImageAddressMode::Clamp;
		Sampler.AddressModeW = EImageAddressMode::Clamp;
		Sampler.Binding = 0;
		Sampler.Register = 0;
		Sampler.Visibility = EShaderVisibility::Pixel;
		LayoutInfo.StaticSamplers = { Sampler };

		m_PipelineLayout = PipelineLayout::Create(LayoutInfo);

		GraphicsPipelineInfo PipelineInfo = {};
		PipelineInfo.Layout = m_PipelineLayout.get();
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
		m_Pipeline = GraphicsPipeline::Create(PipelineInfo);

		BufferInfo TransformDataBufferInfo = {};
		TransformDataBufferInfo.HeapType = EHeapType::Default;
		TransformDataBufferInfo.State = BufferStates::Constant;
		TransformDataBufferInfo.Size = sizeof(TransformData);
		TransformDataBufferInfo.Alignment = 16;
		m_TransformBuffer = Buffer::Create(TransformDataBufferInfo);

		BufferInfo TransformUploadBufferInfo = {};
		TransformUploadBufferInfo.HeapType = EHeapType::Upload;
		TransformUploadBufferInfo.State = BufferStates::Constant;
		TransformUploadBufferInfo.Size = sizeof(TransformData);
		TransformUploadBufferInfo.Alignment = 16;
		m_TransformUploadBuffer = Buffer::Create(TransformUploadBufferInfo);

		Texture = TextureLoader::LoadFromFile("Resources/Textures/Test.png");
	}

	void WorldRenderer::Begin(const Camera& InCamera, const glm::mat4& InViewMatrix)
	{
		m_TransformData.ViewProjection = InCamera.GetProjectionMatrix() * InViewMatrix;
		m_TransformData.Transform = glm::mat4(1.0f);
	}

	void WorldRenderer::Render()
	{
		auto* CmdList = Fusion::GraphicsContext::Get<Fusion::GraphicsContext>()->GetCurrentCommandList();
		m_Pipeline->Bind(CmdList);

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
			m_TransformBuffer->SetData(CmdList, &m_TransformData, m_TransformUploadBuffer);
			CmdList->SetConstantBuffer(m_Pipeline.get(), 0, m_TransformBuffer);

			VertexBufferView View = {};
			View.VertexBuffer = ActorMesh->GetMesh()->GetVertexBuffer();
			View.VertexStride = sizeof(Vertex);
			CmdList->SetVertexBuffer(View);

			Texture->Bind(1);

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
