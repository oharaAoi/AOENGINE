#include "BaseGameObject.h"
#include "Engine/Module/Components/Collider/SphereCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/Collider/LineCollider.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Render/Render.h"

#include <cmath>

using namespace AOENGINE;

namespace {
std::unique_ptr<BaseMaterial> CreateMaterialInstance(MaterialType type) {
	if (type == MaterialType::PBR) {
		return std::make_unique<PBRMaterial>();
	}
	return std::make_unique<Material>();
}
}

BaseGameObject::~BaseGameObject() {
	Finalize();
}

void BaseGameObject::Finalize() {
	if (transform_ != nullptr) {
		transform_->Finalize();
		transform_ = nullptr;
	}
	if (animetor_ != nullptr) {
		animetor_->Finalize();
		animetor_ = nullptr;
	}
	if (!colliders_.empty()) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			ColliderCollector::GetInstance()->RemoveCollider(colliders_[index]);
			colliders_[index] = nullptr;
		}
	}
	materials.clear();
	materialSlots_.clear();
	renderMaterialSlots_.clear();

	components_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Init() {
	transform_ = AddComponent<WorldTransform>();
	transform_->Init();
	animetor_ = nullptr;

	isDestroy_ = false;
	isRendering_ = true;
	isReflection_ = false;

	transform_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Update() {
	transform_->PostUpdate();
	// アニメーションが設定されていない場合はTransformのみ更新
	if (animetor_ == nullptr) {
		UpdateMatrix();
		return;
	}

	animetor_->Update();

	if (animetor_->GetIsSkinning()) {
		Engine::SetPipelineCS("Skinning.json");
		for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
			Engine::SetSkinning(animetor_->GetSkinning(index));
		}
	} else {
		UpdateMatrix();
		transform_->Update(animetor_->GetAnimationMat());
		return;
	}

	UpdateMatrix();
}

void BaseGameObject::UpdateMatrix() {
	// 移動量や追加する
	if (rigidbody_ != nullptr) {
		transform_->Translate(rigidbody_->GetMoveForce());
		rigidbody_->Update();
		transform_->Translate(rigidbody_->GetVelocity(), AOENGINE::GameTimer::DeltaTime());
	}

	// transformの更新
	transform_->Update();

	// colliderの更新
	if (!colliders_.empty()) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			colliders_[index]->Update(Math::QuaternionSRT{
				.scale = transform_->GetScale(),
				.rotate = transform_->GetWorldRotate(),
				.translate = transform_->GetWorldPos() }
			);
		}
	}

	worldPos_ = transform_->GetWorldMatrix().GetPosition();

	for (auto& material : materialSlots_) {
		if (material) { material->Update(); }
	}
}

