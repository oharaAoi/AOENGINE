#pragma once
#include <utility>
#include <list>
#include <unordered_map>
#include "Engine/DirectX/Pipeline/PipelineGroup/Object3dPipelines.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

/// <summary>
/// Sceneのレンダリングを行うクラス
/// </summary>
class SceneRenderer {
public:

	struct ObjectPair {
		Object3dPSO renderingType;
		BaseGameObject* object;

		ObjectPair(Object3dPSO type, BaseGameObject* obj)
			: renderingType(type), object(obj) {
		}
	};

public:

	SceneRenderer() = default;
	~SceneRenderer() = default;
	SceneRenderer(const SceneRenderer&) = delete;
	const SceneRenderer& operator=(const SceneRenderer&) = delete;

	static SceneRenderer* GetInstance();

	void Init();

	void Update();

	void Draw() const;

	void SetObject(Object3dPSO type, BaseGameObject* object);

	void ReleaseObject(BaseGameObject* object);

private:

	std::unordered_map<Object3dPSO, std::list<BaseGameObject*>> object3dMap_;

	std::list<ObjectPair> object3dList_;

};

