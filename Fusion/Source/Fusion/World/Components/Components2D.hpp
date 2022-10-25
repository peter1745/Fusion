#pragma once

#include "BaseComponent.hpp"

#include "Fusion/Renderer/Texture.hpp"

namespace Fusion {

	struct SpriteComponent : public BaseComponent
	{
		Shared<Texture2D> Texture = nullptr;
	};

}
