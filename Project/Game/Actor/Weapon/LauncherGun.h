#pragma once
// Engine
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

/// <summary>
/// ランチャーガン
/// </summary>
class LauncherGun :
	public BaseWeapon {
public:

	LauncherGun() = default;
	~LauncherGun() override = default;

public:

	// 終了処理
	void Finalize() override;
	// 初期化
	void Init() override;
	// 編集
	void Debug_Gui() override;

public:		// member method

	/// <summary>
	/// 攻撃処理
	/// </summary>
	/// <param name="cxt">: 攻撃の内容</param>
	/// <returns></returns>
	bool Attack(const AttackContext& cxt) override;

private:

	BaseParticles* gunFireParticles_;
	Math::Vector3 offset_;

};

