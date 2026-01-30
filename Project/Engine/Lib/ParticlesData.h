#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Json/IJsonConverter.h"

enum class CpuEmitDirection {
	Up,			// 上方向に射出
	Random,		// ランダム方向に射出
	Outside,	// 中心から反対に射出
	CenterFor	// 中心に向かって射出
};

enum class CpuEmitOrigin {
	Center,		// 中心から
	Range,		// 範囲からランダムに
};

enum class CpuEmitterShape {
	Shpere,
	Box,
	Cone
};

namespace AOENGINE {

/// <summary>
/// Particle単体のパラメータ
/// </summary>
struct ParticleSingle {
	AOENGINE::Color color = AOENGINE::Color();			// 色
	Math::Vector3 scale = CVector3::ZERO;			// 拡縮
	Math::Quaternion rotate = Math::Quaternion();		// 回転
	Math::Vector3 translate = CVector3::ZERO;		// 座標
	Math::Vector3 velocity = CVector3::ZERO;		// 速度
	Math::Vector3 acceleration = CVector3::ZERO;	// 加速度
	Math::Vector3 firstScale = CVector3::ZERO;		// 初期拡縮
	Math::Vector3 emitterCenter = CVector3::ZERO;	// Emitterの中心
	float lifeTime = 0;			// 生存時間
	float currentTime = 0;		// 現在の時間
	float damping = 0;			// 減衰
	float gravity = 0;			// 重力
	bool isLerpDiscardValue = false;		// discardの値をlerpさせるか
	float discardValue = 0.01f;		// discardをするしきい値
	float startDiscard = 0.01f;		// discardの初期値
	float endDiscard = 1.0f;		// discardの終了値
	bool isLifeOfScale = false;	// 生存時間によるサイズ
	Math::Vector3 lifeOfMinScale = CVector3::ZERO;
	Math::Vector3 lifeOfMaxScale = CVector3::ZERO;
	bool isLifeOfAlpha = false;	// 生存時間による透明度
	bool isFadeInOut = false;
	float fadeInTime = 0.0f;
	float fadeOutTime = 0.0f;
	float initAlpha_ = 0.0f;
	bool isScaleUpScale = false;	// 生存時間による透明度
	bool isStretch = false;
	bool isAddBlend = true;
	Math::Vector3 upScale = CVector3::ZERO;
	bool isBillBord = true;
	bool isDraw2d = true;
	bool isCenterFor = false;

	bool isTextureAnimation = false;
	Math::Vector2 tileSize = CMath::Vector2::UNIT;
	Math::Matrix4x4 uvMat;

	bool isColorAnimation = false;
	AOENGINE::Color preColor = AOENGINE::Color();
	AOENGINE::Color postColor = AOENGINE::Color();

	float rotateZ = 0;
};

/// <summary>
/// Particleのエミッターのパラメータ
/// </summary>
struct ParticleEmit : public AOENGINE::IJsonConverter {
	bool isLoop = true;						// Loopをするか
	float duration = 5.0f;					// 継続時間
	Math::Vector3 translate = CVector3::ZERO;		// 位置
	Math::Vector3 preTranslate = CVector3::ZERO;		// 位置
	Math::Vector3 rotate = CVector3::ZERO;		// 射出方向
	uint32_t rateOverTimeCout = 10;			// 射出数
	int shape = 0;						// emitterの種類
	int emitDirection = 1;
	int emitOrigin = 0;

