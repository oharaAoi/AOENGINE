#pragma once
#include <string>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// 姿勢安定のゲージ
/// </summary>
class PostureStability :
	public BaseGaugeUI {
public:

	struct Parameter : public IJsonConverter {
		Color normalColor;
		Color pinchColor;

		Parameter() { SetName("postureStability"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("normalColor", normalColor)
				.Add("pinchColor", pinchColor)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "normalColor", normalColor);
			fromJson(jsonData, "pinchColor", pinchColor);
		}

		void Debug_Gui() override {};
	};

public:

	PostureStability() = default;
	~PostureStability() override = default;

	void Init(const std::string& _groupName, const std::string& _itemName);
	
	void Update(float _fillAmount);

	void Debug_Gui() override;

private:

	std::unique_ptr<Sprite> fence_;

	Parameter param_;

};

