#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Collider/ICollider.h"
#include "Engine/Module/Components/WorldTransform.h"

/// <summary>
/// GameObjectやTransformの仲介
/// </summary>
class BaseEntity :
	public AttributeGui {
public:

	BaseEntity() = default;
	virtual ~BaseEntity() = default;

	virtual void Debug_Gui() override = 0;

public:	// accessor method

	void SetIsActive(bool _isActive) { isActive_ = _isActive; }
	bool GetIsActive() const { return isActive_; }

	void SetRendering(bool _isRendering) { object_->SetIsRendering(_isRendering); }

	void Destroy() { object_->SetIsDestroy(true); };

	Vector3 GetPosition() { return object_->GetPosition(); }

	BaseGameObject* GetGameObject() const { return object_; }

	WorldTransform* GetTransform() const { return transform_; }

	ICollider* GetCollider(const std::string& tag) const { return object_->GetCollider(tag); }
	ICollider* GetCollider() const { return object_->GetCollider(); }

	void SetParent(BaseEntity* _parent) { object_->SetParent(_parent->GetGameObject()); }

protected:

	BaseGameObject* object_;
	WorldTransform* transform_;

};