	// particle自体のparameter
	bool isRandomColor = false;
	AOENGINE::Color color = AOENGINE::Color{ 1,1,1,1 };			// 色
	AOENGINE::Color randColor1 = AOENGINE::Color{ 1,0,0,1 };	// 色
	AOENGINE::Color randColor2 = AOENGINE::Color{ 0,0,1,1 };	// 色
	bool isLerpDiscardValue;					// discardの値をlerpさせるか
	float discardValue = 0.01f;					// discardをするしきい値
	float startDiscard = 0.01f;					// discardの初期値
	float endDiscard = 1.0f;					// discardの終了値
	bool separateByAxisScale = false;
	Math::Vector3 minScale = CVector3::UNIT;			// 最小の大きさ
	Math::Vector3 maxScale = CVector3::UNIT;			// 最大の大きさ
	float speed = 1.0f;							// 速度
	float lifeTime = 5.0f;						// particleの生存時間
	float gravity = 0;							// 重力を付与するか
	float dampig = 0;							// 減衰率
	float angleMin = 0.0f;						// 最小の回転
	float angleMax = 360.0f;					// 最大の回転
	bool isDirectionRotate = false;				// 進行方向にParticleを向ける処理
	bool isLifeOfScale = false;					// 生存時間によるサイズ
	Math::Vector3 lifeOfMinScale = CVector3::UNIT;
	Math::Vector3 lifeOfMaxScale = CVector3::ZERO;
	bool isLifeOfAlpha = false;					// 生存時間による透明度
	bool isFadeInOut = false;					// FadeInOutで出現
	float fadeInTime = 0;
	float fadeOutTime = 0;

	bool isStretch = false;
	bool isParticleAddBlend = false;			// blendModeをAddBlendにするかのフラグ
	bool isDraw2d = false;						// blendModeをAddBlendにするかのフラグ
	bool isBillBord = true;

	bool isScaleUp;								// サイズを大きくするか
	Math::Vector3 scaleUpScale;

	float radius = 0.5f;
	float angle = 27.f;
	float height = 1;
	Math::Vector3 size = CVector3::UNIT;

	std::string useTexture = "circle.png";
	std::string useMesh = "plane";

	bool isTextureSheetAnimation = false;
	Math::Vector2 tiles = CMath::Vector2::UNIT;

	bool isColorAnimation = false;
	AOENGINE::Color preColor;
	AOENGINE::Color postColor;

	bool isRandomRotate = true;
	float minAngle = 0;
	float maxAngle = 360;

