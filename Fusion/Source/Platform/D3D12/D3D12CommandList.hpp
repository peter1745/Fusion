#pragma once

#include "D3D12Common.hpp"

#include "Fusion/Renderer/CommandList.hpp"

namespace Fusion {

	class D3D12CommandAllocator;
	class D3D12CommandList : public CommandList
	{
	public:
		D3D12CommandList(const Shared<D3D12CommandAllocator>& InAllocator, D3D12ComPtr<ID3D12GraphicsCommandList6> InCommandList);

		virtual void Reset() override;

		virtual void BeginRecording() override;
		virtual void SetViewports(const std::vector<Viewport>& InViewports) override;

		virtual void EndRecording() override;

		auto& GetNativeList() { return m_CommandList; }

	private:
		Shared<D3D12CommandAllocator> m_Allocator = nullptr;
		D3D12ComPtr<ID3D12GraphicsCommandList6> m_CommandList;

	private:
		friend class D3D12CommandAllocator;
	};

}
