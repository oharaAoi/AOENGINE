#include "BaseGameObject.h"
#include "Engine/Module/Components/Collider/SphereCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
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
	if (collider_ != nullptr) {
		ColliderCollector::GetInstance()->RemoveCollider(collider_.get());
		collider_.reset();
		collider_ = nullptr;
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

		if (animetor_->GetIsSkinning()) {
			Engine::SetSkinning(animetor_->GetSkinning(), model_->GetMesh(0));
		} else {
			UpdateMatrix();
			transform_->Update(animetor_->GetAnimationMat());
			return;
		}
	} else {
		if (animetor_->GetIsSkinning()) {
			Engine::SetSkinning(animetor_->GetSkinning(), model_->GetMesh(0));
		}
	}

	UpdateMatrix();
}

void BaseGameObject::UpdateMatrix() {
	transform_->Update();
#ifdef _DEBUG
	// Debug軸の更新
	if (objectAxis_ != nullptr) {
		objectAxis_->Update(transform_->GetWorldMatrix());
	}
#endif

	if (rigidbody_ != nullptr) {
		rigidbody_->Update();
		transform_->Translate(rigidbody_->GetMoveForce());
	}

	if (collider_ != nullptr) {
		collider_->Update(QuaternionSRT{.scale = transform_->GetScale(),
						  .rotate = transform_->GetQuaternion(),
						  .translate = transform_->GetTranslation()}
		);
	}

	worldPos_ = transform_->GetWorldMatrix().GetPosition();

	for (auto& material : materials) {
		material->Update();
	}
}

void BaseGameObject::PostUpdate() {
	if (rigidbody_ != nullptr) {
		rigidbody_->SetPushbackForce(collider_->GetPushBackDirection());
		transform_->Translate(rigidbody_->GetPushbackForce());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Draw() const {
	if (isReflection_) {
		for (uint32_t index = 0; index < model_->GetMeshsNum(); index++) {
			if (materials.size() > index) {
				Pipeline* pso = Engine::GetLastUsedPipeline();
				Render::DrawEnvironmentModel(pso, model_->GetMesh(index), materials[index].get(), transform_.get());
			}
		}
		return;
	}

	Pipeline* pso = Engine::GetLastUsedPipeline();
	if (animetor_ == nullptr || !animetor_->GetIsSkinning()) {
		Render::DrawModel(pso, model_, transform_.get(), materials);
	} else {
		Render::DrawModel(pso, model_, transform_.get(), animetor_->GetSkinning()->GetVBV(), materials);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Colliderを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetCollider(const std::string& categoryName, ColliderShape shape) {
	if (shape == ColliderShape::SPHERE) {
		collider_ = std::make_unique<SphereCollider>();
	} else if (shape == ColliderShape::AABB || shape == ColliderShape::OBB) {
		collider_ = std::make_unique<BoxCollider>();
	}

	collider_->Init(categoryName, shape);
	collider_->SetName(categoryName);
	collider_->SetCategory(categoryName);
	ColliderCollector::AddCollider(collider_.get());

	collider_->Update(QuaternionSRT{ .scale = transform_->GetScale(), 
						  .rotate = transform_->GetQuaternion(),
						  .translate = transform_->GetTranslation() }
	);
}

void BaseGameObject::SetCollider(const std::string& categoryName, const std::string& shapeName) {
	ColliderShape shape = ColliderShape::SPHERE;
	if (shapeName == "SPHERE") {
		collider_ = std::make_unique<SphereCollider>();
		shape = ColliderShape::SPHERE;
	} else if (shapeName == "BOX") {
		collider_ = std::make_unique<BoxCollider>();
		shape = ColliderShape::AABB;
	}

	collider_->Init(categoryName, shape);
	collider_->SetName(categoryName);
	collider_->SetCategory(categoryName);
	ColliderCollector::AddCollider(collider_.get());

	collider_->Update(QuaternionSRT{ .scale = transform_->GetScale(),
						  .rotate = transform_->GetQuaternion(),
						  .translate = transform_->GetTranslation() }
	);
}

void BaseGameObject::SetPhysics() {
	rigidbody_ = std::make_unique<Rigidbody>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　modelを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetObject(const std::string& objName) {
	model_ = nullptr;
	materials.clear();

	model_ = ModelManager::GetModel(objName);
	for (const auto& material : model_->GetMaterialData()) {
		materials.emplace_back(Engine::CreateMaterial(material.second));
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

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialの色を変える
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetColor(const Vector4& color) {
	for (size_t oi = 0; oi < materials.size(); ++oi) {
		materials[oi]->SetColor(color);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　lightingの設定を変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetIsLighting(bool isLighting) {
	for (size_t oi = 0; oi < materials.size(); ++oi) {
		materials[oi]->SetIsLighting(isLighting);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialのtextureを変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetTexture(const std::string& path) {
	for (size_t oi = 0; oi < materials.size(); ++oi) {
		materials[oi]->SetUseTexture(path);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Debug_Draw() {
	// Debug表示
	if (objectAxis_ != nullptr) {
		objectAxis_->Draw();
	}
}

void BaseGameObject::Debug_Gui() {
	transform_->Debug_Gui();
	int index = 0;
	for (auto& material : materials) {
		std::string guiId = "material_" + std::to_string(index);
		if (ImGui::TreeNode(guiId.c_str())) {
			material->Debug_Gui();
			ImGui::TreePop();
		}
	}

	if (collider_ != nullptr) {
		collider_->Debug_Gui();
	}

	Debug_Axis();

	if (animetor_ != nullptr) {
		animetor_->Debug_Gui();
	}
}

void BaseGameObject::Debug_Axis() {
	// debug姿勢表示のonoff
	if (!isDebugAxis_) {
		ImGui::Checkbox("debugAxis", &isDebugAxis_);

		if (isDebugAxis_) {
			SetObjectAxis();
		}
	} else {
		ImGui::Checkbox("debugAxis", &isDebugAxis_);

		if (!isDebugAxis_) {
			SetObjectAxis(false);
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの姿勢を可視化させる
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetObjectAxis(bool isAxis) {
	if (isAxis) {
		objectAxis_ = std::make_unique<ObjectAxis>();
		objectAxis_->Init();
		isDebugAxis_ = true;
	} else if (objectAxis_ != nullptr) {
		objectAxis_->Finalize();
		objectAxis_ = nullptr;
		isDebugAxis_ = false;
	}
}