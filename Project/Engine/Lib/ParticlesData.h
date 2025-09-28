
#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Json/IJsonConverter.h"

enum class CpuEmitDirection {
	UP,			// 上方向に射出
	RANDOM,		// ランダム方向に射出
	OUTSIDE,	// 中心から反対に射出
	CENTERFOR	// 中心に向かって射出
};

enum class CpuEmitOrigin {
	CENTER,		// 中心から
	RANGE,		// 範囲からランダムに
};

enum class CpuEmitterShape {
	SPHERE,
	BOX,
	CONE
};

struct ParticleSingle {
	Vector4 color;			// 色
	Vector3 scale;			// 拡縮
	Quaternion rotate;		// 回転
	Vector3 translate;		// 座標
	Vector3 velocity;		// 速度
	Vector3 acceleration;	// 加速度
	Vector3 firstScale;		// 初期拡縮
	Vector3 emitterCenter;	// Emitterの中心
	float lifeTime;			// 生存時間
	float currentTime;		// 現在の時間
	float damping;			// 減衰
	float gravity;			// 重力
	bool isLerpDiscardValue;		// discardの値をlerpさせるか
	float discardValue = 0.01f;		// discardをするしきい値
	float startDiscard = 0.01f;		// discardの初期値
	float endDiscard = 1.0f;		// discardの終了値
	bool isLifeOfScale = false;	// 生存時間によるサイズ
	bool isLifeOfAlpha = false;	// 生存時間による透明度
	bool isFadeInOut = false;
	float fadeInTime = 0.0f;
	float fadeOutTime = 0.0f;
	float initAlpha_ = 0.0f;
	bool isScaleUpScale = false;	// 生存時間による透明度
	bool isAddBlend = true;
	Vector3 upScale;
	bool isBillBord = true;
	bool isDraw2d = true;
	bool isCenterFor = false;
};

struct ParticleEmit : public IJsonConverter {
	bool isLoop = true;						// Loopをするか
	float duration = 5.0f;					// 継続時間
	Vector3 translate = CVector3::ZERO;		// 位置
	Vector3 rotate = CVector3::ZERO;		// 射出方向
	uint32_t rateOverTimeCout = 10;			// 射出数
	int shape = 0;						// emitterの種類
	int emitDirection = 1;
	int emitOrigin = 0;

	// particle自体のparameter
	bool isRandomColor = false;
	Vector4 color = Vector4{ 1,1,1,1 };			// 色
	Vector4 randColor1 = Vector4{ 1,0,0,1 };	// 色
	Vector4 randColor2 = Vector4{ 0,0,1,1 };	// 色
	bool isLerpDiscardValue;					// discardの値をlerpさせるか
	float discardValue = 0.01f;					// discardをするしきい値
	float startDiscard = 0.01f;					// discardの初期値
	float endDiscard = 1.0f;					// discardの終了値
	bool separateByAxisScale = false;
	Vector3 minScale = CVector3::UNIT;			// 最小の大きさ
	Vector3 maxScale = CVector3::UNIT;			// 最大の大きさ
	float speed = 1.0f;							// 速度
	float lifeTime = 5.0f;						// particleの生存時間
	float gravity = 0;							// 重力を付与するか
	float dampig = 0;							// 減衰率
	float angleMin = 0.0f;						// 最小の回転
	float angleMax = 360.0f;					// 最大の回転
	bool isDirectionRotate = false;				// 進行方向にParticleを向ける処理
	bool isLifeOfScale = false;					// 生存時間によるサイズ
	bool isLifeOfAlpha = false;					// 生存時間による透明度
	bool isFadeInOut = false;					// FadeInOutで出現
	float fadeInTime = 0;
	float fadeOutTime = 0;

	bool isParticleAddBlend = false;			// blendModeをAddBlendにするかのフラグ
	bool isDraw2d = false;						// blendModeをAddBlendにするかのフラグ
	bool isBillBord = true;

	bool isScaleUp;								// サイズを大きくするか
	Vector3 scaleUpScale;

	float radius = 0.5f;
	float angle = 27.f;
	float height = 1;
	Vector3 size = CVector3::UNIT;

