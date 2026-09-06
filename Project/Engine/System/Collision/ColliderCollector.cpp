#include "ColliderCollector.h"
#include <algorithm>

using namespace AOENGINE;

ColliderCollector* ColliderCollector::GetInstance() {
    static ColliderCollector instance;
    return &instance;
}

void ColliderCollector::Init() {
    colliderList_.clear();
}

void ColliderCollector::Update() {
	// colliderのポインタがnullになっていたら一覧から削除する
	colliderList_.erase(std::remove_if(colliderList_.begin(), colliderList_.end(), [](BaseCollider* _collider) {
		return _collider == nullptr;
	}), colliderList_.end());
}

void ColliderCollector::Draw() const {
    for (auto& collider : colliderList_) {
        collider->Draw();
    }
}

void ColliderCollector::AddCollider(BaseCollider* _collider) {
    GetInstance()->AddColliderList(_collider);
}

void ColliderCollector::AddColliderList(BaseCollider* _collider) {
	if (std::find(colliderList_.begin(), colliderList_.end(), _collider) == colliderList_.end()) {
		colliderList_.push_back(_collider);
	}
}

void ColliderCollector::RemoveCollider(BaseCollider* _collider) {
	colliderList_.erase(std::remove(colliderList_.begin(), colliderList_.end(), _collider), colliderList_.end());
}
