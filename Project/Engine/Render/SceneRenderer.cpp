#include "SceneRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Utilities/Logger.h"

SceneRenderer* SceneRenderer::GetInstance() {
	static SceneRenderer instance;
	return &instance;
}

void SceneRenderer::Finalize() {
	for (auto& pair : objectList_) {
		pair.object->Finalize();
	}
	objectList_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Init() {
	objectList_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Update() {
	for (auto it = objectList_.begin(); it != objectList_.end(); ) {
		if (it->object->GetIsDestroy()) {
			it = objectList_.erase(it);
		} else {
			++it;
		}
	}

	// ソートを行う
	objectList_.sort([](const ObjectPair& objA, const ObjectPair& objB) {
		return objA.renderingType < objB.renderingType;
	});

	for (auto& pair : objectList_) {
		if (pair.object->GetIsActive()) {
			pair.object->Update();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::Draw() const {
	for (auto& pair : objectList_) {
		if (pair.object->GetIsActive()) {
			Engine::SetPipeline(PSOType::Object3d, pair.renderingType);
			pair.object->Draw();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SceneRenderer::CreateObject(const SceneLoader::LevelData* loadData) {
	// levelDataからobjectを作成する
	for (const auto& data : loadData->objects) {
		ObjectPair pair = ObjectPair("Object_Normal.json", std::make_unique<BaseGameObject>());
		pair.object->Init();
		pair.object->SetName(data.name);
		pair.object->SetObject(data.modelName);
		pair.object->GetTransform()->SetSRT(data.srt);
		objectList_.push_back(std::move(pair));
	}
}

BaseGameObject* SceneRenderer::AddObject(const std::string& objectName, const std::string& renderingName) {
	ObjectPair pair = ObjectPair(renderingName, std::make_unique<BaseGameObject>());
	BaseGameObject* gameObject = pair.object.get();
	pair.object->Init();
	pair.object->SetName(objectName);
	objectList_.push_back(std::move(pair));
	return gameObject;
}

void SceneRenderer::ReleaseObject(BaseGameObject* objPtr) {
	for (auto it = objectList_.begin(); it != objectList_.end(); ) {
		if (it->object.get() == objPtr) {
			it = objectList_.erase(it);
		} else {
			++it;
		}
	}
}

void SceneRenderer::ChangeRenderingType(const std::string& renderingName, BaseGameObject* gameObject) {
	for (auto& pair : objectList_) {
		if (pair.object.get() == gameObject) {
			pair.renderingType = renderingName;
		}
	}
}

BaseGameObject* SceneRenderer::GetGameObject(const std::string& objName) {
	for (auto& pair : objectList_) {
		if (pair.object->GetName() == objName) {
			return pair.object.get();
		}
	}

	Logger::Log("[" + objName + "]が見つかりませんでした");
	return nullptr;
}
