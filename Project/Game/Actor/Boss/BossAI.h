#pragma once
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

public:

	BossAI() = default;
	~BossAI() = default;

	void Init();
	
	size_t MoveActionAI(const WorldTransform* bossTransform, const Vector3& targetPos);

	void Debug_Gui() override;

private:

	Parameter param_;

};

