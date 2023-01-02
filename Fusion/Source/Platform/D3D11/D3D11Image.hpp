#pragma once

#include "Fusion/Renderer/Image.hpp"

#include "D3D11Common.hpp"

namespace Fusion {

	class D3D11Image2D : public Image2D
	{
	public:
		D3D11Image2D(const Image2DInfo& InCreateInfo);
		~D3D11Image2D();

		virtual const ImageSize& GetSize() const override { return m_CreateInfo.Size; }

		virtual void Resize(const ImageSize& InSize) override
		{
			m_CreateInfo.Size = InSize;
			Invalidate();
		}

		virtual void Transition(CommandList* InCmdList, EImageState InState) override {}

		virtual EImageState GetState() const override { return m_CreateInfo.InitialState; }

		virtual const Image2DInfo& GetInfo() const override { return m_CreateInfo; }

		void Release() override;

		auto& GetResource() { return m_Image; }
		const auto& GetResource() const { return m_Image; }

	private:
		void Invalidate();

	private:
		Image2DInfo m_CreateInfo;
		D3DComPtr<ID3D11Texture2D> m_Image;
	};

}
