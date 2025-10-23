#pragma once
#include <string>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Game/UI/BaseGaugeUI.h"

enum class GaugeType {
	Posturebility,
	Armor,
	Stan
};

/// <summary>
/// 姿勢安定のゲージ
/// </summary>
class PostureStability :
	public BaseGaugeUI {
public:

	struct Parameter : public IJsonConverter {
		Color normalColor;
		Color pinchColor;
		Color armorColor;
		Color stanColor;

		Parameter() { SetName("postureStability"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("normalColor", normalColor)
				.Add("pinchColor", pinchColor)
				.Add("armorColor", armorColor)
				.Add("stanColor", stanColor)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "normalColor", normalColor);
			fromJson(jsonData, "pinchColor", pinchColor);
			fromJson(jsonData, "armorColor", armorColor);
			fromJson(jsonData, "stanColor", stanColor);
		}

		void Debug_Gui() override {};
	};

public:

	PostureStability() = default;
	~PostureStability() override = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_groupName"></param>
	/// <param name="_itemName"></param>
	void Init(const std::string& _groupName, const std::string& _itemName);
	
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_fillAmount"></param>
	void Update(float _fillAmount);

	/// <summary>
	/// 編集
	/// </summary>
	void Debug_Gui() override;

public:

	void SetGaugeType(GaugeType _type) { gaugeType_ = _type; }

private:

	Sprite* fence_;

	Parameter param_;

	GaugeType gaugeType_;

	VectorTween<Color> stanAnimation_;
};

