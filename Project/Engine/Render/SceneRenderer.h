#pragma once
#include <utility>
#include <memory>
#include <list>
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/System/Scene/SceneLoader.h"

/// <summary>
/// Sceneのレンダリングを行うクラス
/// </summary>
class SceneRenderer {
public:	// 構造体データ

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

public:

	/// <summary>
	/// Objectを生成する
	/// </summary>
	/// <param name="loadData">: 生成データ</param>
	void CreateObject(const SceneLoader::LevelData* loadData);

	/// <summary>
	/// Objectを追加する
	/// </summary>
	/// <param name="objectName">: 追加するobjectの名前</param>
	/// <param name="renderingName">: レンダリングの種類の名前</param>
	/// <returns></returns>
	BaseGameObject* AddObject(const std::string& objectName, const std::string& renderingName);

	/// <summary>
	/// Objectを解放する
	/// </summary>
	/// <param name="objPtr">: 解放するobjectの種類</param>
	void ReleaseObject(BaseGameObject* objPtr);

	/// <summary>
	/// Renderingのタイプを変更する
	/// </summary>
	/// <param name="renderingName">: 変更後のレンダリングのタイプ</param>
	/// <param name="gameObject">: 変更するobjectのポインタ</param>
	void ChangeRenderingType(const std::string& renderingName, BaseGameObject* gameObject);

public:

	BaseGameObject* GetGameObject(const std::string& objName);

private:

	std::list<ObjectPair> objectList_;

	ParticleManager* particleManager_;

};

