#include "ConvexShape.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_operation.hpp>

namespace Fission {

	float ConvexShape::GetFastestLinearSpeed(const glm::vec3& InAngularVelocity, const glm::vec3& InDirection) const
	{
		float MaxSpeed = 0.0f;
		for (size_t Idx = 1; Idx < m_Points.size(); Idx++)
		{
			glm::vec3 LinearVelocity = glm::cross(InAngularVelocity, m_Points[Idx] - m_CenterOfMass);
			MaxSpeed = glm::max(MaxSpeed, glm::dot(InDirection, LinearVelocity));
		}

		return MaxSpeed;
	}

	glm::vec3 ConvexShape::GetFurthestPoint(const glm::vec3& InLocation, const glm::vec3& InDirection, const glm::quat& InOrientation, float InBias) const
	{
		glm::vec3 MaxPoint = (InOrientation * m_Points[0]) + InLocation;
		float MaxDistance = glm::dot(InDirection, MaxPoint);
		for (size_t Idx = 1; Idx < m_Points.size(); Idx++)
		{
			glm::vec3 Point = (InOrientation * m_Points[Idx]) + InLocation;
			float Distance = glm::dot(InDirection, Point);

			if (Distance > MaxDistance)
			{
				MaxDistance = Distance;
				MaxPoint = Point;
			}
		}

		return MaxPoint + (InDirection * InBias);
	}

	AABB ConvexShape::GetBoundingBox(const glm::vec3& InLocation, const glm::quat& InOrientation) const
	{
		const auto& MinBound = m_BoundingBox.GetMinBound();
		const auto& MaxBound = m_BoundingBox.GetMaxBound();

		std::array<glm::vec3, 8> Corners;
		Corners[0] = MinBound;
		Corners[1] = { MinBound.x, MinBound.y, MaxBound.z };
		Corners[2] = { MinBound.x, MaxBound.y, MinBound.z };
		Corners[3] = { MaxBound.x, MinBound.y, MinBound.z };

		Corners[4] = MaxBound;
		Corners[5] = { MaxBound.x, MaxBound.y, MinBound.z };
		Corners[6] = { MaxBound.x, MinBound.y, MaxBound.z };
		Corners[7] = { MinBound.x, MaxBound.y, MaxBound.z };

		AABB BoundingBox;
		for (size_t Idx = 0; Idx < m_Points.size(); Idx++)
			BoundingBox.Expand((InOrientation * Corners[Idx]) + InLocation);
		return BoundingBox;
	}

	int32_t ConvexShape::FindPointFurthestInDirection(const std::vector<glm::vec3>& InPoints, const glm::vec3& InDirection) const
	{
		int32_t MaxIndex = 0;
		float MaxDistance = glm::dot(InDirection, InPoints[0]);

		for (int32_t Idx = 1; Idx < InPoints.size(); Idx++)
		{
			float Distance = glm::dot(InDirection, InPoints[Idx]);

			if (Distance > MaxDistance)
			{
				MaxDistance = Distance;
				MaxIndex = Idx;
			}
		}

		return MaxIndex;
	}

	float ConvexShape::GetDistanceFromLine(const glm::vec3& InLineStart, const glm::vec3& InLineEnd, const glm::vec3& InPoint) const
	{
		glm::vec3 Ray = InPoint - InLineStart;
		glm::vec3 LineLength = glm::normalize(InLineEnd - InLineStart);
		glm::vec3 Perpendicular = Ray - (LineLength * glm::dot(Ray, LineLength));
		return glm::dot(Perpendicular, Perpendicular);
	}

	glm::vec3 ConvexShape::FindPointFurthestFromLine(const std::vector<glm::vec3>& InPoints, const glm::vec3& InPoint0, const glm::vec3& InPoint1) const
	{
		int32_t MaxIndex = 0;
		float MaxDistance = GetDistanceFromLine(InPoint0, InPoint1, InPoints[0]);
		for (int32_t Idx = 1; Idx < InPoints.size(); Idx++)
		{
			float Distance = GetDistanceFromLine(InPoint0, InPoint1, InPoints[Idx]);
			if (Distance > MaxDistance)
			{
				MaxDistance = Distance;
				MaxIndex = Idx;
			}
		}

		return InPoints[MaxIndex];
	}

