#pragma once
// c++
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <algorithm>
#include <unordered_map>
// engine
#include "Engine/Core/Engine.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/Color.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/IComponent.h"

namespace AOENGINE {

/// <summary>
/// MeshやMaterialを持ったGameObject
/// </summary>
class BaseGameObject :
	public ISceneObject {
public: // constructor

	BaseGameObject() = default;
	virtual ~BaseGameObject();

public: // public method

	// 終了処理
	void Finalize();
	// 初期化処理
	void Init() override;
	// 更新処理
	void Update() override;
	/// <summary>Editor用に時間を進めず描画データだけ更新する。</summary>
	void EditorUpdate();
	// 影の描画
	void PreDraw() const override;
	// 描画処理
	void Draw() const override;
	// 行列の更新
	void UpdateMatrix();
	// 後から更新する処理
	void PostUpdate() override;
	// Gizumo表示
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);
	// Frustum Cullingを有効にするか
	bool IsFrustumCullingEnabled() const override;
	// ワールド空間の境界球を取得する
	Math::Sphere GetWorldBoundingSphere() const override;

	/// <summary>
	/// Object_NormalのInstancing描画に回せる状態かを返します。
	/// </summary>
	bool CanUseNormalInstancing() const;
	bool CanUseSkinnedMaterialBatch() const;
	ScenePersistence GetScenePersistence() const override { return ScenePersistence::SceneData; }
	const char* GetSceneTypeName() const override { return "BaseGameObject"; }

public: // accessor method

	template<class T>
	T* GetComponent() {
		for (auto& component : components_) {
			if (auto result = dynamic_cast<T*>(component.get())) {
				return result;
			}
		}
		return nullptr;
	}

	template<class T, class... Args>
	T* AddComponent(Args&&... args) {
		auto component = std::make_unique<T>(std::forward<Args>(args)...);
		T* ptr = component.get();

		components_.push_back(std::move(component));
		return ptr;
	}

	void SetObject(const std::string& _objName, MaterialType _type = MaterialType::Normal);

	void SetParent(BaseGameObject* parent);

	AOENGINE::Model* GetModel() { return model_; }
	const AOENGINE::Model* GetModel() const { return model_; }

	AOENGINE::WorldTransform* GetTransform() { return transform_; }
	const AOENGINE::WorldTransform* GetTransform() const { return transform_; }

	Math::Vector3 GetPosition() const { return worldPos_; }

	void SetEnableShadow(bool _flag) { enableShadow_ = _flag; }
	bool GetEnableShadow() const { return enableShadow_; }

	void SetIsReflection(bool isReflection) { isReflection_ = isReflection; }

	void SetIsRendering(bool _isRendering) { isRendering_ = _isRendering; }

	// -------------------------------------------------
	// ↓ Material関連
	// -------------------------------------------------

	void SetColor(const AOENGINE::Color& color);

	void SetIsLighting(bool isLighting);

	void SetTexture(const std::string& path);

	void SetShaderGraph(AOENGINE::ShaderGraph* _shaderGraph);

	void SetMaterial(MaterialType _type);
	/// <summary>SubMeshが参照するMaterial Slotをオブジェクト単位で差し替えます。</summary>
	bool SetMaterialSlot(uint32_t slot, std::unique_ptr<AOENGINE::BaseMaterial> material);
	/// <summary>Material Slotの種別を変更し、Modelの初期Material情報を設定します。</summary>
	bool SetMaterialSlotType(uint32_t slot, MaterialType type);

	const std::unordered_map<std::string, AOENGINE::BaseMaterial*>& GetMaterials() const { return materials; }
	const std::vector<AOENGINE::BaseMaterial*>& GetMaterialSlots() const { return renderMaterialSlots_; }
	AOENGINE::BaseMaterial* GetMaterial(uint32_t slot) const {
		return slot < materialSlots_.size() ? materialSlots_[slot].get() : nullptr;
	}

	// -------------------------------------------------
	// ↓ Animation関連
	// -------------------------------------------------

	/// <summary>
	/// 現在設定されているModelとその読み込み元からAnimatorを設定する。
	/// </summary>
	void SetAnimator();

	void SetAnimator(const std::string& directoryPath, const std::string& objName);

	AOENGINE::Animator* GetAnimator() { return animetor_.get(); }
	const AOENGINE::Animator* GetAnimator() const { return animetor_.get(); }

	// -------------------------------------------------
	// ↓ Collider関連
	// -------------------------------------------------

	AOENGINE::BaseCollider* GetCollider(const std::string& name);
	AOENGINE::BaseCollider* SetCollider(const std::string& categoryName, ColliderShape shape);
	void AddCollider(AOENGINE::BaseCollider* _collider, const std::string& categoryName, ColliderShape shape);

	const std::vector<AOENGINE::BaseCollider*>& GetColliders() const { return colliders_; }

	void SetPhysics();
	AOENGINE::Rigidbody* GetRigidbody() { return rigidbody_; }
	const AOENGINE::Rigidbody* GetRigidbody() const { return rigidbody_; }

protected:
	void RebuildMaterialSlots();

	// componts
	std::list<std::unique_ptr<IComponent>> components_;

	AOENGINE::Model* model_ = nullptr;
	// Material Slotごとのオブジェクト固有Material Instance。
	std::vector<std::unique_ptr<AOENGINE::BaseMaterial>> materialSlots_;
	// 描画APIへ渡す非所有の連続配列。
	std::vector<AOENGINE::BaseMaterial*> renderMaterialSlots_;
	// Inspector・旧API向けの名前検索。所有権はmaterialSlots_が持ちます。
	std::unordered_map<std::string, AOENGINE::BaseMaterial*> materials;

	std::unique_ptr<AOENGINE::Animator> animetor_ = nullptr;

	std::vector<AOENGINE::BaseCollider*> colliders_;
	AOENGINE::WorldTransform* transform_ = nullptr;
	AOENGINE::Rigidbody* rigidbody_ = nullptr;

	Math::Vector3 worldPos_ = { 1.0f, 1.0f, 1.0f };

	bool isAnimation_ = false;

	bool isReflection_;
	bool isRendering_;

	// 他クラス情報
	AOENGINE::BaseGameObject* pParentObj_;

	bool enableShadow_ = true;
};

}
