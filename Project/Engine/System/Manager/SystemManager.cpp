#include "SystemManager.h"

using namespace AOENGINE;

void SystemManager::Init() {
	behaviorTree_ = AI::BehaviorTreeSystem::GetInstance();
	behaviorTree_->Init();
}

void SystemManager::Update() {
	behaviorTree_->Update();
}