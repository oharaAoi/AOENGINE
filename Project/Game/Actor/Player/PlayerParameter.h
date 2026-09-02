#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// プレイヤーの調整パラメータ
/// </summary>
struct PlayerParameter : public AOENGINE::IJsonConverter {

	// 左右移動
	float moveSpeed;				// 左右移動スピード
	float moveInputThreshold;	// 向き変更とみなす入力の下限

	// ジャンプ
	float jumpPower;	    // ジャンプの強さ
	float hangTime;			// 滞空時間
	float riseGravity;		// 上昇中の減速度
	float fallGravity;		// 落下中の重力
	float maxFallSpeed;		// 落下速度の上限

	// その他
	float groundHeight;		// 仮の地面の高さ
	float stickDeadZone;    // 左スティックのデッドゾーン

	PlayerParameter() {
		SetGroupName("Player");
		SetName("playerParameter");
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

	void Debug_Gui() override;
};
