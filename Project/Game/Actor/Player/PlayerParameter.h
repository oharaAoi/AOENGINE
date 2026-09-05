#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/Editor/Parameter/CustomParameter.h"
#include "Engine/Lib/Math/Vector3.h"

/// <summary>
/// プレイヤーの調整パラメータ
/// </summary>
struct PlayerParameter : public AOENGINE::CustomParameterSet,
						 public AOENGINE::IJsonConverter
{

	// 左右移動
	float moveSpeed;		  // 左右移動の最大速度
	float moveAcceleration;  // 左右移動の加速度
	float moveInputThreshold; // 向き変更とみなす入力の下限

	// ジャンプ
	float jumpPower;			  // ジャンプの強さ
	float hangTime;				  // 滞空時間
	float riseGravity;			  // 上昇中の減速度
	float fallGravity;			  // 落下中の重力
	float maxFallSpeed;			  // 落下速度の上限
	float groundKeepSpeed = 1.0f; // 接地を維持するために足場へ押し付ける速度
	float fallLimitY = -10.0f;	  // これ以上は落ちない高さ。下回ったらこの位置で止める
	float fixedZ = 0.0f;		  // 奥行きは動かさない。押し戻されてもこの位置へ戻す
	float groundCheckDistance = 0.15f; // 足元の何ユニット下までを足場として見るか

	// コネクトステート
	// json に無いキーは値が書き換わらないため、初期値を持たせておく
	float connectableTime = 1.0f; // 接続可能な時間
	float launchWaitTime = 1.5f;  // 集合を始めてから自動で打ち上げるまでの時間
	float gatherSpeed = 12.0f;	  // ブロックグループが集合地点へ向かう速さ
	float launchSpeed = 20.0f;	  // 打ち上げの初速
	float launchAccel = 20.0f;	  // 打ち上げ中の加速度
	float launchLifeTime = 3.0f;  // 打ち上げてから制御を手放すまでの時間
	float gatherBlockSize = 1.0f;		// 集合中の押し戻し判定に使うブロック1個の大きさ
	float gatherSeparationSpeed = 8.0f;	// 集合中にグループ同士が押し戻しで離れていく速さ

	// 被弾
	float maxHp = 3.0f;			// 最大HP
	float invincibleTime = 1.0f;			// 被弾後の無敵時間(秒)
	float invincibleBlinkInterval = 0.08f;	// 無敵中の点滅の間隔(秒)

	// 見た目基準サイズ
	Math::Vector3 baseScale{ 0.4f, 0.4f, 0.4f };
    // ヒットサイズ
	Math::Vector3 hitSize{ 0.6f, 0.9f, 0.6f };
	// 当たり判定オフセット
	Math::Vector3 hitOffset{ 0.0f, -0.45f, 0.0f };

	// 向き
	float facingYawRight = 180.0f;
	float facingYawLeft = 0.0f;
	float facingTurnSpeed = 12.0f;

	// アニメーション
	float animationBlendSpeed = 0.2f;
	float animationSpeed = 1.0f;		// 既定の再生速度
	float walkAnimationSpeed = 1.6f;	// 歩きだけ少し速く回す
	float idleAnimationDelay = 0.12f;

	// その他
	float stickDeadZone; // 左スティックのデッドゾーン

	PlayerParameter() : CustomParameterSet("Player")
	{
		SetGroupName("Player");
		SetName("playerParameter");

		AddParameter("Move Speed", moveSpeed, 0.1f, 0.0f, 100.0f);
		AddParameter("Move Acceleration", moveAcceleration, 0.1f, 0.0f, 1000.0f);
		AddParameter("Move Input Threshold", moveInputThreshold, 0.001f, 0.0f, 1.0f);
		AddParameter("Jump Power", jumpPower, 0.1f, 0.0f, 200.0f);
		AddParameter("Hang Time", hangTime, 0.01f, 0.0f, 5.0f);
		AddParameter("Rise Gravity", riseGravity, 0.1f, 0.0f, 500.0f);
		AddParameter("Fall Gravity", fallGravity, 0.1f, 0.0f, 500.0f);
		AddParameter("Max Fall Speed", maxFallSpeed, 0.1f, 0.0f, 500.0f);
		AddParameter("Ground Keep Speed",groundKeepSpeed,0.1f,0.0f,100.0f);
		AddParameter("Fall Limit Y", fallLimitY, 0.1f, -10000.0f, 10000.0f);
		AddParameter("Fixed Z", fixedZ, 0.1f, -10000.0f, 10000.0f);
		AddParameter("Ground Check Distance", groundCheckDistance, 0.01f, 0.0f, 10.0f);

		AddParameter("Connectable Time",connectableTime,0.01f,0.0f,10.0f);
		AddParameter("Launch Wait Time",launchWaitTime,0.01f,0.0f,10.0f);
		AddParameter("Gather Speed",gatherSpeed,0.1f,0.0f,100.0f);
		AddParameter("Launch Speed",launchSpeed,0.1f,0.0f,100.0f);
		AddParameter("Launch Accel",launchAccel,0.1f,0.0f,100.0f);
		AddParameter("Launch Life Time",launchLifeTime,0.01f,0.0f,10.0f);
		AddParameter("Gather Block Size",gatherBlockSize,0.01f,0.0f,10.0f);
		AddParameter("Gather Separation Speed",gatherSeparationSpeed,0.1f,0.0f,100.0f);

		AddParameter("Max HP", maxHp, 1.0f, 0.0f, 1000.0f);
		AddParameter("Invincible Time", invincibleTime, 0.01f, 0.0f, 10.0f);
		AddParameter("Invincible Blink Interval", invincibleBlinkInterval, 0.01f, 0.0f, 5.0f);

		AddSeparatorText("Look");
		AddParameter("Base Scale", baseScale, 0.01f);
		AddParameter("Hit Size", hitSize, 0.01f);
		AddParameter("Hit Offset", hitOffset, 0.01f);
		AddParameter("Facing Yaw Right(deg)", facingYawRight, 1.0f, -360.0f, 360.0f);
		AddParameter("Facing Yaw Left(deg)", facingYawLeft, 1.0f, -360.0f, 360.0f);
		AddParameter("Facing Turn Speed", facingTurnSpeed, 0.1f, 0.0f, 100.0f);
		AddParameter("Animation Blend Speed", animationBlendSpeed, 0.01f, 0.0f, 10.0f);
		AddParameter("Animation Speed", animationSpeed, 0.01f, 0.0f, 10.0f);
		AddParameter("Walk Animation Speed", walkAnimationSpeed, 0.01f, 0.0f, 10.0f);
		AddParameter("Idle Animation Delay", idleAnimationDelay, 0.01f, 0.0f, 5.0f);

		AddParameter("Stick DeadZone", stickDeadZone, 0.01f, 0.0f, 1.0f);
	}

	json ToJson(const std::string &id) const override
	{
		return AOENGINE::JsonBuilder(id)
			.Add("moveSpeed", moveSpeed)
			.Add("moveAcceleration", moveAcceleration)
			.Add("moveInputThreshold", moveInputThreshold)
			.Add("jumpPower", jumpPower)
			.Add("hangTime", hangTime)
			.Add("riseGravity", riseGravity)
			.Add("fallGravity", fallGravity)
			.Add("maxFallSpeed", maxFallSpeed)
			.Add("groundKeepSpeed", groundKeepSpeed)
			.Add("fallLimitY", fallLimitY)
			.Add("fixedZ", fixedZ)
			.Add("groundCheckDistance", groundCheckDistance)
			.Add("connectableTime", connectableTime)
			.Add("launchWaitTime", launchWaitTime)
			.Add("gatherSpeed", gatherSpeed)
			.Add("launchSpeed", launchSpeed)
			.Add("launchAccel", launchAccel)
			.Add("launchLifeTime", launchLifeTime)
			.Add("gatherBlockSize", gatherBlockSize)
			.Add("gatherSeparationSpeed", gatherSeparationSpeed)
			.Add("maxHp", maxHp)
			.Add("invincibleTime", invincibleTime)
			.Add("invincibleBlinkInterval", invincibleBlinkInterval)
			.Add("baseScale", baseScale)
			.Add("hitSize", hitSize)
			.Add("hitOffset", hitOffset)
			.Add("facingYawRight", facingYawRight)
			.Add("facingYawLeft", facingYawLeft)
			.Add("facingTurnSpeed", facingTurnSpeed)
			.Add("animationBlendSpeed", animationBlendSpeed)
			.Add("animationSpeed", animationSpeed)
			.Add("walkAnimationSpeed", walkAnimationSpeed)
			.Add("idleAnimationDelay", idleAnimationDelay)
			.Add("stickDeadZone", stickDeadZone)
			.Build();
	}

	void FromJson(const json &jsonData) override
	{
		Convert::fromJson(jsonData, "moveSpeed", moveSpeed);
		Convert::fromJson(jsonData, "moveAcceleration", moveAcceleration);
		Convert::fromJson(jsonData, "moveInputThreshold", moveInputThreshold);
		Convert::fromJson(jsonData, "jumpPower", jumpPower);
		Convert::fromJson(jsonData, "hangTime", hangTime);
		Convert::fromJson(jsonData, "riseGravity", riseGravity);
		Convert::fromJson(jsonData, "fallGravity", fallGravity);
		Convert::fromJson(jsonData, "maxFallSpeed", maxFallSpeed);
		Convert::fromJson(jsonData, "groundKeepSpeed", groundKeepSpeed);
		Convert::fromJson(jsonData, "fallLimitY", fallLimitY);
		Convert::fromJson(jsonData, "fixedZ", fixedZ);
		Convert::fromJson(jsonData, "groundCheckDistance", groundCheckDistance);
		Convert::fromJson(jsonData, "connectableTime", connectableTime);
		Convert::fromJson(jsonData, "launchWaitTime", launchWaitTime);
		Convert::fromJson(jsonData, "gatherSpeed", gatherSpeed);
		Convert::fromJson(jsonData, "launchSpeed", launchSpeed);
		Convert::fromJson(jsonData, "launchAccel", launchAccel);
		Convert::fromJson(jsonData, "launchLifeTime", launchLifeTime);
		Convert::fromJson(jsonData, "gatherBlockSize", gatherBlockSize);
		Convert::fromJson(jsonData, "gatherSeparationSpeed", gatherSeparationSpeed);
		Convert::fromJson(jsonData, "maxHp", maxHp);
		Convert::fromJson(jsonData, "invincibleTime", invincibleTime);
		Convert::fromJson(jsonData, "invincibleBlinkInterval", invincibleBlinkInterval);
		Convert::fromJson(jsonData, "baseScale", baseScale);
		Convert::fromJson(jsonData, "hitSize", hitSize);
		Convert::fromJson(jsonData, "hitOffset", hitOffset);
		Convert::fromJson(jsonData, "facingYawRight", facingYawRight);
		Convert::fromJson(jsonData, "facingYawLeft", facingYawLeft);
		Convert::fromJson(jsonData, "facingTurnSpeed", facingTurnSpeed);
		Convert::fromJson(jsonData, "animationBlendSpeed", animationBlendSpeed);
		Convert::fromJson(jsonData, "animationSpeed", animationSpeed);
		Convert::fromJson(jsonData, "walkAnimationSpeed", walkAnimationSpeed);
		Convert::fromJson(jsonData, "idleAnimationDelay", idleAnimationDelay);
		Convert::fromJson(jsonData, "stickDeadZone", stickDeadZone);
	}
};
