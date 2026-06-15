#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

/// <summary>
/// Inspector for editing colliders owned by BaseGameObject.
/// </summary>
class ColliderInspector {
public:
	static void Draw(BaseGameObject& object);
};

}
