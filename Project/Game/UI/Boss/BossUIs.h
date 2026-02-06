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

/// <summary>
/// BossのUIをまとめたクラス
/// </summary>
class BossUIs :
	public AOENGINE::AttributeGui {
public:

	BossUIs() = default;
	~BossUIs() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_boss"></param>
	/// <param name="_player"></param>
	void Init(Boss* _boss, Player* _player);

	// 更新
	void Update(const Math::Vector2& _reticlePos);

	void SetIsEnable(bool flag);

	// 編集
	void Debug_Gui() override;

	void AlertUpdate();

public:

	/// <summary>
	/// 警告の出現
	/// </summary>
	/// <param name="_targetPos">: ターゲットの位置</param>
	/// <param name="_attackerPos">: 攻撃者の位置</param>
	void PopAlert(const Math::Vector3& _targetPos, const Math::Vector3& _attackerPos);

private:

	Boss* pBoss_ = nullptr;
	Player* pPlayer_ = nullptr;

	// hp関連
	std::unique_ptr<BossHealthUI> health_;
	std::unique_ptr<BaseGaugeUI> healthArc_;
	
	// 姿勢制御関連
	std::unique_ptr<PostureStability> postureStability_;
	std::unique_ptr<BaseGaugeUI> postureStabilityArc_;

	// スタン系
	AOENGINE::Sprite* stunPromote_;

	std::list<std::unique_ptr<EnemyAttackAlert>> attackAlertList_;
};

