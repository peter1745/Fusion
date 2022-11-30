#include "BoxShape.hpp"

namespace Fission {

	BoxShape::BoxShape(const glm::vec3& InHalfSize)
		: ConvexShape(EShapeType::Box), m_HalfSize(InHalfSize)
	{
		m_BoundingBox.Expand(-InHalfSize);
		m_BoundingBox.Expand(InHalfSize);

		const auto& MinBound = m_BoundingBox.GetMinBound();
		const auto& MaxBound = m_BoundingBox.GetMaxBound();

		m_Points.resize(8);

		m_Points[0] = MinBound;
		m_Points[1] = { MaxBound.x, MinBound.y, MinBound.z };
		m_Points[2] = { MinBound.x, MaxBound.y, MinBound.z };
		m_Points[3] = { MinBound.x, MinBound.y, MaxBound.z };

		m_Points[4] = MaxBound;
		m_Points[5] = { MinBound.x, MaxBound.y, MinBound.z };
		m_Points[6] = { MinBound.x, MinBound.y, MaxBound.z };
		m_Points[7] = { MaxBound.x, MaxBound.y, MinBound.z };

		m_CenterOfMass = (MaxBound + MinBound) * 0.5f;

		std::vector<glm::vec3> HullPoints;
		std::vector<Triangle> HullTriangles;
		BuildConvexHull(m_Points, HullPoints, HullTriangles);
		m_Points = HullPoints;

		m_BoundingBox.Reset();
		m_BoundingBox.Expand(m_Points);

		m_CenterOfMass = CalculateCenterOfMass(HullPoints, HullTriangles);
		m_InertiaTensor = CalculateInertiaTensor(HullPoints, HullTriangles, m_CenterOfMass);
	}

	/*void BoxShape::CalculateInertiaTensor()
	{
		const auto& MinBound = m_BoundingBox.GetMinBound();
		const auto& MaxBound = m_BoundingBox.GetMaxBound();

		float DX = MaxBound.x - MinBound.x;
		float DY = MaxBound.y - MinBound.y;
		float DZ = MaxBound.z - MinBound.z;

		constexpr float OneOverTwelve = 0.0833333f;

		m_InertiaTensor = glm::mat3(0.0f);
		m_InertiaTensor[0][0] = (DY * DY + DZ * DZ) * OneOverTwelve;
		m_InertiaTensor[1][1] = (DX * DX + DZ * DZ) * OneOverTwelve;
		m_InertiaTensor[2][2] = (DX * DX + DY * DY) * OneOverTwelve;

		glm::vec3 Displacement = glm::zero<glm::vec3>() - m_CenterOfMass;
		float DisplacementLength = glm::dot(Displacement, Displacement);
		glm::mat3 PatTensor = glm::mat3(0.0f);
		PatTensor[0] = glm::vec3(DisplacementLength - Displacement.x * Displacement.x, Displacement.x * Displacement.y, Displacement.x * Displacement.z);
		PatTensor[1] = glm::vec3(Displacement.y * Displacement.x, DisplacementLength - Displacement.y * Displacement.y, Displacement.y * Displacement.z);
		PatTensor[2] = glm::vec3(Displacement.z * Displacement.x, Displacement.z * Displacement.y, DisplacementLength - Displacement.z * Displacement.z);

		m_InertiaTensor += PatTensor;
	}*/

}
