#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"
#include "D3D11SwapChain.hpp"

#define FUSION_RELEASE_COM(InResource) if (InResource != nullptr) { InResource->Release(); InResource = nullptr; }

namespace Fusion {

	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context();
		~D3D11Context();

		virtual Shared<CommandAllocator> GetCommandAllocator() const override { return nullptr; }
		virtual CommandList* GetCurrentCommandList() const override { return nullptr; }
		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override {}

		virtual void NextFrame() override {}
		virtual void WaitForGPU() override {}

		ID3D11Device* GetDevice() const { return m_Device; }
		ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext; }

	private:
		ID3D11Device* m_Device = nullptr;
		ID3D11DeviceContext* m_DeviceContext = nullptr;
	};

}
