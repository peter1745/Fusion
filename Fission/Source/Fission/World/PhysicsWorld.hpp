#pragma once

#include "Fission/Body/Body.hpp"
#include "Fission/Integration/Integrator.hpp"

#include <vector>
#include <memory>

namespace Fission {

	struct PhysicsSettings
	{
		glm::vec3 Gravity = { 0.0f, -9.81f, 0.0f };
	};

	class PhysicsWorld
	{
	public:
		PhysicsWorld() = default;

		void Init();
		void Shutdown();

	public:
		void Simulate(float InDeltaTime);

		Body* CreateBody(const BodySettings& InSettings);

	public:
		PhysicsSettings& GetSettings() { return m_Settings; }
		const PhysicsSettings& GetSettings() const { return m_Settings; }
		void SetSettings(const PhysicsSettings& InSettings) { m_Settings = InSettings; }

	private:
		PhysicsSettings m_Settings;

		std::unique_ptr<Integrator> m_BodyIntegrator = nullptr;

		std::vector<std::unique_ptr<Body>> m_Bodies;
		float m_DeltaTime = 0.0f;
	};

}
