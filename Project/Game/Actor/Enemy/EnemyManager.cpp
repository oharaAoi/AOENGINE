#include "EnemyManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyManager::Init() {
	SetName("EnemyManager");

#ifdef _DEBUG
	AOENGINE::EditorWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyManager::Update() {
	for (auto& enemy : enemyList_) {
		enemy->Update();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyManager::Debug_Gui() {
	if (ImGui::CollapsingHeader("Enemy List")) {
		for (auto& enemy : enemyList_) {
			std::string id = enemy->GetName() + "##" + std::to_string(enemy->GetID());
			if (ImGui::TreeNode(id.c_str())) {
				enemy->Debug_Gui();
				ImGui::TreePop();
			}
		}
	}

	ImGui::Separator();
	ImGui::BulletText("Create Enemy");
	static int enemyType = 0;
	ImGui::RadioButton("近距離", &enemyType, (int)EnemyType::Short);
	ImGui::SameLine();
	ImGui::RadioButton("中距離", &enemyType, (int)EnemyType::Mid);
	ImGui::SameLine();
	ImGui::RadioButton("遠距離", &enemyType, (int)EnemyType::Long);

	if (ImGui::Button("Create")) {
		EnemyType type = static_cast<EnemyType>(enemyType);
		auto& newEnemy = enemyList_.emplace_back(enemyFactory_.Create(type));
		newEnemy->Init();
	}

}