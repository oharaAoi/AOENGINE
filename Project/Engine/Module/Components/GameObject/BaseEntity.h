#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/WorldTransform.h"

/// <summary>
/// GameObjectやTransformの仲介
/// </summary>
class BaseEntity :
	public AOENGINE::AttributeGui {
public:

	BaseEntity() = default;
	virtual ~BaseEntity() = default;

	virtual void Debug_Gui() override = 0;

public:	// accessor method

	void SetIsActive(bool _isActive) { isActive_ = _isActive; }
	bool GetIsActive() const { return isActive_; }

	void SetRendering(bool _isRendering) { object_->SetIsRendering(_isRendering); }

	void Destroy() { object_->SetIsDestroy(true); };

	Math::Vector3 GetPosition() { return object_->GetPosition(); }

	BaseGameObject* GetGameObject() const { return object_; }

	WorldTransform* GetTransform() const { return transform_; }

	AOENGINE::BaseCollider* GetCollider(const std::string& tag) const { return object_->GetCollider(tag); }
	AOENGINE::BaseCollider* GetCollider() const { return object_->GetCollider(); }

	void SetParent(BaseEntity* _parent) { object_->SetParent(_parent->GetGameObject()); }

protected:

	BaseGameObject* object_;
	WorldTransform* transform_;

};