void BaseGameObject::PostUpdate() {
	// colliderから押し戻し量を取得する
	if (rigidbody_ != nullptr) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			if (!colliders_[index]->GetIsStatic()) {
				rigidbody_->SetPushbackForce(colliders_[index]->GetPushBackDirection());
			}
		}
		transform_->Translate(rigidbody_->GetPushbackForce());
	}

	// 押し戻し後の更新を行なう
	transform_->Update();
	worldPos_ = transform_->GetWorldMatrix().GetPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　影の描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::PreDraw() const {
	if (!isRendering_) { return; }
	if (!enableShadow_) { return; }
	if (model_ == nullptr) { return; }
	Engine::SetPipeline(PSOType::Object3d, "Object_ShadowMap.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
		if (animetor_ == nullptr || !animetor_->GetIsSkinning()) {
			AOENGINE::Render::SetShadowMesh(pso, model_->GetMesh(index), transform_, model_->GetMesh(index)->GetVBV());
		} else {
			AOENGINE::Render::SetShadowMesh(pso, model_->GetMesh(index), transform_, animetor_->GetSkinning(index)->GetVBV());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Draw() const {
	// objectが設定されていなかったら描画処理は呼ばない
	if (model_ == nullptr) {
		return;
	}

	if (!isRendering_) {
		return;
	}

	// 鏡面反射をする場合の描画
	if (isReflection_) {
		for (uint32_t index = 0; index < model_->GetMeshsNum(); index++) {
			Pipeline* pso = Engine::GetLastUsedPipeline();
			Mesh* pMesh = model_->GetMesh(index);
			for (uint32_t subMeshIndex = 0; subMeshIndex < pMesh->GetSubMeshCount(); ++subMeshIndex) {
				const SubMesh& subMesh = pMesh->GetSubMesh(subMeshIndex);
				BaseMaterial* material = GetMaterial(subMesh.materialSlot);
				if (material) {
					AOENGINE::Render::DrawEnvironmentModel(pso, pMesh, subMesh, material, transform_);
				}
			}
		}
		return;
	}

	Pipeline* pso = Engine::GetLastUsedPipeline();
	if (animetor_ == nullptr || !animetor_->GetIsSkinning()) {
		AOENGINE::Render::DrawModel(pso, model_, transform_, renderMaterialSlots_);
	} else {
		AOENGINE::Render::DrawModel(pso, model_, transform_, animetor_->GetSkinningArray(), renderMaterialSlots_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Colliderを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

BaseCollider* BaseGameObject::GetCollider(const std::string& name) {
	for (uint32_t index = 0; index < colliders_.size(); ++index) {
		if (colliders_[index]->GetCategoryName() == name) {
			return colliders_[index];
		}
	}
	return nullptr;
}

BaseCollider* BaseGameObject::SetCollider(const std::string& categoryName, ColliderShape shape) {
	if (shape == ColliderShape::Sphere) {
		BaseCollider* collider = AddComponent<SphereCollider>();
		AddCollider(collider, categoryName, shape);
		colliders_.push_back(collider);
		return collider;
	} else if (shape == ColliderShape::AABB || shape == ColliderShape::OBB) {
		BaseCollider* collider = AddComponent<BoxCollider>();
		AddCollider(collider, categoryName, shape);
		colliders_.push_back(collider);
		return collider;
	} else if (shape == ColliderShape::Line) {
		BaseCollider* collider = AddComponent<LineCollider>();
		AddCollider(collider, categoryName, shape);
		colliders_.push_back(collider);
		return collider;
	}
	return nullptr;
}

void BaseGameObject::AddCollider(BaseCollider* _collider, const std::string& categoryName, ColliderShape shape) {
	_collider->Init(categoryName, shape);
	_collider->SetName(categoryName);
	_collider->SetCategory(categoryName);
	ColliderCollector::AddCollider(_collider);

	_collider->Update(Math::QuaternionSRT{ .scale = transform_->GetScale(),
						  .rotate = transform_->GetRotate(),
						  .translate = transform_->GetTranslate() }
	);
}

void BaseGameObject::SetPhysics() {
	if (rigidbody_ != nullptr) {
		return;
	}
	rigidbody_ = AddComponent<Rigidbody>();
}

void BaseGameObject::EditorUpdate() {
	if (animetor_) {
		animetor_->EvaluateCurrentPose();
		if (animetor_->GetIsSkinning() && model_) {
			Engine::SetPipelineCS("Skinning.json");
			for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
				Engine::SetSkinning(animetor_->GetSkinning(index));
			}
		}
	}

	if (animetor_ && !animetor_->GetIsSkinning()) {
		transform_->Update(animetor_->GetAnimationMat());
	} else {
		transform_->Update();
	}

	for (BaseCollider* collider : colliders_) {
		if (collider) {
			collider->Update(Math::QuaternionSRT{
				.scale = transform_->GetScale(),
				.rotate = transform_->GetWorldRotate(),
				.translate = transform_->GetWorldPos() });
		}
	}
	for (auto& material : materialSlots_) {
		if (material) { material->Update(); }
	}
	worldPos_ = transform_->GetWorldMatrix().GetPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　modelを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetObject(const std::string& _objName, MaterialType _type) {
	model_ = nullptr;
	materials.clear();
	materialSlots_.clear();
	renderMaterialSlots_.clear();

	model_ = ModelManager::GetInstance()->GetModel(_objName);
	materialSlots_.resize(model_->GetMaterialSlotCount());
	for (uint32_t slot = 0; slot < model_->GetMaterialSlotCount(); ++slot) {
		auto material = CreateMaterialInstance(_type);
		material->Init();
		material->SetMaterialData(model_->GetMaterialData().at(model_->GetMaterialSlotName(slot)));
		materialSlots_[slot] = std::move(material);
	}
	RebuildMaterialSlots();
}

void AOENGINE::BaseGameObject::SetMaterial(MaterialType _type) {
	materialSlots_.resize(model_->GetMaterialSlotCount());
	for (uint32_t slot = 0; slot < model_->GetMaterialSlotCount(); ++slot) {
		auto material = CreateMaterialInstance(_type);
		material->Init();
		material->SetMaterialData(model_->GetMaterialData().at(model_->GetMaterialSlotName(slot)));
		materialSlots_[slot] = std::move(material);
	}
	RebuildMaterialSlots();
}

void BaseGameObject::RebuildMaterialSlots() {
	materials.clear();
	renderMaterialSlots_.assign(materialSlots_.size(), nullptr);
	if (!model_) {
		return;
	}

	for (uint32_t slot = 0; slot < materialSlots_.size(); ++slot) {
		renderMaterialSlots_[slot] = materialSlots_[slot].get();
		if (materialSlots_[slot]) {
			materials[model_->GetMaterialSlotName(slot)] = materialSlots_[slot].get();
		}
	}
}

bool BaseGameObject::SetMaterialSlot(uint32_t slot, std::unique_ptr<BaseMaterial> material) {
	if (!model_ || slot >= model_->GetMaterialSlotCount() || !material) {
		return false;
	}

	materialSlots_[slot] = std::move(material);
	RebuildMaterialSlots();
	return true;
}

bool BaseGameObject::SetMaterialSlotType(uint32_t slot, MaterialType type) {
	if (!model_ || slot >= model_->GetMaterialSlotCount()) {
		return false;
	}

	auto material = CreateMaterialInstance(type);
	material->Init();
	material->SetMaterialData(model_->GetMaterialData().at(model_->GetMaterialSlotName(slot)));
	return SetMaterialSlot(slot, std::move(material));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　親を設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetParent(BaseGameObject* parent) {
	pParentObj_ = parent;
	transform_->SetParent(parent->GetTransform()->GetWorldMatrix());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　アニメーションを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetAnimator() {
	if (model_ == nullptr || animetor_ != nullptr) {
		return;
	}

	const std::string modelName = model_->GetName();
	SetAnimator(ModelManager::GetInstance()->GetModelPath(modelName), modelName);
}

void BaseGameObject::SetAnimator(const std::string& directoryPath, const std::string& objName) {
	if (model_ == nullptr) {
		return;
	}

	animetor_ = std::make_unique<Animator>();
	animetor_->LoadAnimation(directoryPath, objName, model_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialの色を変える
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetColor(const Color& color) {
	for (auto& material : materialSlots_) {
		if (material) { material->SetColor(color); }
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　lightingの設定を変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetIsLighting(bool isLighting) {
	for (auto& material : materialSlots_) {
		if (material) { material->SetIsLighting(isLighting); }
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialのtextureを変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetTexture(const std::string& path) {
	for (auto& material : materialSlots_) {
		if (material) { material->SetAlbedoTexture(path); }
	}
}

void BaseGameObject::SetShaderGraph(ShaderGraph* _shaderGraph) {
	for (auto& material : materialSlots_) {
		if (material) { material->SetShaderGraph(_shaderGraph); }
	}
}

void BaseGameObject::Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) {
	transform_->Manipulate(windowSize, imagePos);
}

bool BaseGameObject::IsFrustumCullingEnabled() const {
	return model_ != nullptr && transform_ != nullptr;
}

Math::Sphere BaseGameObject::GetWorldBoundingSphere() const {
	if (model_ == nullptr || transform_ == nullptr) {
		return Math::Sphere{ .center = CVector3::ZERO, .radius = 0.0f };
	}

	const Math::Sphere& localSphere = model_->GetLocalBoundingSphere();
	const Math::Matrix4x4& worldMatrix = transform_->GetWorldMatrix();

	// 非等倍Scaleでも球がモデル全体を含むように、最大Scale成分で半径を拡大します。
	const Math::Vector3 worldScale = worldMatrix.GetScale();
	const float maxScale = std::fmax(std::fmax(std::fabs(worldScale.x), std::fabs(worldScale.y)), std::fabs(worldScale.z));

	return Math::Sphere{
		.center = TransformCoord(localSphere.center, worldMatrix),
		.radius = localSphere.radius * maxScale
	};
}

bool BaseGameObject::CanUseNormalInstancing() const {
	// Reflection描画や非表示Objectは個別の描画条件があるためInstancing対象外にします。
	if (model_ == nullptr || transform_ == nullptr || !isRendering_ || isReflection_) {
		return false;
	}

	// Skinning済み頂点はMeshごとにVBVが変わるため、現在の通常Instancingでは扱いません。
	if (animetor_ != nullptr && animetor_->GetIsSkinning()) {
		return false;
	}
	return true;
}

bool BaseGameObject::CanUseSkinnedMaterialBatch() const {
	if (model_ == nullptr || transform_ == nullptr || !isRendering_ || isReflection_ || animetor_ == nullptr) {
		return false;
	}
	return animetor_->GetIsSkinning();
}
