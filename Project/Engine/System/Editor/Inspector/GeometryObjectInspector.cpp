#include "GeometryObjectInspector.h"

using namespace AOENGINE;

void GeometryObjectInspector::Draw(GeometryObject& object) {
	object.GetTransform()->Debug_Gui();
	object.GetMaterial()->Debug_Gui();
}
