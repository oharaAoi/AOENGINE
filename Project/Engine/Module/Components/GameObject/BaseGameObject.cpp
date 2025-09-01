#include "BaseGameObject.h"
#include "Engine/Module/Components/Collider/SphereCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/Render/SceneRenderer.h"

BaseGameObject::~BaseGameObject() {
	Finalize();
}

void BaseGameObject::Finalize() {
	if (transform_ != nullptr) {
		transform_->Finalize();
	}
	if (animetor_ != nullptr) {
		animetor_->Finalize();
	}
	if (!colliders_.empty()) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			ColliderCollector::GetInstance()->RemoveCollider(colliders_[index].get());
			colliders_[index].reset();
			colliders_[index] = nullptr;
		}
	}
	materials.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Init() {
	transform_ = Engine::CreateWorldTransform();
	animetor_ = nullptr;

	isDestroy_ = false;
	isRendering_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Update() {
	// アニメーションが設定されていない場合はTransformのみ更新
	if (animetor_ == nullptr) {
		UpdateMatrix();
		return;
	}

	// アニメーションの制御をスクリプトで行っていない場合
	if (!animetor_->GetIsControlScript()) {
		animetor_->Update();

		/*for (auto& effector : endEffectors_) {
			effector.second->Update(transform_->GetWorldMatrix());
		}
		animetor_->UpdateSkinning();*/

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
	} else { 
		if (animetor_->GetIsSkinning()) {
			Engine::SetPipelineCS("Skinning.json");
			for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
				Engine::SetSkinning(animetor_->GetSkinning(index));
			}
		}
	}

	UpdateMatrix();
}

void BaseGameObject::UpdateMatrix() {
	if (rigidbody_ != nullptr) {
		rigidbody_->Update();
		transform_->Translate(rigidbody_->GetMoveForce());
	}

	transform_->Update();

	if (!colliders_.empty()) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			colliders_[index]->Update(QuaternionSRT{
				.scale = transform_->GetScale(),
				.rotate = transform_->GetRotate(),
				.translate = transform_->GetTranslate() }
			);
		}
	}

	worldPos_ = transform_->GetWorldMatrix().GetPosition();

	for (auto& material : materials) {
		material.second->Update();
	}
}

void BaseGameObject::PostUpdate() {
	if (rigidbody_ != nullptr) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			if (!colliders_[index]->GetIsStatic()) {
				rigidbody_->SetPushbackForce(colliders_[index]->GetPushBackDirection());
			}
		}
		transform_->Translate(rigidbody_->GetPushbackForce());
	}
	transform_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::PreDraw() const {
	if (!isShadow_) { return; }
	if (model_ == nullptr) { return; }
	Engine::SetPipeline(PSOType::Object3d, "Object_ShadowMap.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
		if (animetor_ == nullptr || !animetor_->GetIsSkinning()) {
			Render::SetShadowMesh(pso, model_->GetMesh(index), transform_.get(), model_->GetMesh(index)->GetVBV());
		} else {
			Render::SetShadowMesh(pso, model_->GetMesh(index), transform_.get(), animetor_->GetSkinning(index)->GetVBV());
		}
	}
}

