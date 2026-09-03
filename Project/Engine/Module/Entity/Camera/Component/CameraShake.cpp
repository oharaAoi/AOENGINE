#include "CameraShake.h"

#include <algorithm>
#include <cmath>

#include "Engine/Lib/Math/MyMath.h"

namespace {
float HashToSignedFloat(uint32_t value) {
	value ^= value >> 16;
	value *= 0x7feb352du;
	value ^= value >> 15;
	value *= 0x846ca68bu;
	value ^= value >> 16;
	return static_cast<float>(value & 0x00ffffffu) / 8388607.5f - 1.0f;
}

float SmoothStep(float value) { return value * value * (3.0f - 2.0f * value); }

float SampleWhite(float x, uint32_t seed) {
	return HashToSignedFloat(static_cast<uint32_t>(static_cast<int>(std::floor(x))) + seed);
}

float SampleValue(float x, uint32_t seed) {
	const int sample = static_cast<int>(std::floor(x));
	const float fraction = x - static_cast<float>(sample);
	const float a = HashToSignedFloat(static_cast<uint32_t>(sample) + seed);
	const float b = HashToSignedFloat(static_cast<uint32_t>(sample + 1) + seed);
	return a + (b - a) * SmoothStep(fraction);
}

float SamplePerlin(float x, uint32_t seed) {
	const int sample = static_cast<int>(std::floor(x));
	const float fraction = x - static_cast<float>(sample);
	const float a = HashToSignedFloat(static_cast<uint32_t>(sample) + seed) * fraction;
	const float b = HashToSignedFloat(static_cast<uint32_t>(sample + 1) + seed) * (fraction - 1.0f);
	return std::clamp((a + (b - a) * SmoothStep(fraction)) * 2.0f, -1.0f, 1.0f);
}

float SampleNoise(CameraShakeNoiseType type, float x, uint32_t seed) {
	switch (type) {
	case CameraShakeNoiseType::White: return SampleWhite(x, seed);
	case CameraShakeNoiseType::Value: return SampleValue(x, seed);
	case CameraShakeNoiseType::Perlin:
	default: return SamplePerlin(x, seed);
	}
}

Math::Quaternion MakeRotationOffset(const Math::Vector3& degrees) {
	return Math::Quaternion::EulerToQuaternion(degrees * kToRadian).Normalize();
}
}

void CameraShake::Play(const CameraShakeRequest& request) {
	request_ = request;
	request_.duration = std::max(request_.duration, 0.0f);
	request_.strength = std::max(request_.strength, 0.0f);
	request_.noiseStrength = std::max(request_.noiseStrength, 0.0f);
	request_.noiseFrequency = std::max(request_.noiseFrequency, 0.0f);
	elapsedTime_ = 0.0f;
	result_ = {};
	isPlaying_ = request_.duration > 0.0f && request_.strength > 0.0f;
}

void CameraShake::Stop(bool immediate) {
	if (!immediate) { return; }
	isPlaying_ = false;
	elapsedTime_ = 0.0f;
	result_ = {};
}

void CameraShake::Update(float deltaTime) {
	if (!isPlaying_) { result_ = {}; return; }
	elapsedTime_ = std::min(elapsedTime_ + std::max(deltaTime, 0.0f), request_.duration);
	result_ = request_.source == CameraShakeSource::Noise ? EvaluateNoise(elapsedTime_) : EvaluateParameter(elapsedTime_);
	if (elapsedTime_ >= request_.duration) { isPlaying_ = false; result_ = {}; }
}

bool CameraShake::IsPlaying() const { return isPlaying_; }
const CameraShakeResult& CameraShake::GetResult() const { return result_; }

CameraShakeResult CameraShake::EvaluateParameter(float time) const {
	const float scale = request_.strength * EvaluateDecay();
	const Math::Vector3 positionWave{
		std::sin(time * request_.positionFrequency.x + request_.phase.x),
		std::sin(time * request_.positionFrequency.y + request_.phase.y),
		std::sin(time * request_.positionFrequency.z + request_.phase.z) };
	const Math::Vector3 rotationWave{
		std::sin(time * request_.rotationFrequency.x + request_.phase.x),
		std::sin(time * request_.rotationFrequency.y + request_.phase.y),
		std::sin(time * request_.rotationFrequency.z + request_.phase.z) };
	CameraShakeResult result;
	result.positionOffset = positionWave * request_.positionAmplitude * scale;
	result.rotationOffset = MakeRotationOffset(rotationWave * request_.rotationAmplitude * scale);
	return result;
}

CameraShakeResult CameraShake::EvaluateNoise(float time) const {
	constexpr uint32_t seeds[] = { 11u, 37u, 71u, 101u, 137u, 173u };
	const float noiseTime = time * request_.noiseFrequency;
	const float scale = request_.strength * request_.noiseStrength * EvaluateDecay();
	const Math::Vector3 positionNoise{
		SampleNoise(request_.noiseType, noiseTime, request_.seed + seeds[0]),
		SampleNoise(request_.noiseType, noiseTime, request_.seed + seeds[1]),
		SampleNoise(request_.noiseType, noiseTime, request_.seed + seeds[2]) };
	const Math::Vector3 rotationNoise{
		SampleNoise(request_.noiseType, noiseTime, request_.seed + seeds[3]),
		SampleNoise(request_.noiseType, noiseTime, request_.seed + seeds[4]),
		SampleNoise(request_.noiseType, noiseTime, request_.seed + seeds[5]) };
	CameraShakeResult result;
	result.positionOffset = positionNoise * request_.positionAmplitude * scale;
	result.rotationOffset = MakeRotationOffset(rotationNoise * request_.rotationAmplitude * scale);
	return result;
}

float CameraShake::EvaluateDecay() const {
	if (request_.duration <= 0.0f) { return 0.0f; }
	const float remaining = 1.0f - std::clamp(elapsedTime_ / request_.duration, 0.0f, 1.0f);
	switch (request_.decay) {
	case CameraShakeDecay::None: return 1.0f;
	case CameraShakeDecay::Linear: return remaining;
	case CameraShakeDecay::EaseOut:
	default: return remaining * remaining;
	}
}
