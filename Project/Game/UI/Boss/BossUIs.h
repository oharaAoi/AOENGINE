#pragma once
#include <memory>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
// Game
#include "Game/Actor/Boss/Boss.h"
#include "Game/UI/Boss/BossHealthUI.h"
#include "Game/UI/PostureStability.h"
#include "Game/UI/ArmorDurabilityUI.h"

/// <summary>
/// BossのUIをまとめたクラス
/// </summary>
class BossUIs :
	public AttributeGui {
public:

	struct UIItems : public IJsonConverter {
		Vector2 healthScale = {1.0f, 1.0f};
		Vector2 healthPos = { 640.0f, 360.0f };

		Vector2 postureScale = { 1.0f, 1.0f };
		Vector2 posturePos = { 640.0f, 360.0f };

		UIItems() { SetName("BossUIItems"); }

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

	BossUIs() = default;
	~BossUIs() = default;

	void Init(Boss* _boss);

	void Update();

	void Draw() const;

	void Debug_Gui() override;

private:

	Boss* pBoss_ = nullptr;

	UIItems uiItems_;

	std::unique_ptr<BossHealthUI> health_;
	std::unique_ptr<PostureStability> postureStability_;
	std::unique_ptr<ArmorDurabilityUI> armorDurability_;

};