	float ConvexShape::GetDistanceFromTriangle(const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2, const glm::vec3& InPoint3) const
	{
		glm::vec3 Normal = glm::normalize(glm::cross(InPoint1 - InPoint0, InPoint2 - InPoint0));
		return glm::dot(InPoint3 - InPoint0, Normal);
	}

	glm::vec3 ConvexShape::FindPointFurthestFromTriangle(const std::vector<glm::vec3>& InPoints, const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2) const
	{
		int32_t MaxIndex = 0;
		float MaxDistance = GetDistanceFromTriangle(InPoint0, InPoint1, InPoint2, InPoints[0]);

		for (int32_t Idx = 0; Idx < InPoints.size(); Idx++)
		{
			float Distance = GetDistanceFromTriangle(InPoint0, InPoint1, InPoint2, InPoints[Idx]);

			if (Distance > MaxDistance)
			{
				MaxDistance = Distance;
				MaxIndex = Idx;
			}
		}

		return InPoints[MaxIndex];
	}

	void ConvexShape::BuildTetrahedron(const std::vector<glm::vec3>& InVertices, std::vector<glm::vec3>& OutHullPoints, std::vector<Triangle>& OutHullTriangles) const
	{
		OutHullPoints.clear();
		OutHullTriangles.clear();

		std::array<glm::vec3, 4> Points;

		int32_t Idx = FindPointFurthestInDirection(InVertices, { 1.0f, 0.0f, 0.0f });
		Points[0] = InVertices[Idx];
		
		Idx = FindPointFurthestInDirection(InVertices, -1.0f * Points[0]);
		Points[1] = InVertices[Idx];
		Points[2] = FindPointFurthestFromLine(InVertices, Points[0], Points[1]);
		Points[3] = FindPointFurthestFromTriangle(InVertices, Points[0], Points[1], Points[2]);

		float Distance = GetDistanceFromTriangle(Points[0], Points[1], Points[2], Points[3]);
		if (Distance > 0.0f)
			std::swap(Points[0], Points[1]);

		OutHullPoints.push_back(Points[0]);
		OutHullPoints.push_back(Points[1]);
		OutHullPoints.push_back(Points[2]);
		OutHullPoints.push_back(Points[3]);

		Triangle Triangle0 = {};
		Triangle0.A = 0;
		Triangle0.B = 1;
		Triangle0.C = 2;
		OutHullTriangles.push_back(Triangle0);

		Triangle Triangle1 = {};
		Triangle1.A = 0;
		Triangle1.B = 2;
		Triangle1.C = 3;
		OutHullTriangles.push_back(Triangle1);

		Triangle Triangle2 = {};
		Triangle2.A = 2;
		Triangle2.B = 1;
		Triangle2.C = 3;
		OutHullTriangles.push_back(Triangle2);

		Triangle Triangle3 = {};
		Triangle3.A = 1;
		Triangle3.B = 0;
		Triangle3.C = 3;
		OutHullTriangles.push_back(Triangle3);
	}

	void ConvexShape::RemoveInternalPoints(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles, std::vector<glm::vec3>& InVertices) const
	{
		// Remove points that are inside the hull
		for (int32_t Idx = InVertices.size() - 1; Idx >= 0; Idx--)
		{
			const glm::vec3& Point = InVertices[Idx];

			bool IsExternal = false;
			for (size_t TriangleIdx = 0; TriangleIdx < InHullTriangles.size(); TriangleIdx++)
			{
				const Triangle& HullTriangle = InHullTriangles[TriangleIdx];

				const auto& TrianglePoint0 = InHullPoints[HullTriangle.A];
				const auto& TrianglePoint1 = InHullPoints[HullTriangle.B];
				const auto& TrianglePoint2 = InHullPoints[HullTriangle.C];

				if (GetDistanceFromTriangle(TrianglePoint0, TrianglePoint1, TrianglePoint2, Point) > 0.0f)
				{
					IsExternal = true;
					break;
				}
			}

			if (!IsExternal)
				InVertices.erase(InVertices.begin() + Idx);
		}

		// Remove points that are too close to the hull
		for (int32_t Idx = InVertices.size() - 1; Idx >= 0; Idx--)
		{
			const glm::vec3& Point = InVertices[Idx];

			bool IsTooClose = false;
			for (size_t PointIdx = 0; PointIdx < InHullPoints.size(); PointIdx++)
			{
				glm::vec3 Ray = InHullPoints[PointIdx] - Point;

				if (glm::dot(Ray, Ray) < 0.01f * 0.01f)
				{
					IsTooClose = true;
					break;
				}
			}

			if (IsTooClose)
				InVertices.erase(InVertices.begin() + Idx);
		}
	}

