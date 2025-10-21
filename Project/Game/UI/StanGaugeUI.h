#pragma once
#include <memory>
#include <string>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
// Game
#include "Game/UI/BaseGaugeUI.h"

class StanGaugeUI :
	public BaseGaugeUI {
public:

	struct Parameter : public IJsonConverter {
		float popInterval = 0.2f;
		float minAlpha = 0.2f;
		float maxAlpha = 0.8f; 

		Parameter() { SetName("stanGaugeParameter"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("popInterval", popInterval)
				.Add("minAlpha", minAlpha)
				.Add("maxAlpha", maxAlpha)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "popInterval", popInterval);
			fromJson(jsonData, "minAlpha", minAlpha);
			fromJson(jsonData, "maxAlpha", maxAlpha);
		}

		void Debug_Gui() override {};
	};

public:

	StanGaugeUI() = default;
	~StanGaugeUI() = default;

	void Init(const std::string& _groupName, const std::string& _itemName);

	void Update(float _fillAmount);

	void Debug_Gui() override;

public:

	void Pop();

	void SetIsEnable(bool _isActive);

private:

	// 色の変化に関する変数
	Parameter param_;

	VectorTween<Vector2> scaleTween_;
	VectorTween<Color> colorTween_;

};

