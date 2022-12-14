#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"

#include "D3D12Common.hpp"
#include "D3D12CommandAllocator.hpp"
#include "D3D12DescriptorHeap.hpp"

namespace Fusion {

	class CommandContextQueue
	{
	public:
		typedef void(*CommandFunc)(void*, CommandList*);

		CommandContextQueue();
		~CommandContextQueue();

		void* AllocateCommand(CommandFunc InFunc, uint32_t InSize);
		void Execute(CommandList* InCmdList);

	private:
		uint8_t* m_CommandBuffer;
		uint8_t* m_CommandBufferPtr;
		uint32_t m_CommandCount = 0;
	};

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

		virtual Shared<DescriptorHeap> GetDescriptorHeap(EDescriptorHeapType InType) const override
		{
			FUSION_CORE_VERIFY(m_DescriptorHeaps.find(InType) != m_DescriptorHeaps.end());
			return m_DescriptorHeaps.at(InType);
		}

		template<typename TFunc>
		void SubmitToCommandContext(TFunc&& InFunc)
		{
			auto Command = [](void* InMemory, CommandList* InCmdList)
			{
				auto FuncPtr = reinterpret_cast<TFunc*>(InMemory);
				(*FuncPtr)(InCmdList);
				FuncPtr->~TFunc();
			};

			auto Storage = m_CommandContextQueue.AllocateCommand(Command, sizeof(InFunc));
			new (Storage) TFunc(std::forward<TFunc>(InFunc));
		}

		auto& GetDXGIFactory() { return m_Factory; }
		auto& GetDevice() { return m_Device; }
		auto& GetCommandQueue() { return m_CommandQueue; }

		virtual uint32_t GetCurrentFrameIndex() const override { return m_FrameIndex; }
		uint32_t GetFramesInFlight() const { return m_FrameCount; }

	private:
		static void MessageCallback(D3D12_MESSAGE_CATEGORY InCategory, D3D12_MESSAGE_SEVERITY InSeverity, D3D12_MESSAGE_ID InID, LPCSTR InDescription, void* InContext);

	private:
		D3DComPtr<IDXGIFactory7> m_Factory;
		D3DComPtr<ID3D12Device9> m_Device;
		D3DComPtr<ID3D12InfoQueue1> m_InfoQueue;

		D3DComPtr<ID3D12CommandQueue> m_CommandQueue;

		std::vector<Shared<D3D12CommandAllocator>> m_CommandAllocators;

		std::unordered_map<EDescriptorHeapType, Shared<D3D12DescriptorHeap>> m_DescriptorHeaps{};

		uint32_t m_FrameIndex = 0;
		uint32_t m_FrameCount = 3;

		D3DComPtr<ID3D12Fence1> m_FrameFence;
		HANDLE m_FrameEvent;
		std::vector<uint64_t> m_FrameValues;

		DWORD m_MessageCallbackCookie;

		CommandContextQueue m_CommandContextQueue;
	};

}
