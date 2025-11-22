#include "SystemManager.h"

void SystemManager::Init() {
	behaviorTree_ = BehaviorTreeSystem::GetInstance();
}

void SystemManager::Update() {
	behaviorTree_->Update();
}