#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"

#include "D3D12Common.hpp"
#include "D3D12CommandAllocator.hpp"

namespace Fusion {

	class D3D12Context : public GraphicsContext
	{
	public:
		D3D12Context();
		~D3D12Context();

		virtual Shared<CommandAllocator> GetCommandAllocator() const override { return m_CommandAllocators[m_FrameIndex]; }
		virtual CommandList* GetCurrentCommandList() const override { return m_CommandAllocators[m_FrameIndex]->GetCommandList(0); }
		virtual void ExecuteCommandLists(const std::vector<CommandList*>& InCommandLists) override;

		virtual void NextFrame() override;
		virtual void WaitForGPU() override;

		auto& GetDXGIFactory() { return m_Factory; }
		auto& GetDevice() { return m_Device; }
		auto& GetCommandQueue() { return m_CommandQueue; }

		virtual uint32_t GetCurrentFrameIndex() const override { return m_FrameIndex; }
		uint32_t GetFramesInFlight() const { return m_FrameCount; }

	private:
		static void MessageCallback(D3D12_MESSAGE_CATEGORY InCategory, D3D12_MESSAGE_SEVERITY InSeverity, D3D12_MESSAGE_ID InID, LPCSTR InDescription, void* InContext);

	private:
		D3D12ComPtr<IDXGIFactory7> m_Factory;
		D3D12ComPtr<ID3D12Device9> m_Device;
		D3D12ComPtr<ID3D12InfoQueue1> m_InfoQueue;

		D3D12ComPtr<ID3D12CommandQueue> m_CommandQueue;

		std::vector<Shared<D3D12CommandAllocator>> m_CommandAllocators;

		uint32_t m_FramesBeforeWait = 0;
		uint32_t m_FrameIndex = 0;
		uint32_t m_FrameCount = 3;

		D3D12ComPtr<ID3D12Fence1> m_FrameFence;
		HANDLE m_FrameEvent;
		std::vector<uint64_t> m_FrameValues;

		DWORD m_MessageCallbackCookie;
	};

}
