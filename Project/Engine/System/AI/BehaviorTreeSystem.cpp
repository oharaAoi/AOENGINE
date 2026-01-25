#include "BehaviorTreeSystem.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

using namespace AI;

BehaviorTreeSystem* BehaviorTreeSystem::GetInstance() {
	static BehaviorTreeSystem instance;
	return &instance;
}

void BehaviorTreeSystem::Finalize() {
	trees_.clear();
}

void BehaviorTreeSystem::Init() {
	trees_.clear();
}

void BehaviorTreeSystem::Update() {
#ifdef _DEBUG
	if (trees_.empty()) {
		return;
	}

	// 各Treeの表示
	for (auto& tree : trees_) {
		// treeの表示
		tree->Edit();
		tree->EditSelect();
	}
#endif
}

BehaviorTree* BehaviorTreeSystem::Create() {
	auto& tree = trees_.emplace_back(std::make_unique<BehaviorTree>());
	return tree.get();
}

void AI::BehaviorTreeSystem::SetIsOpenEditor(const std::string& name) {
	for (auto& tree : trees_) {
		if (tree->GetName() == name) {
			tree->SetIsOpenEditor(true);
		}
	}
}
