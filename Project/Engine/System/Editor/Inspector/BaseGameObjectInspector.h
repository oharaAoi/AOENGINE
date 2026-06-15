#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

class BaseGameObjectInspector {
public:
    static void Draw(BaseGameObject& object);

    static void DrawAddComponent(BaseGameObject& object);
};

}