	std::string useTexture = "circle.png";
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
			.Add("isLifeOfAlpha", isLifeOfAlpha)
			.Add("isFadeInOut", isFadeInOut)
			.Add("fadeInTime", fadeInTime)
			.Add("fadeOutTime", fadeOutTime)
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
			.Build();
	}

	void FromJson(const json& jsonData) override {
		fromJson(jsonData, "isLoop", isLoop);
		fromJson(jsonData, "duration", duration);
		fromJson(jsonData, "rotate", rotate);
		fromJson(jsonData, "translate", translate);
		fromJson(jsonData, "shape", shape);
		fromJson(jsonData, "rateOverTimeCout", rateOverTimeCout);
		fromJson(jsonData, "emitDirection", emitDirection);
		fromJson(jsonData, "emitOrigin", emitOrigin);
		fromJson(jsonData, "color", color);
		fromJson(jsonData, "isRandomColor", isRandomColor);
		fromJson(jsonData, "randColor1", randColor1);
		fromJson(jsonData, "randColor2", randColor2);
		fromJson(jsonData, "isLerpDiscardValue", isLerpDiscardValue);
		fromJson(jsonData, "discardValue", discardValue);
		fromJson(jsonData, "startDiscard", startDiscard);
		fromJson(jsonData, "endDiscard", endDiscard);
		fromJson(jsonData, "separateByAxisScale", separateByAxisScale);
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
		fromJson(jsonData, "isFadeInOut", isFadeInOut);
		fromJson(jsonData, "fadeInTime", fadeInTime);
		fromJson(jsonData, "fadeOutTime", fadeOutTime);
		fromJson(jsonData, "isParticleAddBlend", isParticleAddBlend);
		fromJson(jsonData, "isDraw2d", isDraw2d);
		fromJson(jsonData, "isScaleUp", isScaleUp);
		fromJson(jsonData, "scaleUpScale", scaleUpScale);
		fromJson(jsonData, "isBillBord", isBillBord);
		fromJson(jsonData, "useTexture", useTexture);
		fromJson(jsonData, "useMesh", useMesh);
		fromJson(jsonData, "radius", radius);
		fromJson(jsonData, "size", size);
		fromJson(jsonData, "angle", angle);
		fromJson(jsonData, "height", height);
	}

	void Attribute_Gui();

	void Debug_Gui() override {};

