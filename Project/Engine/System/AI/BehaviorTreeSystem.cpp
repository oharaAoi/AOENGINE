#include "BehaviorTreeSystem.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

BehaviorTreeSystem* BehaviorTreeSystem::GetInstance() {
	static BehaviorTreeSystem instance;
	return &instance;
}

void BehaviorTreeSystem::Update() {
    for (auto& tree : trees_) {
        if (ImGui::Begin(tree->GetName().c_str(), nullptr, ImGuiWindowFlags_None)) {
            tree->Edit();

            if (ImGui::IsWindowFocused()) {
                auto t = tree.get();  // shared_ptr をコピー（参照カウント++）
                auto& fuc = EditorWindows::GetInstance()->GetWindowUpdate();

                fuc = [t]() {
                    t->EditSelect();   // * は不要、Edit() が void* でない限り
                    };
            }
        }
        ImGui::End();
    }
}

BehaviorTree* BehaviorTreeSystem::Create() {
	auto& tree = trees_.emplace_back(std::make_unique<BehaviorTree>());
	return tree.get();
}