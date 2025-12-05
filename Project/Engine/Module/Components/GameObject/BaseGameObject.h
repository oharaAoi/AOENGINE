#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Engine/Engine.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/Rigging/EndEffector.h"

#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"

/// <summary>
/// MeshやMaterialを持ったGameObject
/// </summary>
class BaseGameObject :
	public ISceneObject {
public: // コンストラクタ

	BaseGameObject() = default;
	virtual ~BaseGameObject() ;

public:

	// 終了処理
	void Finalize();
	// 初期化処理
	void Init() override;
	// 更新処理
	void Update() override;
	// 影の描画
	void PreDraw() const override;
	// 描画処理
	void Draw() const override;
	// 行列の更新
	void UpdateMatrix();
	// 後から更新する処理
	void PostUpdate() override;

public:

	// 編集処理
	void Debug_Gui() override;
	// Gizumo表示
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);

public:

	void SetObject(const std::string& _objName, MaterialType _type = MaterialType::Normal);

	void SetParent(BaseGameObject* parent);

	Model* GetModel() { return model_; }

	WorldTransform* GetTransform() { return transform_.get(); }

	Vector3 GetPosition() const { return worldPos_ + offset_; }

	void SetEnableShadow(bool _flag) { enableShadow_ = _flag; }

	void SetOffset(const Vector3& _offset) { offset_ = _offset; }

	// -------------------------------------------------
	// ↓ Material関連
	// -------------------------------------------------

	const Color& GetColor()const { return color_; }
	void SetColor(const Color& color);

	void SetIsLighting(bool isLighting);

	void SetTexture(const std::string& path);

	void SetShaderGraph(ShaderGraph* _shaderGraph);

	const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& GetMaterials() const { return materials; }

	// -------------------------------------------------
	// ↓ Animation関連
	// -------------------------------------------------

	void SetAnimator(const std::string& directoryPath, const std::string& objName, bool isSkinning, bool isLoop, bool isControlScript);

	Animator* GetAnimetor() { return animetor_.get(); }

	void SetEndEffector(const std::string& _name, EndEffector* _effector);

	// -------------------------------------------------
	// ↓ Collider関連
	// -------------------------------------------------

	BaseCollider* GetCollider(const std::string& name);
	BaseCollider* GetCollider();
	BaseCollider* SetCollider(const std::string& categoryName, ColliderShape shape);
	void AddCollider(BaseCollider* _collider, const std::string& categoryName, ColliderShape shape);

	void SetCollider(const std::string& categoryName, const std::string& shapeName);

	void SetIsReflection(bool isReflection) { isReflection_ = isReflection; }

	void SetIsRendering(bool _isRendering) { isRendering_ = _isRendering; }

	void SetPhysics();
	Rigidbody* GetRigidbody() { return rigidbody_.get(); }

protected:

	Model* model_ = nullptr;
	std::unordered_map<std::string, std::unique_ptr<BaseMaterial>> materials;

	std::unique_ptr<WorldTransform> transform_ = nullptr;
	std::unique_ptr<Animator> animetor_ = nullptr;
	std::unordered_map<std::string, EndEffector*> endEffectors_;

	std::vector<std::unique_ptr<BaseCollider>> colliders_;

	std::unique_ptr<Rigidbody> rigidbody_ = nullptr;

	Color color_ = {1.0f, 1.0f, 1.0f, 1.0f};
	Vector3 worldPos_ = { 1.0f, 1.0f, 1.0f};
	Vector3 offset_ = CVector3::ZERO;

	bool isAnimation_ = false;

	bool isReflection_;
	bool isRendering_;

	// 他クラス情報
	BaseGameObject* pParentObj_;

	bool enableShadow_ = true;
};