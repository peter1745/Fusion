#include "FusionPCH.hpp"
#include "UniformBuffer.hpp"
#include "Renderer.hpp"

#include "Platform/D3D11/D3D11UniformBuffer.hpp"

namespace Fusion {

	Shared<UniformBuffer> UniformBuffer::Create(uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage /*= EBufferUsage::Dynamic*/)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11UniformBuffer>::Create(InSize, InBindPoint, InUsage);
		}

		FUSION_CORE_VERIFY(false, "Unsupported API!");
		return nullptr;
	}

	Shared<UniformBuffer> UniformBuffer::Create(void* InData, uint32_t InSize, EShaderBindPoint InBindPoint, EBufferUsage InUsage /*= EBufferUsage::Dynamic*/)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11UniformBuffer>::Create(InData, InSize, InBindPoint, InUsage);
		}

		FUSION_CORE_VERIFY(false, "Unsupported API!");
		return nullptr;
	}

}