	bool ConvexShape::IsEdgeUnique(const std::vector<Triangle>& InTriangles, const std::vector<int32_t>& InFacingTriangles, int32_t InIgnoreTriangle, const Edge& InEdge) const
	{
		for (auto TriangleIdx : InFacingTriangles)
		{
			if (TriangleIdx == InIgnoreTriangle)
				continue;

			const auto& Tri = InTriangles[TriangleIdx];

			std::array<Edge, 3> Edges;
			Edges[0].A = Tri.A;
			Edges[0].B = Tri.B;

			Edges[1].A = Tri.B;
			Edges[1].B = Tri.C;

			Edges[2].A = Tri.C;
			Edges[2].B = Tri.A;

			for (const auto& Edge : Edges)
			{
				if (InEdge == Edge)
					return false;
			}
		}

		return true;
	}

	void ConvexShape::AddPoint(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles, const glm::vec3& InPoint) const
	{
		std::vector<int32_t> FacingTriangles;
		FacingTriangles.reserve(InHullTriangles.size());

		// Find all triangles that face InPoint
		for (int32_t Idx = InHullTriangles.size() - 1; Idx >= 0; Idx--)
		{
			const auto& Tri = InHullTriangles[Idx];

			const glm::vec3& Point0 = InHullPoints[Tri.A];
			const glm::vec3& Point1 = InHullPoints[Tri.B];
			const glm::vec3& Point2 = InHullPoints[Tri.C];

			if (GetDistanceFromTriangle(Point0, Point1, Point2, InPoint) > 0.0f)
				FacingTriangles.push_back(Idx);
		}

		// Find all edges that are unique to the triangles
		std::vector<Edge> UniqueEdges;
		UniqueEdges.reserve(FacingTriangles.size());
		for (auto TriangleIdx : FacingTriangles)
		{
			const auto& Tri = InHullTriangles[TriangleIdx];

			std::array<Edge, 3> Edges;
			Edges[0].A = Tri.A;
			Edges[0].B = Tri.B;

			Edges[1].A = Tri.B;
			Edges[1].B = Tri.C;

			Edges[2].A = Tri.C;
			Edges[2].B = Tri.A;

			for (const auto& Edge : Edges)
			{
				if (IsEdgeUnique(InHullTriangles, FacingTriangles, TriangleIdx, Edge))
					UniqueEdges.push_back(Edge);
			}
		}

		// Remove old facing triangles
		for (auto Idx : FacingTriangles)
			InHullTriangles.erase(InHullTriangles.begin() + Idx);

		// Add the new point
		InHullPoints.push_back(InPoint);
		int32_t PointIdx = InHullPoints.size() - 1;

		// Add triangles for all unique edges
		for (const auto& UniqueEdge : UniqueEdges)
		{
			Triangle Tri = {};
			Tri.A = UniqueEdge.A;
			Tri.B = UniqueEdge.B;
			Tri.C = PointIdx;
			InHullTriangles.push_back(Tri);
		}
	}

	void ConvexShape::RemoveUnreferencedVertices(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles) const
	{
		for (int32_t Idx = InHullPoints.size() - 1; Idx >= 0; Idx--)
		{
			bool IsUsed = false;

			for (const auto& Tri : InHullTriangles)
			{
				if (Tri.A == Idx || Tri.B == Idx || Tri.C == Idx)
				{
					IsUsed = true;
					break;
				}
			}

			if (IsUsed)
				continue;

			for (auto& Tri : InHullTriangles)
			{
				if (Tri.A > Idx)
					Tri.A--;
				
				if (Tri.B > Idx)
					Tri.B--;

				if (Tri.C > Idx)
					Tri.C--;
			}

			InHullPoints.erase(InHullPoints.begin() + Idx);
		}
	}