public:
	// ======== Setters / Getters ========
	void SetIsLoop(bool _isLoop) { isLoop = _isLoop; }
	bool GetIsLoop() const { return isLoop; }

	void SetDuration(float _duration) { duration = _duration; }
	float GetDuration() const { return duration; }

	void SetTranslate(const Vector3& _translate) { translate = _translate; }
	const Vector3& GetTranslate() const { return translate; }

	void SetRotate(const Vector3& _rotate) { rotate = _rotate; }
	const Vector3& GetRotate() const { return rotate; }

	void SetRateOverTimeCout(uint32_t _count) { rateOverTimeCout = _count; }
	uint32_t GetRateOverTimeCout() const { return rateOverTimeCout; }

	void SetShape(int _shape) { shape = _shape; }
	int GetShape() const { return shape; }

	void SetEmitDirection(int _emitDirection) { emitDirection = _emitDirection; }
	int GetEmitDirection() const { return emitDirection; }

	void SetEmitOrigin(int _emitOrigin) { emitOrigin = _emitOrigin; }
	int GetEmitOrigin() const { return emitOrigin; }

	void SetIsRandomColor(bool _isRandomColor) { isRandomColor = _isRandomColor; }
	bool GetIsRandomColor() const { return isRandomColor; }

	void SetColor(const Vector4& _color) { color = _color; }
	const Vector4& GetColor() const { return color; }

	void SetRandColor1(const Vector4& _randColor1) { randColor1 = _randColor1; }
	const Vector4& GetRandColor1() const { return randColor1; }

	void SetRandColor2(const Vector4& _randColor2) { randColor2 = _randColor2; }
	const Vector4& GetRandColor2() const { return randColor2; }

	void SetIsLerpDiscard(bool _isDiscard) { isLerpDiscardValue = _isDiscard; }
	bool GetIsLerpDiscard() const { return isLerpDiscardValue; }

	void SetDiscardValue(float _discardValue) { discardValue = _discardValue; }
	float GetDiscardValue() const { return discardValue; }

	void SetStartDiscard(float _stDiscard) { startDiscard = _stDiscard; }
	float GetStartDiscard() const { return startDiscard; }

	void SetEndDiscard(float _endDiscard) { endDiscard = _endDiscard; }
	bool GetEndDiscard() const { return endDiscard; }

	void SetSeparateByAxisScale(bool _flag) { separateByAxisScale = _flag; }
	bool GetSeparateByAxisScale() const { return separateByAxisScale; }

	void SetMinScale(const Vector3& _minScale) { minScale = _minScale; }
	const Vector3& GetMinScale() const { return minScale; }

	void SetMaxScale(const Vector3& _maxScale) { maxScale = _maxScale; }
	const Vector3& GetMaxScale() const { return maxScale; }

	void SetSpeed(float _speed) { speed = _speed; }
	float GetSpeed() const { return speed; }

	void SetLifeTime(float _lifeTime) { lifeTime = _lifeTime; }
	float GetLifeTime() const { return lifeTime; }

	void SetGravity(float _gravity) { gravity = _gravity; }
	float GetGravity() const { return gravity; }

	void SetDampig(float _dampig) { dampig = _dampig; }
	float GetDampig() const { return dampig; }

	void SetAngleMin(float _angleMin) { angleMin = _angleMin; }
	float GetAngleMin() const { return angleMin; }

	void SetAngleMax(float _angleMax) { angleMax = _angleMax; }
	float GetAngleMax() const { return angleMax; }

	void SetIsDirectionRotate(bool _flag) { isDirectionRotate = _flag; }
	bool GetIsDirectionRotate() const { return isDirectionRotate; }

	void SetIsLifeOfScale(bool _flag) { isLifeOfScale = _flag; }
	bool GetIsLifeOfScale() const { return isLifeOfScale; }

	void SetIsLifeOfAlpha(bool _flag) { isLifeOfAlpha = _flag; }
	bool GetIsLifeOfAlpha() const { return isLifeOfAlpha; }

	void SetIsFadeInOut(bool _flag) { isFadeInOut = _flag; }
	bool GetIsFadeInOut() const { return isFadeInOut; }

	void SetFadeInTime(float _time) { fadeInTime = _time; }
	float GetFadeInTime() const { return fadeInTime; }

	void SetFadeOutTime(float _time) { fadeOutTime = _time; }
	float GetFadeOutTime() const { return fadeOutTime; }

	void SetIsParticleAddBlend(bool _flag) { isParticleAddBlend = _flag; }
	bool GetIsParticleAddBlend() const { return isParticleAddBlend; }

	void SetIsDraw2d(bool _flag) { isDraw2d = _flag; }
	bool GetIsDraw2d() const { return isDraw2d; }

	void SetIsBillBord(bool _flag) { isBillBord = _flag; }
	bool GetIsBillBord() const { return isBillBord; }

	void SetIsScaleUp(bool _flag) { isScaleUp = _flag; }
	bool GetIsScaleUp() const { return isScaleUp; }

	void SetScaleUpScale(const Vector3& _scale) { scaleUpScale = _scale; }
	const Vector3& GetScaleUpScale() const { return scaleUpScale; }

	void SetRadius(float _radius) { radius = _radius; }
	float GetRadius() const { return radius; }

	void SetAngle(float _angle) { angle = _angle; }
	float GetAngle() const { return angle; }

	void SetHeight(float _height) { height = _height; }
	float GetHeight() const { return height; }

	void SetSize(const Vector3& _size) { size = _size; }
	const Vector3& GetSize() const { return size; }

	void SetUseTexture(const std::string& _useTexture) { useTexture = _useTexture; }
	const std::string& GetUseTexture() const { return useTexture; }

	void SetUseMesh(const std::string& _useMesh) { useMesh = _useMesh; }
	const std::string& GetUseMesh() const { return useMesh; }
};
