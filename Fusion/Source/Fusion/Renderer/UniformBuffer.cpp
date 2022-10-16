#include "FusionPCH.h"
#include "UniformBuffer.h"
#include "Renderer.h"

#include "Platform/D3D11/D3D11UniformBuffer.h"

namespace Fusion {

	Shared<UniformBuffer> UniformBuffer::Create(uint32_t InSize, EBufferUsage InUsage /*= EBufferUsage::Dynamic*/)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11UniformBuffer>::Create(InSize, InUsage);
		}

		FUSION_CORE_VERIFY(false, "Unsupported API!");
		return nullptr;
	}

	Shared<UniformBuffer> UniformBuffer::Create(void* InData, uint32_t InSize, EBufferUsage InUsage /*= EBufferUsage::Dynamic*/)
	{
		switch (Renderer::CurrentAPI())
		{
		case ERendererAPI::D3D11: return Shared<D3D11UniformBuffer>::Create(InData, InSize, InUsage);
		}

		FUSION_CORE_VERIFY(false, "Unsupported API!");
		return nullptr;
	}

}
