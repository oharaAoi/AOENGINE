#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Engine/Engine.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/ObjectAxis.h"
#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

#include "Engine/Module/Components/Collider/ICollider.h"
#include "Engine/Module/Components/Collider/SphereCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"

class BaseGameObject :
	public AttributeGui {
public:

	BaseGameObject() = default;
	virtual ~BaseGameObject() ;

	virtual void Finalize();
	virtual void Init();
	virtual void Update();
	virtual void Draw() const;

	void PostUpdate();

#ifdef _DEBUG
	void Debug_Draw();

	void Debug_Gui() override;

	void Debug_Axis();

	void SetObjectAxis(bool isAxis = true);
#endif // _DEBUG

	void SetObject(const std::string& objName);

	void SetParent(BaseGameObject* parent);

	Model* GetModel() { return model_; }

	WorldTransform* GetTransform() { return transform_.get(); }

	const Vector3& GetPosition() const { return worldPos_; }

	void SetIsActive(bool isActive) { isActive_ = isActive; }
	bool GetIsActive() const { return isActive_; }

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; }
	bool GetIsDestroy() const { return isDestroy_; }

	// -------------------------------------------------
	// ↓ Material関連
	// -------------------------------------------------

	const Vector4& GetColor()const { return color_; }
	void SetColor(const Vector4& color);

	void SetIsLighting(bool isLighting);

	void SetTexture(const std::string& path);

	// -------------------------------------------------
	// ↓ Animation関連
	// -------------------------------------------------

	void SetAnimater(const std::string& directoryPath, const std::string& objName, bool isSkinning, bool isLoop, bool isControlScript);

	Animator* GetAnimetor() { return animetor_.get(); }

	// -------------------------------------------------
	// ↓ Collider関連
	// -------------------------------------------------

	ICollider* GetCollider() { return collider_.get(); }
	void SetCollider(const std::string& categoryName, ColliderShape shape);

	void SetCollider(const std::string& categoryName, const std::string& shapeName);

	void SetIsReflection(bool isReflection) { isReflection_ = isReflection; }
		
private:

	void SetColliderRadius(float radius) {
		if (auto sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
			sphere->SetRadius(radius);
		}
	}

	void SetColliderSize(const Vector3& size) {
		if (auto box = dynamic_cast<BoxCollider*>(collider_.get())) {
			box->SetSize(size);
		}
	}

protected:

	bool isDestroy_;
	bool isActive_;

	Model* model_ = nullptr;
	std::vector<std::unique_ptr<Material>> materials;	// 後で変えたい

	std::unique_ptr<WorldTransform> transform_ = nullptr;
	std::unique_ptr<Animator> animetor_ = nullptr;

	std::unique_ptr<ICollider> collider_ = nullptr;

	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	Vector3 worldPos_ = { 1.0f, 1.0f, 1.0f};

	bool isAnimation_ = false;

	bool isReflection_;

	// 他クラス情報
	BaseGameObject* pParentObj_;

#ifdef _DEBUG
	bool isDebugAxis_;

	std::unique_ptr<ObjectAxis> objectAxis_ = nullptr; // objectの回転を可視化したもの
#endif // _DEBUG
};