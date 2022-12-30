#pragma once

#include "Fusion/Renderer/Renderer.hpp"

#include "D3D11Context.hpp"
#include "D3D11SwapChain.hpp"
#include "D3D11CommandAllocator.hpp"

namespace Fusion {

	class D3D11Renderer : public Renderer
	{
	public:
		D3D11Renderer(const RendererInfo& InInfo);
		~D3D11Renderer() = default;

		void BeginFrame() override;
		void EndFrame() override;

		void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override;

		Shared<CommandAllocator> GetCurrentCommandAllocator() const override { return m_CommandAllocator; }
		CommandList* GetCurrentCommandList() const override { return m_CommandAllocator->GetCommandList(0); }

		uint32_t GetFramesInFlight() const override { return 1; }
		uint32_t GetCurrentFrame() const override { return 0; }

		void Release() override;

	private:
		Shared<D3D11Context> m_Context = nullptr;
		Shared<D3D11SwapChain> m_SwapChain = nullptr;

		Shared<D3D11CommandAllocator> m_CommandAllocator = nullptr;
	};

}
