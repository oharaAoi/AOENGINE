
#include "InspectorRegistration.h"
#include "InspectorRegistry.h"
#include "BaseGameObjectInspector.h"
#include "GeometryObjectInspector.h"

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
}
