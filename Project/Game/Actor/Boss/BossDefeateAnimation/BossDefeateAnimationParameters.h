#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

struct BossExplosionParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	int explosionCount = 6;					// 爆破回数
	float explosionRange = 30.f;			// 爆発範囲
	float exlosionCoolTime = 0.5f;			// 爆破のクールタイム
	float finalExlosionTime = 0.5f;			// 最後の爆発までの時間
	
	BossExplosionParameter() : CustomParameterSet("BossExplosionParameter") {
		SetGroupName("BossDefeatAnimatinoParameter");
		SetName("BossExplosionParameter");

		AddParameter("爆破回数", explosionCount);
		AddParameter("爆発範囲", explosionRange);
		AddParameter("爆発のクールタイム", exlosionCoolTime);
		AddParameter("最後の爆発までの時間", finalExlosionTime);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("explosionCount", explosionCount)
			.Add("explosionRange", explosionRange)
			.Add("exlosionCoolTime", exlosionCoolTime)
			.Add("finalExlosionTime", finalExlosionTime)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "explosionCount", explosionCount);
		Convert::fromJson(jsonData, "explosionRange", explosionRange);
		Convert::fromJson(jsonData, "exlosionCoolTime", exlosionCoolTime);
		Convert::fromJson(jsonData, "finalExlosionTime", finalExlosionTime);
	}
};

struct BossKnockOutParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	float knockOutSpeed = 10.f;							// 爆破回数
	Math::Vector3 knockOutDirection = Math::Vector3();	// ぶっ飛ばし方向
	float knockOutTime = 2.f;							// ぶっ飛ばす時間
	float knockOutRotateSpeed = 5.f;						// ぶっ飛ばし回転速度


	BossKnockOutParameter() : CustomParameterSet("BossKnockOutParameter") {
		SetGroupName("BossDefeatAnimatinoParameter");
		SetName("BossKnockOutParameter");

		AddParameter("ぶっ飛ばし速度", knockOutSpeed);
		AddParameter("ぶっ飛ばし方向", knockOutDirection);
		AddParameter("ぶっ飛ばす時間", knockOutTime);
		AddParameter("ぶっ飛ばし回転速度", knockOutRotateSpeed);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("knockOutSpeed", knockOutSpeed)
			.Add("knockOutDirection", knockOutDirection)
			.Add("knockOutTime", knockOutTime)
			.Add("knockOutRotateSpeed", knockOutRotateSpeed)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "knockOutSpeed", knockOutSpeed);
		Convert::fromJson(jsonData, "knockOutDirection", knockOutDirection);
		Convert::fromJson(jsonData, "knockOutTime", knockOutTime);
		Convert::fromJson(jsonData, "knockOutRotateSpeed", knockOutRotateSpeed);
	}
};