	void ConvexShape::ExpandConvexHull(std::vector<glm::vec3>& InHullPoints, std::vector<Triangle>& InHullTriangles, const std::vector<glm::vec3>& InVertices) const
	{
		// TODO(Peter): Preferably don't copy...
		std::vector<glm::vec3> ExternalVertices = InVertices;

		RemoveInternalPoints(InHullPoints, InHullTriangles, ExternalVertices);

		while (ExternalVertices.size() > 0)
		{
			int32_t PointIdx = FindPointFurthestInDirection(ExternalVertices, ExternalVertices[0]);
			glm::vec3 Point = ExternalVertices[PointIdx];
			ExternalVertices.erase(ExternalVertices.begin() + PointIdx);
			AddPoint(InHullPoints, InHullTriangles, Point);
			RemoveInternalPoints(InHullPoints, InHullTriangles, ExternalVertices);
		}

		RemoveUnreferencedVertices(InHullPoints, InHullTriangles);
	}

	void ConvexShape::BuildConvexHull(const std::vector<glm::vec3>& InVertices, std::vector<glm::vec3>& OutHullPoints, std::vector<Triangle>& OutHullTriangles) const
	{
		if (InVertices.size() < 4)
			return;

		BuildTetrahedron(InVertices, OutHullPoints, OutHullTriangles);
		ExpandConvexHull(OutHullPoints, OutHullTriangles, InVertices);
	}

	bool ConvexShape::IsPointExternal(const std::vector<glm::vec3>& InHullPoints, const std::vector<Triangle>& InHullTriangles, const glm::vec3& InPoint) const
	{
		bool IsExternal = false;

		for (const auto& Tri : InHullTriangles)
		{
			if (GetDistanceFromTriangle(InHullPoints[Tri.A], InHullPoints[Tri.B], InHullPoints[Tri.C], InPoint) > 0.0f)
			{
				IsExternal = true;
				break;
			}
		}

		return IsExternal;
	}

	glm::vec3 ConvexShape::CalculateCenterOfMass(const std::vector<glm::vec3>& InHullPoints, const std::vector<Triangle>& InHullTriangles) const
	{
		constexpr uint32_t SampleCount = 100;

		AABB BoundingBox;
		BoundingBox.Expand(InHullPoints);

		glm::vec3 CenterOfMass(0.0f);
		float SampleWidth = BoundingBox.GetWidth() / static_cast<float>(SampleCount);
		float SampleHeight = BoundingBox.GetHeight() / static_cast<float>(SampleCount);
		float SampleDepth = BoundingBox.GetDepth() / static_cast<float>(SampleCount);

		const auto& MinBound = BoundingBox.GetMinBound();
		const auto& MaxBound = BoundingBox.GetMaxBound();

		uint32_t CurrentSample = 0;
		for (float X = MinBound.x; X < MaxBound.x; X += SampleWidth)
		{
			for (float Y = MinBound.y; Y < MaxBound.y; Y += SampleHeight)
			{
				for (float Z = MinBound.z; Z < MaxBound.z; Z += SampleDepth)
				{
					glm::vec3 Point(X, Y, Z);

					if (IsPointExternal(InHullPoints, InHullTriangles, Point))
						continue;

					CenterOfMass += Point;
					CurrentSample++;
				}
			}
		}

		CenterOfMass /= CurrentSample;
		return CenterOfMass;
	}

