#include "SceneRenderer.h"
#include "Engine/Engine.h"

SceneRenderer* SceneRenderer::GetInstance() {
	static SceneRenderer instance;
	return &instance;
}

void SceneRenderer::Init() {
	object3dList_.clear();
}

void SceneRenderer::Update() {
	// ソートを行う
	object3dList_.sort([](const ObjectPair& objA, const ObjectPair& objB) {
		return objA.renderingType < objB.renderingType;
	});
}

void SceneRenderer::Draw() const {
	for (auto& pair : object3dList_) {
		Engine::SetPipeline(PSOType::Object3d, pair.renderingType);
		pair.object->Draw();
	}
}

void SceneRenderer::SetObject(const std::string& typeFile, BaseGameObject* object) {
	// nullなら早期
	if (object == nullptr) { return; }
	// ペア作成
	ObjectPair pair = ObjectPair(typeFile, object);
	object3dList_.push_back(pair);
}

void SceneRenderer::ReleaseObject(BaseGameObject* object) {
	BaseGameObject* target = object;

	object3dList_.remove_if([&](ObjectPair pair) {
		return pair.object == target;
							});
}
