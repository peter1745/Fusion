#pragma once

#include "BaseComponent.h"

#include "Fusion/Renderer/Texture.h"

namespace Fusion {

	struct SpriteComponent : public BaseComponent
	{
		Shared<Texture2D> Texture = nullptr;
	};

}
