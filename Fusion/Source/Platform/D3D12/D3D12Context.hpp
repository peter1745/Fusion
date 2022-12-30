#pragma once

#include "Fusion/Renderer/GraphicsContext.hpp"

#include "D3D12Device.hpp"
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

		Shared<Device> GetDevice() const override { return m_Device; }

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

		void Release() override;

		auto& GetDXGIFactory() { return m_Factory; }

	private:
		static void MessageCallback(D3D12_MESSAGE_CATEGORY InCategory, D3D12_MESSAGE_SEVERITY InSeverity, D3D12_MESSAGE_ID InID, LPCSTR InDescription, void* InContext);

	private:
		D3DComPtr<IDXGIFactory7> m_Factory;
		Shared<D3D12Device> m_Device;

		D3DComPtr<ID3D12InfoQueue1> m_InfoQueue;


		std::vector<Shared<D3D12CommandAllocator>> m_CommandAllocators;

		std::unordered_map<EDescriptorHeapType, Shared<D3D12DescriptorHeap>> m_DescriptorHeaps{};

		DWORD m_MessageCallbackCookie;

		CommandContextQueue m_CommandContextQueue;
	};

}
