#include <Fusion.h>
#include <Fusion/EntryPoint.h>
#include <Fusion/Memory/Shared.h>

#include <iostream>

class SandboxApp : public Fusion::Application
{
public:
	SandboxApp(const Fusion::ApplicationSpecification& specification)
		: Application(specification)
	{
		FUSION_CLIENT_INFO("Constructor!");
	}

	virtual void OnInit() override
	{
		FUSION_CLIENT_INFO("OnInit!");
	}

	virtual void OnUpdate(float DeltaTime) override
	{
		//FUSION_CLIENT_INFO("OnUpdate: {}", DeltaTime);
	}

	virtual void OnShutdown() override
	{
		FUSION_CLIENT_INFO("OnShutdown!");
	}
};

Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV)
{
	Fusion::ApplicationSpecification specification = {};
	specification.Title = "Sandbox App";

	return new SandboxApp(specification);
}
