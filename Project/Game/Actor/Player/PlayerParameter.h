#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"

/// <summary>
/// プレイヤーの調整項目
/// </summary>
struct PlayerParameter : public AOENGINE::CustomParameterSet {
public:

	// ------------------------------------------------------------
	// 左右移動
	// ------------------------------------------------------------
	float moveSpeed = 6.0f;       

	// ------------------------------------------------------------
	// ジャンプ
	// ------------------------------------------------------------
	float jumpPower = 18.0f;       // ジャンプの強さ
	float hangTime = 0.35f;        // 滞空時間
	float riseGravity = 55.0f;     // 上昇中の減速度
	float fallGravity = 30.0f;     // 落下中の重力
	float maxFallSpeed = 45.0f;    // 落下速度の上限

	// ------------------------------------------------------------
	// 打ち上げ
	// ------------------------------------------------------------
	float launchPower = 30.0f;     // 打ち上げの初速
	float launchDuration = 0.2f;   // 打ち上げ状態とみなす時間

	// ------------------------------------------------------------
	// その他
	// ------------------------------------------------------------
	float groundHeight = 0.0f;     // 仮の地面の高さ
	float stickDeadZone = 0.2f;    // 左スティックのデッドゾーン

	PlayerParameter() : CustomParameterSet("Player") {
		AddParameter("Move Speed", moveSpeed, 0.1f, 0.0f, 100.0f);

		AddParameter("Jump Power", jumpPower, 0.1f, 0.0f, 200.0f);
		AddParameter("Hang Time", hangTime, 0.01f, 0.0f, 5.0f);
		AddParameter("Rise Gravity", riseGravity, 0.1f, 0.0f, 500.0f);
		AddParameter("Fall Gravity", fallGravity, 0.1f, 0.0f, 500.0f);
		AddParameter("Max Fall Speed", maxFallSpeed, 0.1f, 0.0f, 500.0f);

		AddParameter("Launch Power", launchPower, 0.1f, 0.0f, 200.0f);
		AddParameter("Launch Duration", launchDuration, 0.01f, 0.0f, 5.0f);

		AddParameter("Ground Height", groundHeight, 0.1f);
		AddParameter("Stick DeadZone", stickDeadZone, 0.01f, 0.0f, 1.0f);
	}
};
