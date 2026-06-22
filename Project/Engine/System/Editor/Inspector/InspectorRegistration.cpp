
#include "InspectorRegistration.h"
#include "InspectorRegistry.h"
#include "Engine/System/Editor/Inspector/Entity/BaseGameObjectInspector.h"
#include "Engine/System/Editor/Inspector/Entity/GeometryObjectInspector.h"
#include "Engine/System/Editor/Inspector/Entity/CameraInspector.h"
#include "Engine/System/Editor/Inspector/Light/LightInspector.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 標準Inspectorを登録する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::RegisterDefaultInspectors() {
	InspectorRegistry& registry = InspectorRegistry::GetInstance();

	registry.RegisterObjectDrawer<BaseGameObject>(
		[](BaseGameObject& object) {
			BaseGameObjectInspector::Draw(object);
		}
	);

	registry.RegisterObjectDrawer<GeometryObject>(
		[](GeometryObject& object) {
			GeometryObjectInspector::Draw(object);
		}
	);

	registry.RegisterObjectDrawer<LightGroup>(
		[](LightGroup& lightGroup) {
			LightGroupInspector::Draw(lightGroup);
		}
	);

	registry.RegisterObjectDrawer<DirectionalLight>(
		[](DirectionalLight& light) {
			DirectionalLightInspector::Draw(light);
		}
	);

	registry.RegisterObjectDrawer<PointLight>(
		[](PointLight& light) {
			PointLightInspector::Draw(light);
		}
	);

	registry.RegisterObjectDrawer<SpotLight>(
		[](SpotLight& light) {
			SpotLightInspector::Draw(light);
		}
	);

	registry.RegisterObjectDrawer<Camera3d>(
		[](Camera3d& camera) {
			Camera3dInspector::Draw(camera);
		}
	);

	registry.RegisterObjectDrawer<Camera2d>(
		[](Camera2d& camera) {
			Camera2dInspector::Draw(camera);
		}
	);

	registry.RegisterObjectDrawer<DebugCamera>(
		[](DebugCamera& camera) {
			DebugCameraInspector::Draw(camera);
		}
	);
}
