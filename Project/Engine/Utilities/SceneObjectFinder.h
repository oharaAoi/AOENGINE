#pragma once
#include <string>
#include "Engine/Render/SceneRenderer.h"

template<class T>
T* FindSceneObject(const std::string& objectName) {
    AOENGINE::SceneRenderer* renderer =
        AOENGINE::SceneRenderer::GetInstance();

    if (!renderer) {
        return nullptr;
    }

    for (const AOENGINE::ObjectHandle& handle : renderer->GetObjectHandles()) {
        AOENGINE::SceneObject* object = renderer->FindObject(handle);
        if (!object || object->GetName() != objectName) {
            continue;
        }

        if (T* result = dynamic_cast<T*>(object)) {
            return result;
        }
    }

    return nullptr;
}