	glm::mat3 ConvexShape::CalculateInertiaTensor(const std::vector<glm::vec3>& InHullPoints, const std::vector<Triangle>& InHullTriangles, const glm::vec3& InCenterOfMass) const
	{
		constexpr uint32_t SampleCount = 100;

		AABB BoundingBox;
		BoundingBox.Expand(InHullPoints);

		glm::mat3 InertiaTensor(0.0f);

		float SampleWidth = BoundingBox.GetWidth() / static_cast<float>(SampleCount);
		float SampleHeight = BoundingBox.GetHeight() / static_cast<float>(SampleCount);
		float SampleDepth = BoundingBox.GetDepth() / static_cast<float>(SampleCount);

		const auto& MinBound = BoundingBox.GetMinBound();
		const auto& MaxBound = BoundingBox.GetMaxBound();

		uint32_t CurrentSample = 0;

		for (float X = MinBound.x; X < MaxBound.x; X += SampleWidth)
		{
			for (float Y = MinBound.y; Y < MaxBound.y; Y += SampleHeight)
			{
				for (float Z = MinBound.z; Z < MaxBound.z; Z += SampleDepth)
				{
					glm::vec3 Point(X, Y, Z);

					if (IsPointExternal(InHullPoints, InHullTriangles, Point))
						continue;

					Point -= InCenterOfMass;

					InertiaTensor[0][0] += Point.y * Point.y + Point.z * Point.z;
					InertiaTensor[1][1] += Point.z * Point.z + Point.x * Point.x;
					InertiaTensor[1][1] += Point.x * Point.x + Point.y * Point.y;

					InertiaTensor[0][1] += -1.0f * Point.x * Point.y;
					InertiaTensor[0][2] += -1.0f * Point.x * Point.z;
					InertiaTensor[1][2] += -1.0f * Point.y * Point.z;

					InertiaTensor[1][0] += -1.0f * Point.x * Point.y;
					InertiaTensor[2][0] += -1.0f * Point.x * Point.z;
					InertiaTensor[2][1] += -1.0f * Point.y * Point.z;

					CurrentSample++;
				}
			}
		}

		InertiaTensor *= 1.0f / static_cast<float>(CurrentSample);
		return InertiaTensor;
	}

	glm::vec2 ConvexShape::ProjectSignedVolume1D(const glm::vec3& InLineStart, const glm::vec3& InLineEnd) const
	{
		glm::vec3 Distance = InLineEnd - InLineStart;
		glm::vec3 RayToOrigin = glm::vec3(0.0f) - InLineStart;
		glm::vec3 Point0 = InLineStart + Distance * glm::dot(Distance, RayToOrigin) / glm::dot(RayToOrigin, RayToOrigin);

		uint32_t Idx = 0;
		float AxisMax = 0.0f;
		for (uint32_t I = 0; I < 3; I++)
		{
			float AxisDistance = InLineEnd[I] - InLineStart[I];

			if (AxisDistance * AxisDistance > AxisMax * AxisMax)
			{
				AxisMax = AxisDistance;
				Idx = I;
			}
		}

		float S = InLineStart[Idx];
		float E = InLineEnd[Idx];
		float P = Point0[Idx];

		float C1 = P - S;
		float C2 = E - P;

		// If P is between S and E
		if ((P > S && P < E) || (P > E && P < S))
			return glm::vec2(C2 / AxisMax, C1 / AxisMax);

		// If P is on the far side of S
		if ((S <= E && P <= S) || (S >= E && P >= S))
			return glm::vec2(1.0f, 0.0f);

		// P has to be on the far side of E
		return glm::vec2(0.0f, 1.0f);
	}

	static bool CompareSigns(float InValue0, float InValue1)
	{
		if (InValue0 > 0.0f && InValue1 > 0.0f)
			return true;

		if (InValue0 < 0.0f && InValue1 < 0.0f)
			return true;

		return false;
	}

