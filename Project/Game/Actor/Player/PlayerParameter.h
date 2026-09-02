#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

/// <summary>
/// プレイヤーの調整パラメータ
/// </summary>
struct PlayerParameter :
	public AOENGINE::CustomParameterSet,
	public AOENGINE::IJsonConverter {

	// 左右移動
	float moveSpeed;			// 左右移動スピード
	float moveInputThreshold;	// 向き変更とみなす入力の下限

	// ジャンプ
	float jumpPower;	    // ジャンプの強さ
	float hangTime;		// 滞空時間
	float riseGravity;	// 上昇中の減速度
	float fallGravity;	// 落下中の重力
	float maxFallSpeed;	// 落下速度の上限

	// その他
	float groundHeight;	// 仮の地面の高さ
	float stickDeadZone;	// 左スティックのデッドゾーン

	PlayerParameter() : CustomParameterSet("Player") {
		SetGroupName("Player");
		SetName("playerParameter");

		AddParameter("Move Speed", moveSpeed, 0.1f, 0.0f, 100.0f);
		AddParameter("Move Input Threshold", moveInputThreshold, 0.001f, 0.0f, 1.0f);
		AddParameter("Jump Power", jumpPower, 0.1f, 0.0f, 200.0f);
		AddParameter("Hang Time", hangTime, 0.01f, 0.0f, 5.0f);
		AddParameter("Rise Gravity", riseGravity, 0.1f, 0.0f, 500.0f);
		AddParameter("Fall Gravity", fallGravity, 0.1f, 0.0f, 500.0f);
		AddParameter("Max Fall Speed", maxFallSpeed, 0.1f, 0.0f, 500.0f);
		AddParameter("Ground Height", groundHeight, 0.1f);
		AddParameter("Stick DeadZone", stickDeadZone, 0.01f, 0.0f, 1.0f);
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("moveSpeed", moveSpeed)
			.Add("moveInputThreshold", moveInputThreshold)
			.Add("jumpPower", jumpPower)
			.Add("hangTime", hangTime)
			.Add("riseGravity", riseGravity)
			.Add("fallGravity", fallGravity)
			.Add("maxFallSpeed", maxFallSpeed)
			.Add("groundHeight", groundHeight)
			.Add("stickDeadZone", stickDeadZone)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "moveSpeed", moveSpeed);
		Convert::fromJson(jsonData, "moveInputThreshold", moveInputThreshold);
		Convert::fromJson(jsonData, "jumpPower", jumpPower);
		Convert::fromJson(jsonData, "hangTime", hangTime);
		Convert::fromJson(jsonData, "riseGravity", riseGravity);
		Convert::fromJson(jsonData, "fallGravity", fallGravity);
		Convert::fromJson(jsonData, "maxFallSpeed", maxFallSpeed);
		Convert::fromJson(jsonData, "groundHeight", groundHeight);
		Convert::fromJson(jsonData, "stickDeadZone", stickDeadZone);
	}
};
