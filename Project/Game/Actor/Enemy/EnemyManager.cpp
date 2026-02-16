#include "EnemyManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "ENgine/Render.h"
#include "Game/Information/ColliderCategory.h"


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
		newEnemy->SetEnemyBulletManager(enemyBulletManager_.get());
	}

}

void EnemyManager::AddListToBoss(Boss* boss) {
	enemyList_.emplace_back(boss);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyManager::Init() {
	SetName("EnemyManager");

	bossRoot_ = std::make_unique<BossRoot>();
	bossRoot_->Init();

	enemyBulletManager_ = std::make_unique<EnemyBulletManager>();
	enemyBulletManager_->Init();

	// bossを生成
	auto& boss = enemyList_.emplace_back(enemyFactory_.Create(EnemyType::Boss));
	pBoss_ = dynamic_cast<Boss*>(boss.get());
	pBoss_->Init();

	bossRoot_->SetBoss(pBoss_);

#ifdef _DEBUG
	AOENGINE::EditorWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyManager::Update() {
	std::erase_if(enemyList_, [](const std::unique_ptr<BaseEnemy>& enemy) {
		return enemy->GetIsDead();
	});

	for (auto& enemy : enemyList_) {
		enemy->Update();

		// カメラに写っているかを判定する
		if (CheckWithinCamera(enemy.get())) {
			enemy->SetIsWithinSight(true);
		} else {
			enemy->SetIsWithinSight(false);
		}
	}

	if (pPlayer_) {
		enemyBulletManager_->SetPlayerPosition(pPlayer_->GetPosition());
	}
	enemyBulletManager_->Update();

	bossRoot_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 敵がカメラに写っているかを判定する
///////////////////////////////////////////////////////////////////////////////////////////////

bool EnemyManager::CheckWithinCamera(BaseEnemy* enemy) {
	Math::Vector4 clip = Math::Vector4(enemy->GetPosition(), 1.0f) * AOENGINE::Render::GetViewProjectionMat();
	if (clip.w <= 0.0f) return false;

	Math::Vector3 ndc;
	ndc.x = clip.x / clip.w;
	ndc.y = clip.y / clip.w;
	ndc.z = clip.z / clip.w;

	bool visible =
		ndc.x >= -1 && ndc.x <= 1 &&
		ndc.y >= -1 && ndc.y <= 1 &&
		ndc.z >= 0 && ndc.z <= 1;

	return visible;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ポインタに対するColliderの探索
///////////////////////////////////////////////////////////////////////////////////////////////

BaseEnemy* EnemyManager::SearchCollider(AOENGINE::BaseCollider* collider) {
	for (std::unique_ptr<BaseEnemy>& enemy : enemyList_) {
		if (enemy->GetCollider(collider->GetCategoryName()) == collider) {
			return enemy.get();
		}
	}
	return nullptr;
}