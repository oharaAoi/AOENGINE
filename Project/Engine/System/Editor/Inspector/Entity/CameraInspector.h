#pragma once
#include "Engine/Module/Entity/Camera/Camera3d.h"
#include "Engine/Module/Entity/Camera/Camera2d.h"
#include "Engine/Module/Entity/Camera/DebugCamera.h"

namespace AOENGINE {

/// <summary>
/// Camera3dのInspector
/// </summary>
class Camera3dInspector {
public:

	static void Draw(Camera3d& camera);

};

/// <summary>
/// Camera2dのInspector
/// </summary>
class Camera2dInspector {
public:

	static void Draw(Camera2d& camera);

};

/// <summary>
/// DebugCameraのInspector
/// </summary>
class DebugCameraInspector {
public:

	static void Draw(DebugCamera& camera);

};

}