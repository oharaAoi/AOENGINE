#include "CameraInspector.h"

void AOENGINE::Camera3dInspector::Draw(Camera3d& camera) {

	camera.Debug_Gui();
}

void AOENGINE::Camera2dInspector::Draw(Camera2d& camera) {

	camera.Debug_Gui();
}

void AOENGINE::DebugCameraInspector::Draw(DebugCamera& camera) {

	camera.Debug_Gui();
}
