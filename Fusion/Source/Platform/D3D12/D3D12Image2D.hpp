#pragma once

#include "Fusion/Renderer/Image.hpp"

#include "D3D12Common.hpp"

namespace Fusion {

	class D3D12Image2D : public Image2D
	{
	public:
		D3D12Image2D(const Image2DInfo& InCreateInfo);

		virtual const ImageSize& GetSize() const override { return m_CreateInfo.Size; }

		virtual void Resize(const ImageSize& InSize) override
		{
			m_CreateInfo.Size = InSize;
			Invalidate();
		}

		virtual void Transition(CommandList* InCmdList, EImageState InState) override;

		virtual EImageState GetState() const override { return m_State; }

		virtual const Image2DInfo& GetInfo() const override { return m_CreateInfo; }

		auto& GetResource() { return m_Resource; }
		const auto& GetResource() const { return m_Resource; }

	private:
		void Invalidate();

	private:
		Image2DInfo m_CreateInfo;

		D3DComPtr<ID3D12Resource2> m_Resource;
		EImageState m_State;

		friend class D3D12RenderTexture;
	};

}