	glm::vec3 ConvexShape::ProjectSignedVolume2D(const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2) const
	{
		glm::vec3 Normal = glm::cross(InPoint1 - InPoint0, InPoint2 - InPoint0);
		glm::vec3 P0 = Normal * glm::dot(InPoint0, Normal) / glm::dot(Normal, Normal);

		uint32_t Idx = 0;
		float AreaMax = 0.0f;
		for (uint32_t I = 0; I < 3; I++)
		{
			uint32_t J = (I + 1) % 3;
			uint32_t K = (I + 2) % 3;

			glm::vec2 A(InPoint0[J], InPoint0[K]);
			glm::vec2 B(InPoint1[J], InPoint1[K]);
			glm::vec2 C(InPoint2[J], InPoint2[K]);

			glm::vec2 AB = B - A;
			glm::vec2 AC = C - A;

			float Area = AB.x * AC.y - AB.y * AC.x;
			if (Area * Area > AreaMax * AreaMax)
			{
				Idx = I;
				AreaMax = Area;
			}
		}

		// Project onto the appropriate axis
		uint32_t X = (Idx + 1) % 3;
		uint32_t Y = (Idx + 2) % 3;
		std::array<glm::vec2, 3> Points =
		{
			glm::vec2(InPoint0[X], InPoint0[Y]),
			glm::vec2(InPoint1[X], InPoint1[Y]),
			glm::vec2(InPoint2[X], InPoint2[Y])
		};

		glm::vec2 P1(P0[X], P0[Y]);

		glm::vec3 Areas;
		for (uint32_t I = 0; I < 3; I++)
		{
			uint32_t J = (I + 1) % 3;
			uint32_t K = (I + 2) % 3;

			glm::vec2 AB = Points[J] - P1;
			glm::vec2 AC = Points[K] - P1;

			Areas[I] = AB.x * AC.y - AB.y * AC.x;
		}

		// Return the barycentric points if projected origin is inside triangle
		if (CompareSigns(AreaMax, Areas[0]) && CompareSigns(AreaMax, Areas[1]) && CompareSigns(AreaMax, Areas[2]))
			return Areas / AreaMax;

		// Project onto edges and return closest point
		float Distance = 1e10;

		const std::array<glm::vec3, 3> EdgePoints = { InPoint0, InPoint1, InPoint2 };

		glm::vec3 Lambdas(1.0f, 0.0f, 0.0f);

		for (uint32_t I = 0; I < 3; I++)
		{
			uint32_t J = (I + 1) % 3;
			uint32_t K = (I + 2) % 3;

			glm::vec2 LambdaEdge = ProjectSignedVolume1D(EdgePoints[J], EdgePoints[K]);
			glm::vec3 Point = EdgePoints[J] * LambdaEdge[0] + EdgePoints[K] * LambdaEdge[1];
			if (glm::dot(Point, Point) < Distance)
			{
				Distance = glm::dot(Point, Point);
				Lambdas[I] = 0.0f;
				Lambdas[J] = LambdaEdge[0];
				Lambdas[K] = LambdaEdge[1];
			}
		}

		return Lambdas;
	}

	glm::vec4 ConvexShape::ProjectSignedVolume3D(const glm::vec3& InPoint0, const glm::vec3& InPoint1, const glm::vec3& InPoint2, const glm::vec3& InPoint3) const
	{
		// TODO(Peter): Verify that M is actually correct (Should it be reversed?)

		glm::mat4 M(1.0f);
		M[0] = glm::vec4(InPoint0.x, InPoint0.y, InPoint0.z, 1.0f);
		M[1] = glm::vec4(InPoint1.x, InPoint1.y, InPoint1.z, 1.0f);
		M[2] = glm::vec4(InPoint2.x, InPoint2.y, InPoint2.z, 1.0f);
		M[3] = glm::vec4(InPoint3.x, InPoint3.y, InPoint3.z, 1.0f);

		glm::mat4 CofactorMatrix = glm::transpose(glm::adjugate(M));

		glm::vec4 C4;
		C4[0] = CofactorMatrix[0].w;
		C4[1] = CofactorMatrix[1].w;
		C4[2] = CofactorMatrix[2].w;
		C4[3] = CofactorMatrix[3].w;

		float Determinant = C4[0] + C4[1] + C4[2] + C4[3];

		if (CompareSigns(Determinant, C4[0]) && CompareSigns(Determinant, C4[1]) && CompareSigns(Determinant, C4[2]) && CompareSigns(Determinant, C4[3]))
			return C4 * (1.0f / Determinant);

		const std::array<glm::vec3, 4> FacePoints = { InPoint0, InPoint1, InPoint2, InPoint3 };

		glm::vec4 Lambdas;
		float Distance = 1e10;
		for (uint32_t I = 0; I < 4; I++)
		{
			uint32_t J = (I + 1) % 4;
			uint32_t K = (I + 2) % 4;

			glm::vec3 LambdasFace = ProjectSignedVolume2D(FacePoints[I], FacePoints[J], FacePoints[K]);
			glm::vec3 Point = FacePoints[I] * LambdasFace[0] + FacePoints[J] * LambdasFace[1] + FacePoints[K] * LambdasFace[2];

			if (glm::dot(Point, Point) < Distance)
			{
				Distance = glm::dot(Point, Point);
				Lambdas = glm::vec4(0.0f);
				Lambdas[I] = LambdasFace[0];
				Lambdas[J] = LambdasFace[1];
				Lambdas[K] = LambdasFace[2];
			}
		}

		return Lambdas;
	}

}
