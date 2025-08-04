
#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Json/IJsonConverter.h"

struct ParticleSingle {
	Vector4 color;			// 色
	Vector3 scale;			// 拡縮
	Quaternion rotate;		// 回転
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
	bool isAddBlend = true;
	bool stretchBillboard;
	float stretchScaleFactor;
	Vector3 upScale;
	bool isBillBord = true;
};

struct ParticleEmit : public IJsonConverter {
	bool isLoop = true;						// Loopをするか
	float duration = 5.0f;					// 継続時間
	Vector4 rotate = Quaternion();			// 回転(Quaternion)
	float rotateAngle = 0;					// 回転量
	Vector3 translate = CVector3::ZERO;		// 位置
	Vector3 direction = CVector3::UP;		// 射出方向
	uint32_t shape = 0;						// emitterの種類(0 = 全方向, 1 = 一方方向)
	uint32_t rateOverTimeCout = 10;			// 射出数
	int emitType = 1;
	int emitOrigin = 0;
	
	// particle自体のparameter
	Vector4 color = Vector4{ 1,1,1,1 };			// 色
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

	bool stretchBillboard = false;	// 生存時間による透明度
	float stretchScale = 4.0f;

	bool isParticleAddBlend = false;	// blendModeをAddBlendにするかのフラグ
	bool isBillBord = true;

	bool isScaleUp;				// サイズを大きくするか
	Vector3 scaleUpScale;

	float radius;

	std::string useTexture = "white.png";
	std::string useMesh = "plane";

	ParticleEmit() {
		toJsonFunction_ = [this](const std::string& id) {
			return this->ToJson(id);
			};
	}

	json ToJson(const std::string& id) const override {
		return JsonBuilder(id)
			.Add("isLoop", isLoop)
			.Add("duration", duration)
			.Add("rotate", rotate)
			.Add("rotateAngle", rotateAngle)
			.Add("translate", translate)
			.Add("direction", direction)
			.Add("shape", shape)
			.Add("rateOverTimeCout", rateOverTimeCout)
			.Add("emitType", emitType)
			.Add("emitOrigin", emitOrigin)
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
			.Add("isParticleAddBlend", isParticleAddBlend)
			.Add("isScaleUp", isScaleUp)
			.Add("scaleUpScale", scaleUpScale)
			.Add("isBillBord", isBillBord)
			.Add("useTexture", useTexture)
			.Add("useMesh", useMesh)
			.Add("radius", radius)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		fromJson(jsonData, "isLoop", isLoop);
		fromJson(jsonData, "duration", duration);
		fromJson(jsonData, "rotate", rotate);
		fromJson(jsonData, "rotateAngle", rotateAngle);
		fromJson(jsonData, "translate", translate);
		fromJson(jsonData, "direction", direction);
		fromJson(jsonData, "shape", shape);
		fromJson(jsonData, "rateOverTimeCout", rateOverTimeCout);
		fromJson(jsonData, "emitType", emitType);
		fromJson(jsonData, "emitOrigin", emitOrigin);
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
		fromJson(jsonData, "isParticleAddBlend", isParticleAddBlend);
		fromJson(jsonData, "isScaleUp", isScaleUp);
		fromJson(jsonData, "scaleUpScale", scaleUpScale);
		fromJson(jsonData, "isBillBord", isBillBord);
		fromJson(jsonData, "useTexture", useTexture);
		fromJson(jsonData, "useMesh", useMesh);
		fromJson(jsonData, "radius", radius);
	}

	void Attribute_Gui();
};
