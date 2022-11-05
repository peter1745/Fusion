#include "FusionPCH.hpp"
#include "UniformBuffer.hpp"
#include "Renderer.hpp"

#include "Platform/D3D11/D3D11UniformBuffer.hpp"
#include "Platform/D3D12/D3D12UniformBuffer.hpp"

namespace Fusion {

	Shared<UniformBuffer> UniformBuffer::Create(DescriptorHeap* InDescriptorHeap, const ConstantBufferInfo& InCreateInfo)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11UniformBuffer>::Create(InCreateInfo.Size, InCreateInfo.BindPoint, InCreateInfo.Usage);
		case ERendererAPI::D3D12: return Shared<D3D12UniformBuffer>::Create(InDescriptorHeap, InCreateInfo);
		}

		FUSION_CORE_VERIFY(false, "Unsupported API!");
		return nullptr;
	}

	Shared<UniformBuffer> UniformBuffer::Create(void* InData, uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage /*= EBufferUsage::Dynamic*/)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::None: return nullptr;
		case ERendererAPI::D3D11: return Shared<D3D11UniformBuffer>::Create(InData, InSize, InBindPoint, InUsage);
		}

		FUSION_CORE_VERIFY(false, "Unsupported API!");
		return nullptr;
	}

}
