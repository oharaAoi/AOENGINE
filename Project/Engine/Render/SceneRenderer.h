#pragma once
#include <utility>
#include <memory>
#include <list>
#include <unordered_map>
#include "Engine/DirectX/Pipeline/PipelineGroup/Object3dPipelines.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/System/Scene/SceneLoader.h"

/// <summary>
/// Sceneのレンダリングを行うクラス
/// </summary>
class SceneRenderer {
public:

	struct ObjectPair {
		std::string renderingType;
		std::unique_ptr<BaseGameObject> object;

		ObjectPair(std::string type, std::unique_ptr<BaseGameObject> obj)
			: renderingType(type), object(std::move(obj)) {
		}
	};

public:

	SceneRenderer() = default;
	~SceneRenderer() = default;
	SceneRenderer(const SceneRenderer&) = delete;
	const SceneRenderer& operator=(const SceneRenderer&) = delete;

	static SceneRenderer* GetInstance();

	void Finalize();

	void Init();

	void Update();

	void Draw() const;

	void CreateObject(const SceneLoader::LevelData* loadData);

	BaseGameObject* AddObject(const std::string& objectName, const std::string& renderingName);

	void ReleaseObject(BaseGameObject* objPtr);

	void ChangeRenderingType(const std::string& renderingName, BaseGameObject* gameObject);

public:

	BaseGameObject* GetGameObject(const std::string& objName);

private:

	std::list<ObjectPair> objectList_;

};

