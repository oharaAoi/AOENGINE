#pragma once
#include "Game/UI/BaseGaugeUI.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// 姿勢安定のゲージ
/// </summary>
class PostureStability :
	public BaseGaugeUI {
public:

	struct Parameter : public IJsonConverter {
		Vector4 normalColor;
		Vector4 pinchColor;

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
	};

public:

	PostureStability() = default;
	~PostureStability() override = default;

	void Init();
	
	void Update(float _fillAmount);

	void Draw() const override;

	void Debug_Gui() override;

private:

	std::unique_ptr<Sprite> fence_;

	Parameter param_;

};

