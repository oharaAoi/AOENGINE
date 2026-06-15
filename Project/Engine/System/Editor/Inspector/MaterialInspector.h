#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

/// <summary>
/// BaseGameObjectが持つMaterialを編集するInspector
/// </summary>
class MaterialInspector {
public:
	static void Draw(BaseGameObject& object);
};

}
