#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Json/IJsonConverter.h"

struct ParticleSingle {
	Vector4 color;			// 色
	Vector3 scale;			// 拡縮
	Quaternion rotate;			// 回転
	Vector3 translate;		// 座標
	Vector3 velocity;		// 速度
	Vector3 acceleration;	// 加速度
	Vector3 firstScale;		// 初期拡縮
	float lifeTime;			// 生存時間
	float firstLifeTime;	// 初期生存時間
	float currentTime;		// 現在の時間
	float damping;			// 減衰
	float gravity;			// 重力
	bool isLifeOfScale = false;	// 生存時間によるサイズ
	bool isLifeOfAlpha = false;	// 生存時間による透明度
	bool isScaleUpScale = false;	// 生存時間による透明度
	Vector3 upScale;

	//ParticleSingle() {
	//	toJsonFunction_ = [this](const std::string& id) {
	//		return this->ToJson(id);
	//	};
	//}

	//json ToJson(const std::string& id) const override {
	//	return JsonBuilder(id)
	//		.Add("color", color)
	//		.Add("scale", scale)
	//		.Add("rotate", rotate)
	//		.Add("translate", translate)
	//		.Add("velocity", velocity)
	//		.Add("acceleration", acceleration)
	//		.Add("lifeTime", lifeTime)
	//		.Add("currentTime", currentTime)
	//		.Add("damping", damping)
	//		.Add("gravity", gravity)
	//		.Build();
	//}

	//void FromJson(const json& jsonData) override {
	//	fromJson(jsonData, "color", color);
	//	fromJson(jsonData, "scale", scale);
	//	fromJson(jsonData, "rotate", rotate);
	//	fromJson(jsonData, "translate", translate);
	//	fromJson(jsonData, "velocity", velocity);
	//	fromJson(jsonData, "acceleration", acceleration);
	//	fromJson(jsonData, "lifeTime", lifeTime);
	//	fromJson(jsonData, "currentTime", currentTime);
	//	fromJson(jsonData, "damping", damping);
	//	fromJson(jsonData, "gravity", gravity);
	//}
};

struct ParticleEmit : public IJsonConverter {
	Vector4 rotate = Quaternion();			// 回転(Quaternion)
	float rotateAngle = 0;					// 回転量
	Vector3 translate = CVector3::ZERO;		// 位置
	Vector3 direction = CVector3::UP;		// 射出方向
	uint32_t shape = 0;			// emitterの種類(0 = 全方向, 1 = 一方方向)
	uint32_t count = 10;			// 射出数
	float frequency = 5.0f;		// 射出間隔
	float frequencyTime = frequency;	// 時間調整用(実際に動かす時間)
	bool emit = 0;				// 射出許可
	bool isOneShot = false;				// 一度きりか

	// particle自体のparameter
	Vector4 color = Vector4{1,1,1,1};			// 色
	Vector3 minScale = CVector3::UNIT;		// 最小の大きさ
	Vector3 maxScale = CVector3::UNIT;		// 最大の大きさ
	float speed = 1.0f;			// 速度
	float lifeTime = 5.0f;			// particleの生存時間
	float gravity = 0;			// 重力を付与するか
	float dampig = 0;			// 減衰率
	float angleMin = 0.0f;		// 最小の回転
	float angleMax = 360.0f;	// 最大の回転
	bool isDirectionRotate = false;		// 進行方向にParticleを向ける処理
	bool isLifeOfScale = false;	// 生存時間によるサイズ
	bool isLifeOfAlpha = false;	// 生存時間による透明度

	bool isScaleUp;				// サイズを大きくするか
	Vector3 scaleUpScale;

	std::string useTexture = "white.png";

	ParticleEmit() {
		toJsonFunction_ = [this](const std::string& id) {
			return this->ToJson(id);
			};
	}

	json ToJson(const std::string& id) const override {
		return JsonBuilder(id)
			.Add("rotate", rotate)
			.Add("rotateAngle", rotateAngle)
			.Add("translate", translate)
			.Add("direction", direction)
			.Add("shape", shape)
			.Add("count", count)
			.Add("frequency", frequency)
			.Add("frequencyTime", frequencyTime)
			.Add("emit", emit)
			.Add("isOneShot", isOneShot)
			.Add("color", color)
			.Add("minScale", minScale)
			.Add("maxScale", maxScale)
			.Add("speed", speed)
			.Add("lifeTime", lifeTime)
			.Add("gravity", gravity)
			.Add("damping", dampig)  // `damping` の変数名のスペルを修正
			.Add("angleMin", angleMin)
			.Add("angleMax", angleMax)
			.Add("isDirectionRotate", isDirectionRotate)
			.Add("isLifeOfScale", isLifeOfScale)
			.Add("isLifeOfAlpha", isLifeOfAlpha)
			.Add("isScaleUp", isScaleUp)
			.Add("scaleUpScale", scaleUpScale)
			.Add("useTexture", useTexture)  // `damping` の変数名のスペルを修正
			.Build();
	}

	void FromJson(const json& jsonData) override {
		fromJson(jsonData, "rotate", rotate);
		fromJson(jsonData, "rotateAngle", rotateAngle);
		fromJson(jsonData, "translate", translate);
		fromJson(jsonData, "direction", direction);
		fromJson(jsonData, "shape", shape);
		fromJson(jsonData, "count", count);
		fromJson(jsonData, "frequency", frequency);
		fromJson(jsonData, "frequencyTime", frequencyTime);
		fromJson(jsonData, "emit", emit);
		fromJson(jsonData, "isOneShot", isOneShot);
		fromJson(jsonData, "color", color);
		fromJson(jsonData, "minScale", minScale);
		fromJson(jsonData, "maxScale", maxScale);
		fromJson(jsonData, "speed", speed);
		fromJson(jsonData, "lifeTime", lifeTime);
		fromJson(jsonData, "gravity", gravity);
		fromJson(jsonData, "damping", dampig);  // `damping` の変数名のスペルを修正
		fromJson(jsonData, "angleMin", angleMin);
		fromJson(jsonData, "angleMax", angleMax);
		fromJson(jsonData, "isDirectionRotate", isDirectionRotate);
		fromJson(jsonData, "isLifeOfScale", isLifeOfScale);
		fromJson(jsonData, "isLifeOfAlpha", isLifeOfAlpha);
		fromJson(jsonData, "isScaleUp", isScaleUp);
		fromJson(jsonData, "scaleUpScale", scaleUpScale);
		fromJson(jsonData, "useTexture", useTexture);  // `damping` の変数名のスペルを修正
	}

#ifdef _DEBUG
	void Attribute_Gui();
#endif
};

