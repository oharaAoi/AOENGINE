#include "ObjectAxis.h"
#include "Engine/Engine.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

ObjectAxis::ObjectAxis() {
}

ObjectAxis::~ObjectAxis() {
}

void ObjectAxis::Finalize() {
}

void ObjectAxis::Init() {
	transform_ = Engine::CreateWorldTransform();

	model_ = nullptr;
	model_ = ModelManager::GetModel("axis.obj");
	materials.clear();
}

void ObjectAxis::Update(const Matrix4x4& parentWorldMat) {
	transform_->Update(parentWorldMat);
}

void ObjectAxis::Draw() const {
	Pipeline* pso = Engine::GetLastUsedPipeline();
	Render::DrawModel(pso, model_, transform_.get(), materials);
}
