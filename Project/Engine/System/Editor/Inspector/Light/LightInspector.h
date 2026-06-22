#pragma once

#include "Engine/Module/Components/Light/LightGroup.h"

namespace AOENGINE {

/// <summary>
/// Light共通パラメータのInspector。
/// </summary>
class BaseLightInspector {
public:
	static void DrawBaseParameters(BaseLight& light, const char* label);
};

/// <summary>
/// LightGroupのInspector。
/// </summary>
class LightGroupInspector {
public:
	static void Draw(LightGroup& lightGroup);
};

/// <summary>
/// DirectionalLightのInspector。
/// </summary>
class DirectionalLightInspector {
public:
	static void Draw(DirectionalLight& light);
};

/// <summary>
/// PointLightのInspector。
/// </summary>
class PointLightInspector {
public:
	static void Draw(PointLight& light);
};

/// <summary>
/// SpotLightのInspector。
/// </summary>
class SpotLightInspector {
public:
	static void Draw(SpotLight& light);
};

}
