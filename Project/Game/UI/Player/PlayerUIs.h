#pragma once
#include <memory>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
// Game
#include "Game/Actor/Player/Player.h"
#include "Game/UI/PostureStability.h"

/// <summary>
/// PlayerのUIをまとめたクラス
/// </summary>
class PlayerUIs :
	public AttributeGui {
public:

	struct UIItems : public IJsonConverter {
		Vector2 healthScale = { 1.0f, 1.0f };
		Vector2 healthPos = { 640.0f, 360.0f };

		Vector2 postureScale = { 1.0f, 1.0f };
		Vector2 posturePos = { 640.0f, 360.0f };

		UIItems() { SetName("PlayerUIItems"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("healthScale", healthScale)
				.Add("healthPos", healthPos)
				.Add("postureScale", postureScale)
				.Add("posturePos", posturePos)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "healthScale", healthScale);
			fromJson(jsonData, "healthPos", healthPos);
			fromJson(jsonData, "postureScale", postureScale);
			fromJson(jsonData, "posturePos", posturePos);
		}
	};

public:

	PlayerUIs() = default;
	~PlayerUIs() = default;

	void Init(Player* _player);

	void Update();

	void Draw() const;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:

	Player* pPlayer_;

	UIItems uiItems_;

	std::unique_ptr<BaseGaugeUI> health_;

	std::unique_ptr<PostureStability> postureStability_;

};

