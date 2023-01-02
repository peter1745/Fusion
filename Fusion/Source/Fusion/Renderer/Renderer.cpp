#include "FusionPCH.hpp"
#include "Renderer.hpp"
#include "RenderSettings.hpp"

#include "Platform/Vulkan/VulkanRenderer.hpp"

#ifdef FUSION_PLATFORM_WINDOWS
	#include "Platform/D3D11/D3D11Renderer.hpp"
	#include "Platform/D3D12/D3D12Renderer.hpp"
#endif

namespace Fusion {

	static Renderer* s_Instance = nullptr;

	Unique<Renderer> Renderer::Create(const RendererInfo& InInfo)
	{
		Renderer* Result = nullptr;

		switch (RenderSettings::Get().API)
		{
		case ERendererAPI::None:
			Result = nullptr;
			break;
#ifdef FUSION_PLATFORM_WINDOWS
		case ERendererAPI::D3D11:
			Result = new D3D11Renderer(InInfo);
			break;
		case ERendererAPI::D3D12:
			Result = new D3D12Renderer(InInfo);
			break;
#endif
		case ERendererAPI::Vulkan:
			Result = new VulkanRenderer(InInfo);
			break;
		}

		s_Instance = Result;
		return Unique<Renderer>(Result);
	}

	Renderer& Renderer::GetCurrent()
	{
		return *s_Instance;
	}

	CommandQueue::CommandQueue()
	{
		m_CommandBuffer = new uint8_t[10 * 1024 * 1024];
		m_CommandBufferPtr = m_CommandBuffer;
		memset(m_CommandBuffer, 0, 10 * 1024 * 1024);
	}

	CommandQueue::~CommandQueue()
	{
		delete[] m_CommandBuffer;
	}

	void* CommandQueue::AllocateCommand(CommandQueue::CommandFunc InFunc, uint32_t InSize)
	{
		*reinterpret_cast<CommandFunc*>(m_CommandBufferPtr) = InFunc;
		m_CommandBufferPtr += sizeof(CommandFunc);

		*reinterpret_cast<uint32_t*>(m_CommandBufferPtr) = InSize;
		m_CommandBufferPtr += sizeof(uint32_t);

		void* Memory = m_CommandBufferPtr;
		m_CommandBufferPtr += InSize;

		m_CommandCount++;
		return Memory;
	}

	void CommandQueue::Execute(CommandList* InCmdList)
	{
		uint8_t* Buffer = m_CommandBuffer;

		for (uint32_t Idx = 0; Idx < m_CommandCount; Idx++)
		{
			CommandFunc Func = *reinterpret_cast<CommandFunc*>(Buffer);
			Buffer += sizeof(CommandFunc);

			uint32_t Size = *reinterpret_cast<uint32_t*>(Buffer);
			Buffer += sizeof(uint32_t);

			Func(Buffer, InCmdList);
			Buffer += Size;
		}

		m_CommandBufferPtr = m_CommandBuffer;
		m_CommandCount = 0;
	}
}