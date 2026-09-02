#pragma once

#include <string>

#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/GameObject/SceneObject.h"

namespace AOENGINE {

class BaseCollider;
class BaseGameObject;
class WorldTransform;

/// <summary>
/// ゲームロジックとSceneWorld上のBaseGameObjectを関連付ける非所有ラッパー。
/// BaseGameObjectの寿命はSceneWorldが管理し、BaseEntityはObjectHandleだけを保持する。
/// </summary>
class BaseEntity {
public:
	BaseEntity() = default;
	explicit BaseEntity(BaseGameObject* object);
	virtual ~BaseEntity() = default;

	/// <summary>SceneWorldに登録済みのGameObjectと関連付ける。</summary>
	void Bind(BaseGameObject* object);
	void Bind(const ObjectHandle& handle);
	void Unbind();

	/// <summary>関連付けたGameObjectが現在もSceneWorldに存在するか。</summary>
	bool IsValid() const;
	const ObjectHandle& GetObjectHandle() const { return objectHandle_; }

	BaseGameObject* GetGameObject() const;
	WorldTransform* GetTransform() const;
	BaseCollider* GetCollider(const std::string& tag) const;

	void SetIsActive(bool isActive);
	bool GetIsActive() const;
	void SetRendering(bool isRendering);
	Math::Vector3 GetPosition() const;

	/// <summary>関連付けたGameObjectと子階層をSceneWorldから削除し、関連付けを解除する。</summary>
	void Destroy();

	/// <summary>このEntityのGameObjectをparentのGameObjectの子にする。</summary>
	bool SetParent(const BaseEntity* parent);
	/// <summary>親子関係を解除してシーン直下へ移動する。</summary>
	bool MoveToRoot();

protected:
	ObjectHandle objectHandle_{};
};

}
