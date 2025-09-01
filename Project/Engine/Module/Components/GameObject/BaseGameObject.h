#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Engine/Engine.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/Rigging/EndEffector.h"

#include "Engine/Module/Components/Collider/ICollider.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"

class BaseGameObject :
	public ISceneObject {
public:

	BaseGameObject() = default;
	virtual ~BaseGameObject() ;

	void Finalize();
	void Init() override;
	void Update() override;
	void PreDraw() const override;
	void Draw() const override;

	void UpdateMatrix();

	void PostUpdate() override;

public:

	void Debug_Gui() override;

	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);

public:

	void SetObject(const std::string& _objName, MaterialType _type = MaterialType::NORMAL);

	void SetParent(BaseGameObject* parent);

	Model* GetModel() { return model_; }

	WorldTransform* GetTransform() { return transform_.get(); }

	const Vector3& GetPosition() const { return worldPos_; }

	void SetIsShadow(bool _flag) { isShadow_ = _flag; }

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

	void SetEndEffector(const std::string& _name, EndEffector* _effector);

	// -------------------------------------------------
	// ↓ Collider関連
	// -------------------------------------------------

	ICollider* GetCollider(const std::string& name);
	ICollider* GetCollider();
	void SetCollider(const std::string& categoryName, ColliderShape shape);
	void AddCollider(ICollider* _collider, const std::string& categoryName, ColliderShape shape);

	void SetCollider(const std::string& categoryName, const std::string& shapeName);

	void SetIsReflection(bool isReflection) { isReflection_ = isReflection; }

	void SetIsRendering(bool _isRendering) { isRendering_ = _isRendering; }

	void SetPhysics();
	Rigidbody* GetRigidbody() { return rigidbody_.get(); }

protected:

	Model* model_ = nullptr;
	std::unordered_map<std::string, std::unique_ptr<BaseMaterial>> materials;	// 後で変えたい

	std::unique_ptr<WorldTransform> transform_ = nullptr;
	std::unique_ptr<Animator> animetor_ = nullptr;
	std::unordered_map<std::string, EndEffector*> endEffectors_;

	std::vector<std::unique_ptr<ICollider>> colliders_;

	std::unique_ptr<Rigidbody> rigidbody_ = nullptr;

	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	Vector3 worldPos_ = { 1.0f, 1.0f, 1.0f};

	bool isAnimation_ = false;

	bool isReflection_;
	bool isRendering_;

	// 他クラス情報
	BaseGameObject* pParentObj_;

	bool isShadow_ = true;
};