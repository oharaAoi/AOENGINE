#pragma once

// engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Game/Actor/Common/ScreenWorldPlaneAnchor.h"

/// <summary>
/// DamageFloorの画面固定位置と当たり判定の調整値。
/// </summary>
struct DamageFloorParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {
	Math::Vector2 viewportAnchor{ 0.5f, 0.95f };
	float worldZ = 0.0f;
	float damage = 1.0f;
	Math::Vector3 hitSize{ 20.0f, 1.0f, 2.0f };

	DamageFloorParameter() : CustomParameterSet("Damage Floor") {
		SetGroupName("DamageFloor");
		SetName("damageFloorParameter");
		AddParameter("Viewport Anchor", viewportAnchor, 0.01f, 0.0f, 1.0f);
		AddParameter("World Z", worldZ, 0.1f);
		AddParameter("Damage", damage, 1.0f, 0.0f, 10000.0f);
		AddParameter("Hit Size (half)", hitSize, 0.1f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("viewportAnchor", viewportAnchor)
			.Add("worldZ", worldZ)
			.Add("damage", damage)
			.Add("hitSize", hitSize)
			.Build();
	}

	void FromJson(const json& data) override {
		Convert::fromJson(data, "viewportAnchor", viewportAnchor);
		Convert::fromJson(data, "worldZ", worldZ);
		Convert::fromJson(data, "damage", damage);
		Convert::fromJson(data, "hitSize", hitSize);
	}
};

/// <summary>
/// damageをプレイヤーに与える床
/// </summary>
class DamageFloor : 
	public AOENGINE::BaseEntity {
public: // constructor

	DamageFloor() = default;
	~DamageFloor() override = default;

public: // public method

	// 初期化、更新
	void Init(AOENGINE::BaseGameObject* body);
	void Update(const Math::Matrix4x4& viewProjection);

public: // accessor

	float GetDamage() const { return parameter_.damage; }
	const Math::Vector3& GetPosition() const { return position_; }

private:

	DamageFloorParameter parameter_;
	ScreenWorldPlaneAnchor screenAnchor_;
	Math::Vector3 position_{};
};

