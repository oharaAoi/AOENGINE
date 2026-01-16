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
		if (ImGui::Begin(tree->GetName().c_str(), nullptr, ImGuiWindowFlags_None)) {
			// treeの表示
			tree->Edit();
			tree->EditSelect();
		}
		ImGui::End();
	}
#endif
}

BehaviorTree* BehaviorTreeSystem::Create() {
	auto& tree = trees_.emplace_back(std::make_unique<BehaviorTree>());
	return tree.get();
}