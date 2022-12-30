#pragma once

#include "Fusion/Renderer/Renderer.hpp"

#include "D3D12Context.hpp"
#include "D3D12SwapChain.hpp"
#include "D3D12CommandAllocator.hpp"

namespace Fusion {

	class D3D12Renderer : public Renderer
	{
	public:
		D3D12Renderer(const RendererInfo& InInfo);

		void BeginFrame() override;
		void EndFrame() override;

		void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override;

		Shared<CommandAllocator> GetCurrentCommandAllocator() const override { return m_CommandAllocators[m_CurrentFrame]; }
		CommandList* GetCurrentCommandList() const override { return m_CommandAllocators[m_CurrentFrame]->GetCommandList(0); }

		uint32_t GetFramesInFlight() const override { return m_FramesInFlight; }
		uint32_t GetCurrentFrame() const override { return m_CurrentFrame; }

		void Release() override;

	private:
		Shared<D3D12Context> m_Context = nullptr;
		Shared<D3D12SwapChain> m_SwapChain = nullptr;

		uint32_t m_FramesInFlight = 3;
		uint32_t m_CurrentFrame = 0;

		std::vector<D3DComPtr<ID3D12Fence1>> m_FrameFences;
		HANDLE m_FrameEvent;
		std::vector<uint64_t> m_FrameValues;

		std::vector<Shared<D3D12CommandAllocator>> m_CommandAllocators;
	};

}
