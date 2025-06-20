#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/State/ICharacterState.h"
#include "Game/Actor/Weapon/PulseArmor.h"

class Boss;

/// <summary>
/// Bossがバリアを展開している状態
/// </summary>
class BossStateDeployArmor :
	public ICharacterState<Boss> {
public:

	struct ArmorParameter : public IJsonConverter {
		float durability = 100.0f;
		Vector3 scale = CVector3::UNIT;
		Vector4 color = Vector4(CVector3::UNIT, 1.0f);
		Vector4 edgeColor = Vector4(CVector3::UNIT, 1.0f);

		Vector3 uvScale;
		Vector3 uvRotate;
		Vector3 uvTranslate;

		ArmorParameter() { SetName("BossArmorParameter"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("durability", durability)
				.Add("scale", scale)
				.Add("color", color)
				.Add("edgeColor", edgeColor)
				.Add("uvScale", uvScale)
				.Add("uvRotate", uvRotate)
				.Add("uvTranslate", uvTranslate)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "durability", durability);
			fromJson(jsonData, "scale", scale);
			fromJson(jsonData, "color", color);
			fromJson(jsonData, "edgeColor", edgeColor);
			fromJson(jsonData, "uvScale", uvScale);
			fromJson(jsonData, "uvRotate", uvRotate);
			fromJson(jsonData, "uvTranslate", uvTranslate);
		}
	};

public:

	BossStateDeployArmor() = default;
	~BossStateDeployArmor() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnExit() override;


#ifdef _DEBUG
	void Debug_Gui() override;
#endif

private:

	ArmorParameter armorParam_;

	PulseArmor* armor_;

};

