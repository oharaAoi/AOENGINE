#pragma once
#include <list>
#include <memory>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
// Game
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

	void Init(Boss* _boss);

	void Update();

	void Debug_Gui() override;

public:

	void PopAlert();

	void PopStan();

private:

	Boss* pBoss_ = nullptr;

	std::unique_ptr<BossHealthUI> health_;
	std::unique_ptr<PostureStability> postureStability_;
	std::unique_ptr<ArmorDurabilityUI> armorDurability_;
	std::unique_ptr<StanGaugeUI> stanGaugeUI_;

	std::list<std::unique_ptr<EnemyAttackAlert>> attackAlertList_;
};

