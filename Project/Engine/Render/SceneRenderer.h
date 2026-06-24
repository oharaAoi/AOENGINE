#pragma once

#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "Engine/System/Manager/ImGuiManager.h"

#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/System/Scene/SceneWorld.h"
#include "Engine/Render/ModelInstancingRenderer.h"
#include "Engine/Utilities/Logger.h"

namespace Math {
class Frustum;
}

namespace AOENGINE {

class BaseGameObject;

/// <summary>
/// SceneWorldが所有するオブジェクトを描画するクラス。
/// SceneRenderer自身はオブジェクトを所有せず、描画に必要な設定だけをObjectHandleに紐づけて保持します。
/// </summary>
class SceneRenderer {
public:
	SceneRenderer() = default;
	~SceneRenderer() = default;
	SceneRenderer(const SceneRenderer&) = delete;
	const SceneRenderer& operator=(const SceneRenderer&) = delete;

	static SceneRenderer* GetInstance();

public:
	// 終了処理
	void Finalize();
	// 初期化処理
	void Init();
	// 更新処理
	void Update();
	// 後から行う更新
	void PostUpdate();
	// 描画処理
	void Draw() const;
	/// <summary>
	/// Cameraに依存しないShadow Mapを1フレームに1度描画する。
	/// </summary>
	void DrawShadowMap() const;
	/// <summary>
	/// 現在Renderへ適用されているCameraとRenderTargetへSceneを描画する。
	/// </summary>
	void DrawSceneObjects() const;
	// Particleなどの描画
	void PostDraw() const;
	// objectの編集
	void EditObject(const ImVec2& windowSize, const ImVec2& imagePos);

public:
	/// <summary>
	/// SceneLoaderのLevelDataからSceneWorldへオブジェクトを生成し、描画設定を登録します。
	/// </summary>
	void CreateObject(AOENGINE::SceneLoader::LevelData* loadData);

	/// <summary>
	/// ISceneObject派生オブジェクトをSceneWorldへ生成し、SceneRendererへ描画対象として登録します。
	/// </summary>
	template<typename T, typename... Args>
	T* AddObject(const std::string& objectName, const std::string& renderingName, int renderQueue = 0, bool isPostDraw = false, Args&&... args) {
		static_assert(std::is_base_of<AOENGINE::ISceneObject, T>::value, "T must derive from ISceneObject");

		T* gameObject = sceneWorld_.CreateObject<T>(objectName, std::forward<Args>(args)...);
		if (!gameObject) {
			return nullptr;
		}

		AddRenderEntry(gameObject->GetHandle(), renderingName, renderQueue, isPostDraw);
		return gameObject;
	}

	/// <summary>
	/// Objectを解放します。子階層もSceneWorld側でまとめて破棄されます。
	/// </summary>
	void ReleaseObject(AOENGINE::ISceneObject* objPtr);

	/// <summary>
	/// 指定オブジェクトの描画Pipelineを変更します。
	/// </summary>
	void ChangeRenderingType(const std::string& renderingName, AOENGINE::ISceneObject* gameObject);

public:
	template<typename T>
	T* GetGameObject(const std::string& _objName) {
		static_assert(std::is_base_of<AOENGINE::ISceneObject, T>::value, "T must derive from ISceneObject");

		for (const ObjectHandle& handle : sceneWorld_.GetObjectHandles()) {
			AOENGINE::ISceneObject* object = sceneWorld_.FindObjectAs<AOENGINE::ISceneObject>(handle);
			if (object && object->GetName() == _objName) {
				return dynamic_cast<T*>(object);
			}
		}

		AOENGINE::Logger::Log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		AOENGINE::Logger::Log("[" + _objName + "] not find");
		AOENGINE::Logger::Log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		return nullptr;
	}

	template<typename T>
	T* GetGameObject(T* _ptr) {
		for (const ObjectHandle& handle : sceneWorld_.GetObjectHandles()) {
			AOENGINE::ISceneObject* object = sceneWorld_.FindObjectAs<AOENGINE::ISceneObject>(handle);
			if (object == _ptr) {
				return dynamic_cast<T*>(object);
			}
		}
		return nullptr;
	}

	void SetRenderingQueue(const std::string& objName, int num);

	std::vector<ObjectHandle> GetObjectHandles() const;
	std::vector<ObjectHandle> GetRootObjectHandles() const;

	AOENGINE::SceneObject* FindObject(const ObjectHandle& handle);
	const AOENGINE::SceneObject* FindObject(const ObjectHandle& handle) const;

	bool SetParent(const ObjectHandle& child, const ObjectHandle& parent);
	bool AddChild(const ObjectHandle& parent, const ObjectHandle& child);
	bool RemoveChild(const ObjectHandle& parent, const ObjectHandle& child);
	bool MoveToRoot(const ObjectHandle& handle);
	void DestroyObject(const ObjectHandle& handle);

	SceneWorld& GetSceneWorld() { return sceneWorld_; }
	const SceneWorld& GetSceneWorld() const { return sceneWorld_; }

private:
	struct RenderEntry {
		ObjectHandle handle;
		std::string renderingType;
		int renderQueue = 0;
		bool isPostDraw = false;
	};

	void AddRenderEntry(const ObjectHandle& handle, const std::string& renderingName, int renderQueue, bool isPostDraw);
	void RemoveRenderEntry(const ObjectHandle& handle);
	void RemoveInvalidRenderEntries();

	AOENGINE::ISceneObject* GetRenderableObject(const RenderEntry& entry);
	const AOENGINE::ISceneObject* GetRenderableObject(const RenderEntry& entry) const;

	/// <summary>
	/// Frustum Cullingの対象なら視錐台との交差判定を行います。
	/// 対象外のSceneObjectは常に表示扱いにします。
	/// </summary>
	bool IsVisible(const AOENGINE::ISceneObject& object, const ::Math::Frustum& frustum) const;

	/// <summary>
	/// 通常3DモデルをInstancing batchへ追加できる場合だけ追加します。
	/// 追加できない場合はfalseを返し、呼び出し側で従来の個別描画へ戻します。
	/// </summary>
	bool TryAddNormalInstancingBatch(
		const RenderEntry& entry,
		const AOENGINE::BaseGameObject& object,
		std::vector<AOENGINE::ModelInstancingRenderer::NormalBatch>& batches) const;

	ObjectHandle CreateObjectRecursive(const AOENGINE::SceneLoader::Objects& data, const ObjectHandle& parent);
	void RegisterLightObjects();

private:
	SceneWorld sceneWorld_;
	std::vector<RenderEntry> renderEntries_;

	AOENGINE::ParticleManager* particleManager_ = nullptr;
	AOENGINE::GpuParticleManager* gpuParticleManager_ = nullptr;
	mutable AOENGINE::ModelInstancingRenderer modelInstancingRenderer_;
};
}