	ParticleEmit() {
		toJsonFunction_ = [this](const std::string& id) {
			return this->ToJson(id);
			};
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("isLoop", isLoop)
			.Add("duration", duration)
			.Add("rotate", rotate)
			.Add("translate", translate)
			.Add("shape", shape)
			.Add("rateOverTimeCout", rateOverTimeCout)
			.Add("emitDirection", emitDirection)
			.Add("emitOrigin", emitOrigin)
			.Add("color", color)
			.Add("isRandomColor", isRandomColor)
			.Add("randColor1", randColor1)
			.Add("randColor2", randColor2)
			.Add("isLerpDiscardValue", isLerpDiscardValue)
			.Add("discardValue", discardValue)
			.Add("startDiscard", startDiscard)
			.Add("endDiscard", endDiscard)
			.Add("separateByAxisScale", separateByAxisScale)
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
			.Add("lifeOfMinScale", lifeOfMinScale)
			.Add("lifeOfMaxScale", lifeOfMaxScale)
			.Add("isLifeOfAlpha", isLifeOfAlpha)
			.Add("isFadeInOut", isFadeInOut)
			.Add("fadeInTime", fadeInTime)
			.Add("fadeOutTime", fadeOutTime)
			.Add("isStretch", isStretch)
			.Add("isParticleAddBlend", isParticleAddBlend)
			.Add("isDraw2d", isDraw2d)
			.Add("isScaleUp", isScaleUp)
			.Add("scaleUpScale", scaleUpScale)
			.Add("isBillBord", isBillBord)
			.Add("useTexture", useTexture)
			.Add("useMesh", useMesh)
			.Add("radius", radius)
			.Add("size", size)
			.Add("angle", angle)
			.Add("height", height)
			.Add("isTextureSheetAnimation", isTextureSheetAnimation)
			.Add("tiles", tiles)
			.Add("isColorAnimation", isColorAnimation)
			.Add("preColor", preColor)
			.Add("postColor", postColor)
			.Add("isRandomRotate", isRandomRotate)
			.Add("minAngle", minAngle)
			.Add("maxAngle", maxAngle)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "isLoop", isLoop);
		Convert::fromJson(jsonData, "duration", duration);
		Convert::fromJson(jsonData, "rotate", rotate);
		Convert::fromJson(jsonData, "translate", translate);
		Convert::fromJson(jsonData, "shape", shape);
		Convert::fromJson(jsonData, "rateOverTimeCout", rateOverTimeCout);
		Convert::fromJson(jsonData, "emitDirection", emitDirection);
		Convert::fromJson(jsonData, "emitOrigin", emitOrigin);
		Convert::fromJson(jsonData, "color", color);
		Convert::fromJson(jsonData, "isRandomColor", isRandomColor);
		Convert::fromJson(jsonData, "randColor1", randColor1);
		Convert::fromJson(jsonData, "randColor2", randColor2);
		Convert::fromJson(jsonData, "isLerpDiscardValue", isLerpDiscardValue);
		Convert::fromJson(jsonData, "discardValue", discardValue);
		Convert::fromJson(jsonData, "startDiscard", startDiscard);
		Convert::fromJson(jsonData, "endDiscard", endDiscard);
		Convert::fromJson(jsonData, "separateByAxisScale", separateByAxisScale);
		Convert::fromJson(jsonData, "minScale", minScale);
		Convert::fromJson(jsonData, "maxScale", maxScale);
		Convert::fromJson(jsonData, "speed", speed);
		Convert::fromJson(jsonData, "lifeTime", lifeTime);
		Convert::fromJson(jsonData, "gravity", gravity);
		Convert::fromJson(jsonData, "damping", dampig);  // `damping` の変数名のスペルを修正
		Convert::fromJson(jsonData, "angleMin", angleMin);
		Convert::fromJson(jsonData, "angleMax", angleMax);
		Convert::fromJson(jsonData, "isDirectionRotate", isDirectionRotate);
		Convert::fromJson(jsonData, "isLifeOfScale", isLifeOfScale);
		Convert::fromJson(jsonData, "lifeOfMinScale", lifeOfMinScale);
		Convert::fromJson(jsonData, "lifeOfMaxScale", lifeOfMaxScale);
		Convert::fromJson(jsonData, "isLifeOfAlpha", isLifeOfAlpha);
		Convert::fromJson(jsonData, "isFadeInOut", isFadeInOut);
		Convert::fromJson(jsonData, "fadeInTime", fadeInTime);
		Convert::fromJson(jsonData, "fadeOutTime", fadeOutTime);
		Convert::fromJson(jsonData, "isStretch", isStretch);
		Convert::fromJson(jsonData, "isParticleAddBlend", isParticleAddBlend);
		Convert::fromJson(jsonData, "isDraw2d", isDraw2d);
		Convert::fromJson(jsonData, "isScaleUp", isScaleUp);
		Convert::fromJson(jsonData, "scaleUpScale", scaleUpScale);
		Convert::fromJson(jsonData, "isBillBord", isBillBord);
		Convert::fromJson(jsonData, "useTexture", useTexture);
		Convert::fromJson(jsonData, "useMesh", useMesh);
		Convert::fromJson(jsonData, "radius", radius);
		Convert::fromJson(jsonData, "size", size);
		Convert::fromJson(jsonData, "angle", angle);
		Convert::fromJson(jsonData, "height", height);
		Convert::fromJson(jsonData, "isTextureSheetAnimation", isTextureSheetAnimation);
		Convert::fromJson(jsonData, "tiles", tiles);
		Convert::fromJson(jsonData, "isColorAnimation", isColorAnimation);
		Convert::fromJson(jsonData, "preColor", preColor);
		Convert::fromJson(jsonData, "postColor", postColor);
		Convert::fromJson(jsonData, "isRandomRotate", isRandomRotate);
		Convert::fromJson(jsonData, "minAngle", minAngle);
		Convert::fromJson(jsonData, "maxAngle", maxAngle);
	}

	void Attribute_Gui();

	void Debug_Gui() override {};
};

}