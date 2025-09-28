#pragma once
#include <memory>
#include <string>
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

class StanGaugeUI :
	public AttributeGui {
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

	void Init(const Vector2& scale, const Vector2& pos);

	void Update();

	void Draw() const;

	void Debug_Gui() override;

public:

	void Pop();

	void SetIsEnable(bool _isActive);

private:

	std::unique_ptr<Sprite> gauge_;
	std::unique_ptr<Sprite> bg_;

	// 色の変化に関する変数
	Parameter param_;

	Vector2 scale_;
	VectorTween<Vector2> scaleTween_;

	float alpha_;
	VectorTween<float> alphaTween_;

};

