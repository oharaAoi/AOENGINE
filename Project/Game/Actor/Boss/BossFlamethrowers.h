#pragma once
// c++
#include <memory>
// engine
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"
// game
#include "Game/Actor/Weapon/Flamethrower.h"
#include "Game/Manager/BaseBulletManager.h"

enum class BossFlamethrowersType {
	Left,
	Right
};

class BossFlamethrowers :
	public AOENGINE::AttributeGui {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		Math::QuaternionSRT flamethrowerSRT;
		float radius;
		float angle;
		bool clockwise;

		Parameter() {
			SetGroupName("Boss");
			SetName("BossFlamethrower");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("flamethrowerSRT", flamethrowerSRT)
				.Add("radius", radius)
				.Add("angle", angle)
				.Add("clockwise", clockwise)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "flamethrowerSRT", flamethrowerSRT);
			Convert::fromJson(jsonData, "radius", radius);
			Convert::fromJson(jsonData, "angle", angle);
			Convert::fromJson(jsonData, "clockwise", clockwise);
		}

		void Debug_Gui() override;
	};

public:

	BossFlamethrowers() = default;
	~BossFlamethrowers() = default;

public:

	// 初期化
	void Init(AOENGINE::WorldTransform* transform);
	// 更新
	void Update();
	// 編集
	void Debug_Gui() override;

public: // accessor

	void SetBulletManager(BaseBulletManager* manager);

private:

	static const uint32_t kFlamethrowerCount_ = 2;

	Parameter param_[kFlamethrowerCount_];

	// 武器
	std::unique_ptr<Flamethrower> flamethrowers_[kFlamethrowerCount_];
};

