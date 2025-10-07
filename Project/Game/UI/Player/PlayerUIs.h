#pragma once
#include <memory>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
// Game
#include "Game/Actor/Player/Player.h"
#include "Game/UI/Health.h"
#include "Game/UI/PostureStability.h"
#include "Game/UI/Player/EnergyOutput.h"
#include "Game/UI/Player/WeaponRemainingRounds.h"

/// <summary>
/// PlayerのUIをまとめたクラス
/// </summary>
class PlayerUIs :
	public AttributeGui {
public:

	PlayerUIs() = default;
	~PlayerUIs() = default;

	void Init(Player* _player);

	void Update(const Vector2& reticlePos);

	void Debug_Gui() override;

private:

	Player* pPlayer_;

	Sprite* ap_;
	std::unique_ptr<EnergyOutput> energyOutput_;
	std::unique_ptr<Health> health_;

	std::unique_ptr<PostureStability> postureStability_;

	std::unique_ptr<WeaponRemainingRounds> leftWeapon_;
	std::unique_ptr<WeaponRemainingRounds> rightWeapon_;

};

