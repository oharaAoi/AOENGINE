#include "BehaviorTreeSystem.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

BehaviorTreeSystem* BehaviorTreeSystem::GetInstance() {
	static BehaviorTreeSystem instance;
	return &instance;
}

void BehaviorTreeSystem::Init() {
	trees_.clear();
}

void BehaviorTreeSystem::Update() {
#ifdef _DEBUG
	// 各Treeの表示
	for (auto& tree : trees_) {
		if (ImGui::Begin(tree->GetName().c_str(), nullptr, ImGuiWindowFlags_None)) {
			// treeの表示
			tree->Edit();
			// inspectorの表示
			auto t = tree.get();
			auto& fuc = EditorWindows::GetInstance()->GetWindowUpdate();

			fuc = [t]() {
				t->EditSelect();
				};
		}
		ImGui::End();
	}
#endif
}

BehaviorTree* BehaviorTreeSystem::Create() {
	auto& tree = trees_.emplace_back(std::make_unique<BehaviorTree>());
	return tree.get();
}