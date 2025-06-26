#pragma once
#include <unordered_map>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// Bossの次の行動を決定するクラス
/// </summary>
class BossAI :
	public AttributeGui {
public:

	struct Parameter : public IJsonConverter {
		float farDistance = 80.0f;
		float midDistance = 60.0f;
		float nearDistance = 10.0f;

		Parameter() { SetName("bossAIParameter"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("farDistance", farDistance)
				.Add("midDistance", midDistance)
				.Add("nearDistance", nearDistance)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "farDistance", farDistance);
			fromJson(jsonData, "midDistance", midDistance);
			fromJson(jsonData, "nearDistance", nearDistance);
		}
	};

	struct ActionScoreWeight : public IJsonConverter {
		float distance = 0.6f;
		float special = 0.4f;
		float unused = 0.2f;

		ActionScoreWeight() { SetName("actionScoreWeight"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("distance", distance)
				.Add("special", special)
				.Add("unused", unused)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "distance", distance);
			fromJson(jsonData, "special", special);
			fromJson(jsonData, "unused", unused);
		}
	};

	struct ActionScore {
		float distanceScore = 0;		// 距離に応じたスコア
		float specialActionScore = 0;	// 特殊行動に応じたスコア
		float unusedBonusScore = 0;		// 使用回数によるスコア

		ActionScore() {};

		float TotalScore(const ActionScoreWeight& weight);

		void Debug() {
			ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "distance    : %f", distanceScore);
			ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "special     : %f", specialActionScore);
			ImGui::TextColored(ImVec4(0.2f, 0.2f, 0.8f, 1.0f), "unusedBonus : %f", unusedBonusScore);
		}
	};

public:

	BossAI() = default;
	~BossAI() = default;

	void Init();
	
	size_t MoveActionAI(const WorldTransform* bossTransform, const Vector3& targetPos);

	void Debug_Gui() override;

private:

	float GetProximityScore(float distance, float targetDistance, float maxDistance);

private:

	Parameter param_;

	ActionScoreWeight weight;

	std::unordered_map<size_t, ActionScore> scoreMap_;

	float distance_;
};

