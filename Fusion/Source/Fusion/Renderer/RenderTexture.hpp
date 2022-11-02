#pragma once

#include "Fusion/Memory/Shared.hpp"
#include "CommonTypes.hpp"
#include "Fusion/Core/Enum.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Fusion {

	using EImageFlag = Flags<>;
	using EImageState = Flags<>;

	namespace ImageStates
	{
		static constexpr EImageState RenderTarget = (1 << 0);
		static constexpr EImageState DepthWrite = (1 << 1);
		static constexpr EImageState DepthRead = (1 << 2);
		static constexpr EImageState UnorderedAccess = (1 << 3);
		static constexpr EImageState NonPixelShaderResource = (1 << 4);
		static constexpr EImageState PixelShaderResource = (1 << 5);
		static constexpr EImageState IndirectArgument = (1 << 6);
		static constexpr EImageState CopyDst = (1 << 7);
		static constexpr EImageState CopySrc = (1 << 8);
		static constexpr EImageState ResolveDst = (1 << 9);
		static constexpr EImageState ResolveSrc = (1 << 10);
		static constexpr EImageState ShadingRateSrc = (1 << 11);
	};

	namespace ImageFlags
	{
		static constexpr EImageFlag AllowRenderTarget = (1 << 0);
		static constexpr EImageFlag AllowDepthStencil = (1 << 1);
	}

	struct AttachmentSize { uint32_t Width; uint32_t Height; };

	struct RenderTextureAttachment
	{
		EGraphicsFormat Format;
		glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		EImageState InitialState;
		EImageFlag Flags;
	};

	struct RenderTextureInfo
	{
		uint32_t Width;
		uint32_t Height;
		std::vector<RenderTextureAttachment> ColorAttachments;
		RenderTextureAttachment DepthAttachment;
	};

	class RenderTexture : public SharedObject
	{
	public:
		virtual ~RenderTexture() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Clear() = 0;

		virtual AttachmentSize GetImageSize(uint32_t InAttachmentIndex, uint32_t InFrameIndex) const = 0;
		virtual void Resize(uint32_t InAttachmentIndex, uint32_t InFrameIndex, const AttachmentSize& InSize) = 0;

		virtual uint64_t ReadPixel(uint32_t InAttachmentIdx, uint32_t InX, uint32_t InY) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void TransitionImages(EImageState InColorAttachmentState, EImageState InDepthStencilState) = 0;

		virtual void* GetColorTextureID(uint32_t InColorAttachmentIdx) const = 0;

		virtual const RenderTextureInfo& GetInfo() const = 0;

	public:
		static Shared<RenderTexture> Create(const RenderTextureInfo& InCreateInfo);
	};

}
