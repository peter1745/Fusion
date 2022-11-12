#include "FusionPCH.hpp"
#include "RenderSettings.hpp"

namespace Fusion {

	RenderSettings& RenderSettings::Get()
	{
		static RenderSettings s_Settings;
		return s_Settings;
	}

}
