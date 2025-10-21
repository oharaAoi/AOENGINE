#pragma once
#define NOMINMAX
#include <list>
#include <memory>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
// Game
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/UI/Boss/BossHealthUI.h"
#include "Game/UI/Boss/EnemyAttackAlert.h"
#include "Game/UI/PostureStability.h"
#include "Game/UI/ArmorDurabilityUI.h"
#include "Game/UI/StanGaugeUI.h"

/// <summary>
/// BossのUIをまとめたクラス
/// </summary>
class BossUIs :
	public AttributeGui {
public:

	BossUIs() = default;
	~BossUIs() = default;

	void Init(Boss* _boss, Player* _player);

	void Update();

	void Debug_Gui() override;

public:

	void PopAlert(const Vector3& _targetPos, const Vector3& _attackerPos);

private:

	Boss* pBoss_ = nullptr;
	Player* pPlayer_ = nullptr;

	std::unique_ptr<BossHealthUI> health_;
	std::unique_ptr<PostureStability> postureStability_;

	std::list<std::unique_ptr<EnemyAttackAlert>> attackAlertList_;
};

