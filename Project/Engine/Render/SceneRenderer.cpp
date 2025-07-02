#include "SceneRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

SceneRenderer* SceneRenderer::GetInstance() {
	static SceneRenderer instance;
	return &instance;
}

void SceneRenderer::Finalize() {
	objectList_.clear();
	particleManager_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init() {
	objectList_.clear();

	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Init();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Update() {
	for (auto it = objectList_.begin(); it != objectList_.end(); ) {
		if ((*it)->GetSceneObject()->GetIsDestroy()) {
			it = objectList_.erase(it);
		} else {
			++it;
		}
	}

	objectList_.sort([](const std::unique_ptr<IObjectPair>& a, const std::unique_ptr<IObjectPair>& b) {
		if (a->GetRenderQueue() == b->GetRenderQueue()) {
			return a->GetRenderingType() < b->GetRenderingType();
		}
		return a->GetRenderQueue() == b->GetRenderQueue();
	});

	for (auto& pair : objectList_) {
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			obj->Update();
		}
	}

	particleManager_->Update();
}

void SceneRenderer::PostUpdate() {
	for (auto& pair : objectList_) {
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			obj->PostUpdate();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Draw() const {
	for (auto& pair : objectList_) {
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			Engine::SetPipeline(PSOType::Object3d, pair->GetRenderingType());
			obj->Draw();
		}
	}

	particleManager_->Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::CreateObject(const SceneLoader::LevelData* loadData) {
	// levelDataからobjectを作成する
	for (const auto& data : loadData->objects) {
		auto object = std::make_unique<BaseGameObject>();
		object->Init();
		object->SetName(data.name);
		object->SetObject(data.modelName);
		object->GetTransform()->SetSRT(data.srt);

		// colliderが設定されていたら
		if (data.colliderType != "") {
			object->SetCollider("none", data.colliderType);
			ICollider* collider = object->GetCollider();
			collider->SetLoacalPos(data.colliderCenter);
			if (data.colliderType == "BOX") {
				collider->SetSize(data.colliderSize);
			} else if (data.colliderType == "SPHERE") {
				collider->SetRadius(data.colliderRadius);
			}
		}

		auto pair = std::make_unique<ObjectPair<BaseGameObject>>("Object_Normal.json", 0, std::move(object));
		objectList_.push_back(std::move(pair));
	}
}

void SceneRenderer::ReleaseObject(ISceneObject* objPtr) {
	for (auto it = objectList_.begin(); it != objectList_.end(); ) {
		if ((*it)->GetSceneObject() == objPtr) {
			it = objectList_.erase(it);
		} else {
			++it;
		}
	}
}

void SceneRenderer::ChangeRenderingType(const std::string& renderingName, ISceneObject* gameObject) {
	for (auto& pair : objectList_) {
		if (pair->GetSceneObject() == gameObject) {
			pair->SetRenderingType(renderingName);
		}
	}
}