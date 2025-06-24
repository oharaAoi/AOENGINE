#pragma once
#include <utility>
#include <memory>
#include <list>
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/Utilities/Logger.h"

/// <summary>
/// Sceneのレンダリングを行うクラス
/// </summary>
class SceneRenderer {
public:	// 構造体データ

	struct IObjectPair {
		virtual ~IObjectPair() = default;
		virtual ISceneObject* GetSceneObject() = 0;
		virtual const std::string& GetRenderingType() const = 0;
		virtual void SetRenderingType(const std::string& name) = 0;
	};

	template <typename T>
	struct ObjectPair : IObjectPair {
		std::string renderingType;
		std::unique_ptr<T> object;

		ObjectPair(const std::string& type, std::unique_ptr<T> obj)
			: renderingType(type), object(std::move(obj)) {
		}

		T* GetSceneObject() override { return object.get(); }

		const std::string& GetRenderingType() const override { return renderingType; }
		void SetRenderingType(const std::string& name) override { renderingType = name; }
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

	template<typename T, typename... Args>
	T* AddObject(const std::string& objectName, const std::string& renderingName, Args&&... args) {
		static_assert(std::is_base_of<ISceneObject, T>::value, "T must derive from ISceneObject");

		auto pair = std::make_unique<ObjectPair<T>>(
			renderingName,
			std::make_unique<T>(std::forward<Args>(args)...)
		);
		T* gameObject = static_cast<T*>(pair->object.get());
		pair->object->Init();
		pair->object->SetName(objectName);

		objectList_.push_back(std::move(pair));

		return gameObject;
	}

	/// <summary>
	/// Objectを解放する
	/// </summary>
	/// <param name="objPtr">: 解放するobjectの種類</param>
	void ReleaseObject(ISceneObject* objPtr);

	/// <summary>
	/// Renderingのタイプを変更する
	/// </summary>
	/// <param name="renderingName">: 変更後のレンダリングのタイプ</param>
	/// <param name="gameObject">: 変更するobjectのポインタ</param>
	void ChangeRenderingType(const std::string& renderingName, ISceneObject* gameObject);

public:

	template<typename T>
	T* GetGameObject(const std::string& objName) {
		static_assert(std::is_base_of<ISceneObject, T>::value, "T must derive from ISceneObject");

		for (auto& pair : objectList_) {
			if (pair->GetSceneObject()->GetName() == objName) {
				return dynamic_cast<T*>(pair->GetSceneObject());  // 安全にキャスト
			}
		}

		Logger::Log("[" + objName + "]が見つかりませんでした");
		return nullptr;
	}

private:

	std::list<std::unique_ptr<IObjectPair>> objectList_;

	ParticleManager* particleManager_;

};

