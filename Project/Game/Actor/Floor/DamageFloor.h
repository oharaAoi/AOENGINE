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
/// DamageFloorの画面固定位置と当たり判定の調整値
/// </summary>
struct DamageFloorParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {
	Math::Vector2 viewportAnchor{ 0.5f, 0.95f };
	float worldZ = 0.0f;
	float damage = 1.0f;
	Math::Vector3 hitSize{ 20.0f, 1.0f, 2.0f };
	float knockbackPower = 40.0f; 

	// 画面上の位置へ追いつくまでの遅れ。0に近いほどカメラにぴったり付いてくる
	float followSmoothTime = 0.35f;
	// 追いつく時の最大速度。遅れを取り戻す速さの上限
	float followMaxSpeed = 100.0f;

	DamageFloorParameter() : CustomParameterSet("Damage Floor") {
		SetGroupName("DamageFloor");
		SetName("damageFloorParameter");
		AddParameter("Viewport Anchor", viewportAnchor, 0.01f, 0.0f, 1.0f);
		AddParameter("World Z", worldZ, 0.1f);
		AddParameter("Damage", damage, 1.0f, 0.0f, 10000.0f);
		AddParameter("Hit Size (half)", hitSize, 0.1f);
		AddParameter("Knockback Power", knockbackPower, 0.1f, 0.0f, 1000.0f);
		AddParameter("Follow Smooth Time", followSmoothTime, 0.01f, 0.0f, 5.0f);
		AddParameter("Follow Max Speed", followMaxSpeed, 1.0f, 0.0f, 100000.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("viewportAnchor", viewportAnchor)
			.Add("worldZ", worldZ)
			.Add("damage", damage)
			.Add("hitSize", hitSize)
			.Add("knockbackPower", knockbackPower)
			.Add("followSmoothTime", followSmoothTime)
			.Add("followMaxSpeed", followMaxSpeed)
			.Build();
	}

	void FromJson(const json& data) override {
		Convert::fromJson(data, "viewportAnchor", viewportAnchor);
		Convert::fromJson(data, "worldZ", worldZ);
		Convert::fromJson(data, "damage", damage);
		Convert::fromJson(data, "hitSize", hitSize);
		Convert::fromJson(data, "knockbackPower", knockbackPower);
		Convert::fromJson(data, "followSmoothTime", followSmoothTime);
		Convert::fromJson(data, "followMaxSpeed", followMaxSpeed);
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
	float GetKnockbackPower() const { return parameter_.knockbackPower; }
	const Math::Vector3& GetPosition() const { return position_; }

private:

	DamageFloorParameter parameter_;
	ScreenWorldPlaneAnchor screenAnchor_;

	// 実際に置いている位置。画面上の位置へ遅れて付いていく
	Math::Vector3 position_{};
	// 追従のイージングが持ち越す速度
	Math::Vector3 followVelocity_{};
	// 最初の1回だけは遅れずにその場へ置く
	bool hasFollowStarted_ = false;
};

