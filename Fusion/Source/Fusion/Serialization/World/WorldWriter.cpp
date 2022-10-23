#include "FusionPCH.h"
#include "WorldWriter.h"
#include "ComponentSerialization.h"

#include "Fusion/Serialization/YAMLCustomConverters.h"

namespace Fusion {

	void WorldYAMLWriter::WriteWorld(const Shared<World>& InWorld)
	{
		m_Emitter << YAML::BeginMap;
		m_Emitter << YAML::Key << "World" << YAML::Value << InWorld->GetName();

		{
			m_Emitter << YAML::Key << "Actors" << YAML::Value << YAML::BeginSeq;

			auto Actors = InWorld->FindAllActorsWith<TransformComponent>();

			for (auto Actor : Actors)
			{
				m_Emitter << YAML::BeginMap;
				m_Emitter << YAML::Key << "Actor" << YAML::Value << Actor->GetActorID();
				m_Emitter << YAML::Key << "Name" << YAML::Value << Actor->Name;

				ComponentUtils::AllConst(Actor, [this, CurrentActor = Actor]<typename TComponent>(const TComponent* InComponent)
				{
					m_Emitter << YAML::Key << ComponentSerializer<TComponent>::Name;
					m_Emitter << YAML::BeginMap;
					ComponentSerializer<TComponent>::Serialize(*this, InComponent);
					m_Emitter << YAML::EndMap;
				});

				m_Emitter << YAML::EndMap;
			}

			m_Emitter << YAML::EndSeq;
		}

		m_Emitter << YAML::EndMap;
	}

	void WorldYAMLWriter::Write(const char* InName, const auto& InValue)
	{
		m_Emitter << YAML::Key << InName << YAML::Value << InValue;
	}

	void WorldYAMLWriter::Close(const std::filesystem::path& InFilePath)
	{
		std::ofstream OutStream(InFilePath);
		OutStream << m_Emitter.c_str();
		OutStream.close();
	}

}
