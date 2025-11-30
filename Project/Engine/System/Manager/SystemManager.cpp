#include "SystemManager.h"

void SystemManager::Init() {
	behaviorTree_ = BehaviorTreeSystem::GetInstance();
	behaviorTree_->Init();
}

void SystemManager::Update() {
	behaviorTree_->Update();
}