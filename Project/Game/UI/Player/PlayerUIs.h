#pragma once
#include <memory>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
// Game
#include "Game/Actor/Player/Player.h"
#include "Game/UI/PostureStability.h"
#include "Game/UI/Player/EnergyOutput.h"
#include "Game/UI/Player/WeaponRemainingRounds.h"

/// <summary>
/// PlayerのUIをまとめたクラス
/// </summary>
class PlayerUIs :
	public AttributeGui {
public:

	struct UIItems : public IJsonConverter {
		Vector2 apScale = { 1.0f, 1.0f };
		Vector2 apPos = { 640.0f, 360.0f };

		Vector2 healthScale = { 1.0f, 1.0f };
		Vector2 healthPos = { 640.0f, 360.0f };

		Vector2 postureScale = { 1.0f, 1.0f };
		Vector2 posturePos = { 640.0f, 360.0f };

		UIItems() { 
			SetGroupName("PlayerUIs");
			SetName("PlayerUIItems");
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("apScale", apScale)
				.Add("apPos", apPos)
				.Add("healthScale", healthScale)
				.Add("healthPos", healthPos)
				.Add("postureScale", postureScale)
				.Add("posturePos", posturePos)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "apScale", apScale);
			fromJson(jsonData, "apPos", apPos);
			fromJson(jsonData, "healthScale", healthScale);
			fromJson(jsonData, "healthPos", healthPos);
			fromJson(jsonData, "postureScale", postureScale);
			fromJson(jsonData, "posturePos", posturePos);
		}

		void Debug_Gui() override;
	};

public:

	PlayerUIs() = default;
	~PlayerUIs() = default;

	void Init(Player* _player);

	void Update(const Vector2& reticlePos);

	void Draw() const;

	void Debug_Gui() override;

private:

	Player* pPlayer_;

	UIItems uiItems_;

	std::unique_ptr<Sprite> ap_;
	std::unique_ptr<EnergyOutput> energyOutput_;
	std::unique_ptr<BaseGaugeUI> health_;

	std::unique_ptr<PostureStability> postureStability_;

	std::unique_ptr<WeaponRemainingRounds> leftWeapon_;
	std::unique_ptr<WeaponRemainingRounds> rightWeapon_;

};