void BaseGameObject::Draw() const {
	if (!isRendering_) {
		return;
	}

	if (isReflection_) {
		for (uint32_t index = 0; index < model_->GetMeshsNum(); index++) {
			if (materials.size() > index) {
				Pipeline* pso = Engine::GetLastUsedPipeline();
				Mesh* pMesh = model_->GetMesh(index);
				Render::DrawEnvironmentModel(pso, pMesh, materials.at(pMesh->GetUseMaterial()).get(), transform_.get());
			}
		}
		return;
	}

	Pipeline* pso = Engine::GetLastUsedPipeline();
	if (animetor_ == nullptr || !animetor_->GetIsSkinning()) {
		Render::DrawModel(pso, model_, transform_.get(), materials);
	} else {
		for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
			Render::DrawModel(pso, model_->GetMesh(index), transform_.get(), animetor_->GetSkinning(index)->GetVBV(), materials);
		}
	}

	/*if (animetor_ != nullptr) {
		animetor_->GetSkeleton()->DrawNodeHierarchy(transform_->GetWorldMatrix());
	}*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Colliderを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

ICollider* BaseGameObject::GetCollider(const std::string& name) {
	for (uint32_t index = 0; index < colliders_.size(); ++index) {
		if (colliders_[index]->GetCategoryName() == name) {
			return colliders_[index].get();
		}
	}
	return nullptr;
}

ICollider* BaseGameObject::GetCollider() {
	if (!colliders_.empty()) {
		return colliders_[0].get();
	}
	return nullptr;
}

void BaseGameObject::SetCollider(const std::string& categoryName, ColliderShape shape) {
	if (shape == ColliderShape::SPHERE) {
		auto& newCollider = colliders_.emplace_back(std::make_unique<SphereCollider>());
		AddCollider(newCollider.get(), categoryName, shape);
	} else if (shape == ColliderShape::AABB || shape == ColliderShape::OBB) {
		auto& newCollider = colliders_.emplace_back(std::make_unique<BoxCollider>());
		AddCollider(newCollider.get(), categoryName, shape);
	}
}

void BaseGameObject::AddCollider(ICollider* _collider, const std::string& categoryName, ColliderShape shape) {
	_collider->Init(categoryName, shape);
	_collider->SetName(categoryName);
	_collider->SetCategory(categoryName);
	ColliderCollector::AddCollider(_collider);

	_collider->Update(QuaternionSRT{ .scale = transform_->GetScale(),
						  .rotate = transform_->GetRotate(),
						  .translate = transform_->GetTranslate() }
	);
}

void BaseGameObject::SetCollider(const std::string& categoryName, const std::string& shapeName) {
	ColliderShape shape = ColliderShape::SPHERE;

	if (shapeName == "SPHERE") {
		auto& newCollider = colliders_.emplace_back(std::make_unique<SphereCollider>());
		shape = ColliderShape::SPHERE;
		AddCollider(newCollider.get(), categoryName, shape);
	} else if (shapeName == "BOX") {
		auto& newCollider = colliders_.emplace_back(std::make_unique<BoxCollider>());
		shape = ColliderShape::AABB;
		AddCollider(newCollider.get(), categoryName, shape);
	}
}

void BaseGameObject::SetPhysics() {
	rigidbody_ = std::make_unique<Rigidbody>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　modelを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetObject(const std::string& _objName, MaterialType _type) {
	model_ = nullptr;
	materials.clear();

	model_ = ModelManager::GetModel(_objName);
	for (const auto& material : model_->GetMaterialData()) {
		if (_type == MaterialType::NORMAL) {
			materials[material.first] = std::make_unique<Material>();
		} else if (_type == MaterialType::PBR) {
			materials[material.first] = std::make_unique<PBRMaterial>();
		}
		materials[material.first]->Init();
		materials[material.first]->SetMaterialData(material.second);
	}
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

void BaseGameObject::SetAnimater(const std::string& directoryPath, const std::string& objName, bool isSkinning, bool isLoop, bool isControlScript) {
	animetor_.reset(new Animator);
	animetor_->LoadAnimation(directoryPath, objName, model_, isSkinning, isLoop, isControlScript);
}

void BaseGameObject::SetEndEffector(const std::string& _name, EndEffector* _effector) {
	_effector->SetSkelton(animetor_->GetSkeleton());
	endEffectors_[_name] = _effector;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialの色を変える
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetColor(const Vector4& color) {
	for (auto& material : materials) {
		material.second->SetColor(color);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　lightingの設定を変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetIsLighting(bool isLighting) {
	for (auto& material : materials) {
		material.second->SetIsLighting(isLighting);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialのtextureを変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetTexture(const std::string& path) {
	for (auto& material : materials) {
		material.second->SetAlbedoTexture(path);
	}
}

void BaseGameObject::Debug_Gui() {
	transform_->Debug_Gui();
	int index = 0;
	if (ImGui::CollapsingHeader("Material")) {
		for (auto& material : materials) {
			std::string guiId = "material_" + std::to_string(index);
			if (ImGui::TreeNode(guiId.c_str())) {
				material.second->Debug_Gui();
				ImGui::TreePop();
			}
		}
	}

	if (!colliders_.empty()) {
		for (uint32_t oi = 0; oi < colliders_.size(); ++oi) {
			std::string tag = colliders_[oi]->GetCategoryName();
			if (ImGui::TreeNode(tag.c_str())) {
				colliders_[oi]->Debug_Gui();
				ImGui::TreePop();
			}

		}
	}

	if (animetor_ != nullptr) {
		if (ImGui::CollapsingHeader("Animator")) {
			animetor_->Debug_Gui();
		}
	}
}

void BaseGameObject::Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) {
	transform_->Manipulate(windowSize, imagePos);
}