#include "FissionPCH.hpp"
#include "ContactResolver.hpp"

#include "Fission/Body/DynamicBody.hpp"

#include "Shapes/SphereShape.hpp"
#include "Shapes/BoxShape.hpp"

#include <array>

#include <iostream>
#include <format>

namespace Fission {

	void ContactResolver::GenerateContact(Body* InBodyA, Body* InBodyB)
	{
		auto& Contact = m_Contacts.emplace_back();
		Contact.BodyA = InBodyA;
		Contact.BodyB = InBodyB;

		Contact.Normal = InBodyB->GetPosition() - InBodyA->GetPosition();

		Contact.WorldSpacePointA = GetContactPosition(InBodyA->GetPosition(), Contact.Normal, InBodyA->GetShape());
		Contact.WorldSpacePointB = GetContactPosition(InBodyB->GetPosition(), -Contact.Normal, InBodyB->GetShape());

		std::cout << std::format("Normal={},{},{}", Contact.Normal.x, Contact.Normal.y, Contact.Normal.z) << std::endl;
	}

	void ContactResolver::ResolveContacts()
	{
		for (auto& Contact : m_Contacts)
		{
			float InvMassA = 0.0f;
			float InvMassB = 0.0f;

			if (Contact.BodyA->GetType() == EBodyType::Dynamic)
			{
				auto* DynamicA = Contact.BodyA->As<DynamicBody>();
				InvMassA = DynamicA->GetInverseMass();
				DynamicA->SetLinearVelocity(glm::zero<glm::vec3>());
			}

			if (Contact.BodyB->GetType() == EBodyType::Dynamic)
			{
				auto* DynamicB = Contact.BodyB->As<DynamicBody>();
				InvMassB = DynamicB->GetInverseMass();
				DynamicB->SetLinearVelocity(glm::zero<glm::vec3>());
			}

			float MassContributionA = InvMassA / (InvMassA + InvMassB);
			float MassContributionB = InvMassB / (InvMassA + InvMassB);

			glm::vec3 Distance = Contact.WorldSpacePointB - Contact.WorldSpacePointA;
			Contact.BodyA->SetPosition(Contact.BodyA->GetPosition() + Distance * MassContributionA);
			Contact.BodyB->SetPosition(Contact.BodyB->GetPosition() - Distance * MassContributionB);
		}
	}

	glm::vec3 ContactResolver::GetContactPosition(const glm::vec3& InBodyPosition, const glm::vec3& InNormal, const Shape* InShape) const
	{
		switch (InShape->GetType())
		{
		case EShapeType::Sphere: return InBodyPosition + InNormal * static_cast<const SphereShape*>(InShape)->GetRadius();
		case EShapeType::Box:
		{
			Math::AABB BoundingBox = static_cast<const BoxShape*>(InShape)->GetBoundingBox();

			int BestAxis = 0;
			float BestAxisWeight = 0.0f;

			for (int Idx = 0; Idx < 3; Idx++)
			{
				float Weight = glm::abs(InNormal[Idx] / BoundingBox.MaxBound[Idx]);

				if (Weight > BestAxisWeight)
				{
					BestAxis = Idx;
					BestAxisWeight = Weight;
				}
			}

			return (InNormal * BoundingBox.MaxBound[BestAxis] / glm::abs(InNormal[BestAxis])) + InBodyPosition;
		}
		}
	}

}
