#include "FusionPCH.hpp"
#include "WorldReader.hpp"
#include "ComponentSerialization.hpp"

namespace Fusion {

	WorldYAMLReader::WorldYAMLReader(const std::filesystem::path& InFilePath)
	{
		std::ifstream StreamIn(InFilePath);
		FUSION_CORE_VERIFY(StreamIn);
		std::stringstream StreamBuffer;
		StreamBuffer << StreamIn.rdbuf();

		try
		{
			m_RootNode = YAML::Load(StreamBuffer);
		}
		catch (YAML::ParserException InException)
		{
			FUSION_CORE_ERROR("Failed to load world file '{}' Error: {}", InFilePath.string(), InException.what());
		}

		StreamIn.close();
	}

	void WorldYAMLReader::ReadWorld(Shared<World> InWorld)
	{
		if (!m_RootNode)
			return;

		if (!m_RootNode["World"])
		{
			FUSION_CORE_ERROR("Can't load world file, no 'World' entry found!");
			return;
		}

		InWorld->SetName(m_RootNode["World"].as<std::string>("Empty World"));
		FUSION_CORE_INFO("Deserializing World '{}'", InWorld->GetName());

		auto ActorList = m_RootNode["Actors"];
		if (ActorList)
		{
			for (auto ActorNode : ActorList)
			{
				uint64_t ActorID = ActorNode["Actor"].as<uint64_t>(0);
				std::string Name = ActorNode["Name"].as<std::string>("Unnamed Actor");
				Shared<Actor> NewActor = InWorld->CreateActorWithID(ActorID, Name);

				for (auto Node : ActorNode)
				{
					std::string NodeName = Node.first.as<std::string>();

					if (NodeName == "Actor" || NodeName == "Name")
						continue;

					// If we get here we know that this node represents a component
					ComponentUtils::AllFindByName(NodeName, [this, &NewActor, &ActorNode, &NodeName]<typename TComponent>(TComponent& InComponent)
					{
						m_CurrentComponentNode = ActorNode[NodeName];
						ComponentSerializer<TComponent>::Deserialize(*this, InComponent);
						NewActor->AddOrReplaceComponent<TComponent>(InComponent);
					});
				}
			}
		}
	}

}
