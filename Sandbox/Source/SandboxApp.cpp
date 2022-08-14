#include <Fusion.h>
#include <Fusion/EntryPoint.h>

#include <iostream>

class SandboxApp : public Fusion::Application
{
public:
	SandboxApp(const Fusion::ApplicationSpecification& specification)
		: Application(specification)
	{
		std::cout << "Construct" << std::endl;
	}

	virtual void OnInit() override
	{
		std::cout << "OnInit" << std::endl;
	}

	virtual void OnUpdate(float DeltaTime) override
	{
		std::cout << "OnUpdate: " << DeltaTime << std::endl;
	}

	virtual void OnShutdown() override
	{
		std::cout << "OnShutdown" << std::endl;
	}
};

Fusion::Application* Fusion::CreateApplication(int ArgC, char** ArgV)
{
	Fusion::ApplicationSpecification specification = {};
	specification.Title = "Sandbox App";

	return new SandboxApp(specification);
}
