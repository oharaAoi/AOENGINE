#include "SceneRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"

SceneRenderer* SceneRenderer::GetInstance() {
	static SceneRenderer instance;
	return &instance;
}

void SceneRenderer::Finalize() {
	objectList_.clear();
	particleManager_->Finalize();
	gpuParticleManager_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init() {
	objectList_.clear();

	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Init();

	gpuParticleManager_ = GpuParticleManager::GetInstance();
	gpuParticleManager_->Init();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Update() {
	// 削除処理
	for (auto it = objectList_.begin(); it != objectList_.end(); ) {
		if ((*it)->GetSceneObject()->GetIsDestroy()) {
			it = objectList_.erase(it);
		} else {
			++it;
		}
	}

	// ソートする
	objectList_.sort([](const std::unique_ptr<IObjectPair>& a, const std::unique_ptr<IObjectPair>& b) {
		if (a->GetRenderQueue() == b->GetRenderQueue()) {
			return a->GetRenderingType() < b->GetRenderingType();
		}
		return a->GetRenderQueue() == b->GetRenderQueue();
	});

	// objectの更新
	for (auto& pair : objectList_) {
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			obj->Update();
		}
	}

	// particleの更新
	particleManager_->Update();
	gpuParticleManager_->Update();
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
	// 影の描画
	Render::SetShadowMap();
	for (auto& pair : objectList_) {
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			obj->PreDraw();
		}
	}

	// objectの描
	std::vector<RenderTargetType> types;
	types.push_back(RenderTargetType::Object3D_RenderTarget);
	types.push_back(RenderTargetType::MotionVector_RenderTarget);
	Render::SetRenderTarget(types, GraphicsContext::GetInstance()->GetDxCommon()->GetDepthHandle());

	Render::ChangeShadowMap();
	for (auto& pair : objectList_) {
		if (pair->GetPostDraw()) {
			continue;
		}
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			Engine::SetPipeline(PSOType::Object3d, pair->GetRenderingType());
			obj->Draw();
		}
	}

	// particleの描画
	particleManager_->Draw();
	gpuParticleManager_->Draw();

#ifdef _DEBUG
	Render::GetShadowMap()->Debug_Gui();
#endif
}

void SceneRenderer::PostDraw() const {
	for (auto& pair : objectList_) {
		if (!pair->GetPostDraw()) {
			continue;
		}
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			Engine::SetPipeline(PSOType::Object3d, pair->GetRenderingType());
			obj->Draw();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::EditObject(const ImVec2& windowSize, const ImVec2& imagePos) {
	for (auto& pair : objectList_) {
		ISceneObject* obj = pair->GetSceneObject();
		if (obj->GetIsActive()) {
			obj->Manipulate(windowSize, imagePos);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::CreateObject(SceneLoader::LevelData* loadData) {
	// levelDataからobjectを作成する
	for (auto& data : loadData->objects) {
		auto object = std::make_unique<BaseGameObject>();
		object->Init();
		object->SetName(data.name);
		if (data.modelName != "") {
			object->SetObject(data.modelName);
		}
		object->GetTransform()->SetSRT(data.srt);

		// colliderが設定されていたら
		if (!data.collidersData.empty()) {
			for (auto colliderData : data.collidersData) {
				object->SetCollider(colliderData.colliderTag, colliderData.colliderType);
				ICollider* collider = object->GetCollider(colliderData.colliderTag);
				collider->SetLoacalPos(colliderData.center);
				if (colliderData.colliderType == "BOX") {
					BoxCollider* box = dynamic_cast<BoxCollider*>(collider);
					box->SetSize(colliderData.size);
				} else if (colliderData.colliderType == "SPHERE") {
					collider->SetRadius(colliderData.radius);
				}
				// colliderのtagが設定されていたら代入
				if (colliderData.colliderTag != "") {
					collider->SetCategory(colliderData.colliderTag);
				} else {
					collider->SetCategory("none");
				}

				// collisionFilterが設定されていたら
				if (!colliderData.filter.empty()) {
					for (size_t index = 0; index < colliderData.filter.size(); ++index) {
						collider->SetTarget(colliderData.filter[index]);
					}
				}
				collider->Update(data.srt);
			}
		}

		object->SetIsRendering(data.isRendering_);

		auto pair = std::make_unique<ObjectPair<BaseGameObject>>("Object_Normal.json", 0, false,  std::move(object));
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

void SceneRenderer::SetRenderingQueue(const std::string& objName, int num) {
	for (auto& pair : objectList_) {
		if (pair->GetSceneObject()->GetName() == objName) {
			pair->SetRenderQueue(num);
		}
	}
}
