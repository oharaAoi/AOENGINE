#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Json/IJsonConverter.h"

namespace AOENGINE {

/// <summary>
/// GPUParticleの単体のパラメータ
/// </summary>
struct GpuParticleEmitterData {
	AOENGINE::Color color;

	Math::Vector3 minScale;
	float pad1;
	Math::Vector3 maxScale;
	float pad2;
	Math::Vector3 targetScale;
	float pad3;
	Math::Vector3 rotate;
	float pad4;
	Math::Vector3 pos;
	float pad5;
	Math::Vector3 prePos;
	float pad6;
	Math::Vector3 size;

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
	float pad7;
};

enum class EmitType {
	Up,
	Random,
	Outside
};

enum class EmitOrigin {
	Center,
	Range,
	Edge
};

enum class GpuEmitterShape {
	Sphere,
	Box,
	Cone
};

/// <summary>
/// Emitterのパラメータ
/// </summary>
struct GpuParticleEmitterItem : public AOENGINE::IJsonConverter {
	bool isLoop = true;						// Loopをするか
	float duration = 5.0f;					// 継続時間
	Math::Vector3 minScale = Math::Vector3(1, 1, 1);
	Math::Vector3 maxScale = Math::Vector3(1, 1, 1);
	Math::Vector3 rotate = Math::Vector3(0, 0, 0);
	Math::Vector3 pos = Math::Vector3(0, 0, 0);
	uint32_t rateOverTimeCout = 10;
	int shape;
	int emitType = 0;
	int emitOrigin = 0;

	AOENGINE::Color color = AOENGINE::Color(1, 1, 1, 1);

	bool separateByAxisScale = false;
	float scaleMinScaler = 1.0f;
	float scaleMaxScaler = 5.0f;

	float speed = 10.0f;
	float lifeTime = 10.0f;
	float gravity = 0.0f;
	float damping = 0.0f;

	bool lifeOfScaleDown = false;
	bool lifeOfScaleUp = false;
	Math::Vector3 targetScale = Math::Vector3(1, 1, 1);
	bool lifeOfAlpha = false;

	float radius = 1.0f;
	Math::Vector3 size = CVector3::UNIT;
	float angle = 1.2f;
	float height = 2.0f;

	GpuParticleEmitterItem() {
		toJsonFunction_ = [this](const std::string& id) {
			return this->ToJson(id);
			};
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("isLoop", isLoop)
			.Add("duration", duration)
			.Add("minScale", minScale)
			.Add("maxScale", maxScale)
			.Add("rotate", rotate)
			.Add("pos", pos)
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

	void Debug_Gui() override {};
};

}