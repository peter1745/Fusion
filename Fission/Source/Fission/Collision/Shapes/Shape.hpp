#pragma once

namespace Fission {

	enum class EShapeType { Sphere, Box };

	class Shape
	{
	public:
		virtual constexpr EShapeType GetType() const = 0;
	};

}
