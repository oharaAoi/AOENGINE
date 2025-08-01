#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Json/IJsonConverter.h"

struct GpuParticleEmitterData {
	Vector4 color;
	Vector3 minScale;
	float pad1;
	Vector3 maxScale;
	float pad2;
	Vector3 targetScale;
	float pad3;
	Vector3 rotate;
	float pad4;
	Vector3 pos;
	float pad5;
	Vector3 size;

	uint32_t count;
	uint32_t emitType;
	uint32_t emitOrigin;
	uint32_t lifeOfScaleDown;

	uint32_t lifeOfScaleUp;
	uint32_t lifeOfAlpha;

	uint32_t separateByAxisScale;
	float scaleMinScaler;
	float scaleMaxScaler;

	float speed;
	float lifeTime;
	float gravity;
	float damping;

	float radius;
	float angle;
	float height;
	float pad6;
};

enum EmitType {
	UP,
	RANDOM,
	OUTSIDE
};

enum EmitOrigin {
	CENTER,
	RANGE,
	EDGE
};

enum GpuEmitterShape {
	SPHERE,
	BOX,
	CONE
};

struct GpuParticleEmitterItem : public IJsonConverter {
	bool isLoop = true;						// Loopをするか
	float duration = 5.0f;					// 継続時間
	Vector3 minScale = Vector3(1, 1, 1);
	Vector3 maxScale = Vector3(1, 1, 1);
	Vector3 rotate = Vector3(0, 0, 0);
	Vector3 pos = Vector3(0, 0, 0);
	uint32_t rateOverTimeCout = 10;
	int shape;
	int emitType = 0;
	int emitOrigin = 0;

	Vector4 color = Vector4(1,1,1,1);

	bool separateByAxisScale = false;
	float scaleMinScaler = 1.0f;
	float scaleMaxScaler = 5.0f;

	float speed = 10.0f;
	float lifeTime = 10.0f;
	float gravity = 0.0f;
	float damping = 0.0f;

	bool lifeOfScaleDown = false;
	bool lifeOfScaleUp = false;
	Vector3 targetScale = Vector3(1, 1, 1);
	bool lifeOfAlpha = false;

	float radius = 1.0f;
	Vector3 size = CVector3::UNIT;
	float angle = 1.2f;
	float height = 2.0f;

	GpuParticleEmitterItem() {
		toJsonFunction_ = [this](const std::string& id) {
			return this->ToJson(id);
			};
	}

	json ToJson(const std::string& id) const override {
		return JsonBuilder(id)
			.Add("isLoop", isLoop)
			.Add("duration", duration)
			.Add("minScale", minScale)
			.Add("maxScale", maxScale)
			.Add("rotate", rotate)
			.Add("rateOverTimeCout", rateOverTimeCout)
			.Add("shape", shape)
			.Add("emitType", emitType)
			.Add("emitOrigin", emitOrigin)
			.Add("color", color)
			.Add("SeparateByAxisScale", separateByAxisScale)
			.Add("scaleMinScaler", scaleMinScaler)
			.Add("scaleMaxScaler", scaleMaxScaler)
			.Add("speed", speed)
			.Add("lifeTime", lifeTime)
			.Add("gravity", gravity)
			.Add("damping", damping)
			.Add("lifeOfScaleDown", lifeOfScaleDown)
			.Add("lifeOfScaleUp", lifeOfScaleUp)
			.Add("targetScale", targetScale)
			.Add("lifeOfAlpha", lifeOfAlpha)
			.Add("radius", radius)
			.Add("size", size)
			.Add("angle", angle)
			.Add("height", height)
			
			.Build();
	}

	void FromJson(const json& jsonData) override {
		fromJson(jsonData, "isLoop", isLoop);
		fromJson(jsonData, "duration", duration);
		fromJson(jsonData, "minScale", minScale);
		fromJson(jsonData, "maxScale", maxScale);
		fromJson(jsonData, "rotate", rotate);
		fromJson(jsonData, "pos", pos);
		fromJson(jsonData, "rateOverTimeCout", rateOverTimeCout);
		fromJson(jsonData, "shape", shape);
		fromJson(jsonData, "emitType", emitType);
		fromJson(jsonData, "emitOrigin", emitOrigin);
		fromJson(jsonData, "color", color);
		fromJson(jsonData, "SeparateByAxisScale", separateByAxisScale);
		fromJson(jsonData, "scaleMinScaler", scaleMinScaler);
		fromJson(jsonData, "scaleMaxScaler", scaleMaxScaler);
		fromJson(jsonData, "speed", speed);
		fromJson(jsonData, "lifeTime", lifeTime);
		fromJson(jsonData, "gravity", gravity);
		fromJson(jsonData, "damping", damping);
		fromJson(jsonData, "lifeOfScaleDown", lifeOfScaleDown);
		fromJson(jsonData, "lifeOfScaleUp", lifeOfScaleUp);
		fromJson(jsonData, "targetScale", targetScale);
		fromJson(jsonData, "lifeOfAlpha", lifeOfAlpha);
		fromJson(jsonData, "radius", radius);
		fromJson(jsonData, "size", size);
		fromJson(jsonData, "angle", angle);
		fromJson(jsonData, "height", height);
	}

	void Attribute_Gui();
};