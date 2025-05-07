#include "ColliderCollector.h"
#include "Engine/Engine.h"

ColliderCollector* ColliderCollector::GetInstance() {
    static ColliderCollector instance;
    return &instance;
}

void ColliderCollector::Init() {
    colliderList_.clear();
}

void ColliderCollector::Update() {
    // colliderのポインタがnullになっていたらリストから削除する
    colliderList_.remove_if([](ICollider* _collider) {
        return _collider == nullptr;
    });
}

void ColliderCollector::Draw() const {
    Engine::SetPSOPrimitive();
    for (auto& collider : colliderList_) {
        collider->Draw();
    }
}

void ColliderCollector::AddCollider(ICollider* _collider) {
    GetInstance()->AddColliderList(_collider);
}

void ColliderCollector::AddColliderList(ICollider* _collider) {
    if (std::find(colliderList_.begin(), colliderList_.end(), _collider) == colliderList_.end()) {
        colliderList_.push_back(_collider);
    }
}

void ColliderCollector::RemoveCollider(ICollider* _collider) {
    colliderList_.remove(_collider);
}
