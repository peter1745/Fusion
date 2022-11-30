#pragma once

#include "ShapeBase.hpp"

#include <vector>
#include <array>

namespace Fission {

	class ConvexShape : public ShapeBase
	{
	public:
		ConvexShape(EShapeType InType)
			: ShapeBase(InType) {}

		virtual ~ConvexShape() = default;

		virtual float GetFastestLinearSpeed(const glm::vec3& InAngularVelocity, const glm::vec3& InDirection) const override;
		virtual glm::vec3 GetFurthestPoint(const glm::vec3& InLocation, const glm::vec3& InDirection, const glm::quat& InOrientation, float InBias) const override;
		virtual AABB GetBoundingBox(const glm::vec3& InLocation, const glm::quat& InOrientation) const override;

		virtual void CalculateInertiaTensor() override {}

	protected:
		int32_t FindPointFurthestInDirection(const std::vector<glm::vec3>& InPoints, const glm::vec3& InDirection) const;
		float GetDistanceFromLine(const glm::vec3& InLineStart, const glm::vec3& InLineEnd, const glm::vec3& InPoint) const;
		glm::vec3 FindPointFurthestFromLine(const std::vector<glm::vec3>& InPoints, const glm::vec3& InPoint0, const glm::vec3& InPoint1) const;
		float GetDistanceFromTriangle(const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2, const glm::vec3& InPoint3) const;
		glm::vec3 FindPointFurthestFromTriangle(const std::vector<glm::vec3>& InPoints, const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2) const;

		struct Triangle
		{
			int32_t A;
			int32_t B;
			int32_t C;
		};
		void BuildTetrahedron(const std::vector<glm::vec3>& InVertices, std::vector<glm::vec3>& OutHullPoints, std::vector<Triangle>& OutHullTriangles) const;

		void RemoveInternalPoints(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles, std::vector<glm::vec3>& InVertices) const;

		struct Edge
		{
			int32_t A;
			int32_t B;

			bool operator==(const Edge& InOther) const { return (A == InOther.A && B == InOther.B) || (A == InOther.B && B == InOther.A); }
		};
		bool IsEdgeUnique(const std::vector<Triangle>& InTriangles, const std::vector<int32_t>& InFacingTriangles, int32_t InIgnoreTriangle, const Edge& InEdge) const;

		void AddPoint(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles, const glm::vec3& InPoint) const;
		void RemoveUnreferencedVertices(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles) const;
		void ExpandConvexHull(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles, const std::vector<glm::vec3>& InVertices) const;

		void BuildConvexHull(const std::vector<glm::vec3>& InVertices, std::vector<glm::vec3>& OutHullPoints, std::vector<Triangle>& OutHullTriangles) const;

		bool IsPointExternal(const std::vector<glm::vec3>& InHullPoints, const std::vector<Triangle>& InHullTriangles, const glm::vec3& InPoint) const;

		glm::vec3 CalculateCenterOfMass(const std::vector<glm::vec3>& InHullPoints, const std::vector<Triangle>& InHullTriangles) const;
		glm::mat3 CalculateInertiaTensor(const std::vector<glm::vec3>& InHullPoints, const std::vector<Triangle>& InHullTriangles, const glm::vec3& InCenterOfMass) const;

	public:
		glm::vec2 ProjectSignedVolume1D(const glm::vec3& InLineStart, const glm::vec3& InLineEnd) const;
		glm::vec3 ProjectSignedVolume2D(const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2) const;
		glm::vec4 ProjectSignedVolume3D(const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2, const glm::vec3& InPoint3) const;

	protected:
		std::vector<glm::vec3> m_Points;
		AABB m_BoundingBox;
	};
}
