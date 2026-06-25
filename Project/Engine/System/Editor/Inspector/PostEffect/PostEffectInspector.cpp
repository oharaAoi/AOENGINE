#include "PostEffectInspector.h"

#include <algorithm>
#include <cstdint>

#include "Engine/Module/PostEffect/Bloom.h"
#include "Engine/Module/PostEffect/DepthBasedOutline.h"
#include "Engine/Module/PostEffect/Dissolve.h"
#include "Engine/Module/PostEffect/GaussianFilter.h"
#include "Engine/Module/PostEffect/GlitchNoise.h"
#include "Engine/Module/PostEffect/Grayscale.h"
#include "Engine/Module/PostEffect/LuminanceBasedOutline.h"
#include "Engine/Module/PostEffect/MotionBlur.h"
#include "Engine/Module/PostEffect/RadialBlur.h"
#include "Engine/Module/PostEffect/Smoothing.h"
#include "Engine/Module/PostEffect/ToonMap.h"
#include "Engine/Module/PostEffect/Vignette.h"
#include "Engine/System/Manager/ImGuiManager.h"

namespace {

void DrawUInt(const char* label, uint32_t& value, uint32_t minimum = 1) {
	ImGui::DragScalar(label, ImGuiDataType_U32, &value, 1.0f, &minimum, nullptr, "%u");
	value = (std::max)(value, minimum);
}

void DrawGrayscale(PostEffect::Grayscale& effect) {
	auto& settings = effect.GetSettings();
	ImGui::ColorEdit4("Color", &settings.color.r);
}

void DrawRadialBlur(PostEffect::RadialBlur& effect) {
	auto setting = effect.GetBlurSetting();
	ImGui::DragFloat2("Center", &setting.blurCenter.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Strength", &setting.blurStrength, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Start", &setting.blurStart, 0.01f, 0.0f, 1.0f);
	setting.blurCenter.x = std::clamp(setting.blurCenter.x, 0.0f, 1.0f);
	setting.blurCenter.y = std::clamp(setting.blurCenter.y, 0.0f, 1.0f);
	effect.SetBlurSetting(setting);
	DrawUInt("Sample Count", effect.GetSettings().sampleCount);
}

void DrawGlitchNoise(PostEffect::GlitchNoise& effect) {
	auto& settings = effect.GetSettings();
	ImGui::DragFloat2("Texel Size", &settings.texelSize.x, 1.0f, 0.0f);

	float strength = effect.GetStrength();
	float duration = effect.GetNoiseTime();
	const bool strengthChanged = ImGui::DragFloat("Strength", &strength, 0.01f, 0.0f);
	const bool durationChanged = ImGui::DragFloat("Duration", &duration, 0.01f, 0.0f);
	if (strengthChanged || durationChanged) {
		effect.SetNoiseParameters(strength, duration);
	}

	ImGui::Text("Current Time: %.3f", effect.GetCurrentTime());
	if (ImGui::Button("Restart")) {
		effect.RestartNoise();
	}
}

void DrawVignette(PostEffect::Vignette& effect) {
	auto& settings = effect.GetSettings();
	ImGui::ColorEdit4("Color", &settings.color.r);
	ImGui::DragFloat("Scale", &settings.scale, 0.1f, 0.0f, 20.0f);
	ImGui::DragFloat("Power", &settings.power, 0.01f, 0.0f, 1.0f);
	settings.scale = std::clamp(settings.scale, 0.0f, 20.0f);
	settings.power = std::clamp(settings.power, 0.0f, 1.0f);
}

void DrawDissolve(PostEffect::Dissolve& effect) {
	auto& settings = effect.GetSettings();
	ImGui::DragFloat3("UV Scale", &settings.uvTransform.scale.x, 0.1f);
	ImGui::DragFloat3("UV Rotation", &settings.uvTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("UV Translation", &settings.uvTransform.translate.x, 0.1f);
	ImGui::ColorEdit4("Color", &settings.color.r);
	ImGui::ColorEdit4("Edge Color", &settings.edgeColor.r);
	ImGui::DragFloat("Threshold", &settings.threshold, 0.01f, 0.0f, 1.0f);
	settings.threshold = std::clamp(settings.threshold, 0.0f, 1.0f);
}

void DrawToonMap(PostEffect::ToonMap& effect) {
	auto& settings = effect.GetSettings();
	ImGui::DragFloat("Exposure", &settings.exposure, 0.01f, 0.0f);
	settings.exposure = (std::max)(settings.exposure, 0.0f);
}

void DrawBloom(PostEffect::Bloom& effect) {
	auto& settings = effect.GetSettings();
	ImGui::DragFloat("Intensity", &settings.bloomIntensity, 0.01f, 0.0f);
	settings.bloomIntensity = (std::max)(settings.bloomIntensity, 0.0f);

	if (ImGui::CollapsingHeader("Brightness Threshold", ImGuiTreeNodeFlags_DefaultOpen)) {
		auto& threshold = effect.GetBrightnessThreshold().GetSettings().threshold;
		ImGui::DragFloat("Threshold", &threshold, 0.01f, 0.0f, 1.0f);
		threshold = std::clamp(threshold, 0.0f, 1.0f);
	}
	if (ImGui::CollapsingHeader("Gaussian Blur", ImGuiTreeNodeFlags_DefaultOpen)) {
		DrawUInt("Horizontal Samples", effect.GetBlurWidth().GetSettings().sampleCount);
		DrawUInt("Vertical Samples", effect.GetBlurHeight().GetSettings().sampleCount);
	}
}

void DrawSmoothing(PostEffect::Smoothing& effect) {
	DrawUInt("Size", effect.GetSettings().size);
}

void DrawGaussianFilter(PostEffect::GaussianFilter& effect) {
	auto& settings = effect.GetSettings();
	ImGui::DragFloat("Deviation", &settings.deviation, 0.01f, 0.0f);
	settings.deviation = (std::max)(settings.deviation, 0.0f);
	DrawUInt("Size", settings.size);
}

void DrawLuminanceOutline(PostEffect::LuminanceBasedOutline& effect) {
	auto& settings = effect.GetSettings();
	ImGui::DragFloat("Scale", &settings.scale, 0.1f, 0.0f);
	settings.scale = (std::max)(settings.scale, 0.0f);
}

void DrawDepthOutline(PostEffect::DepthBasedOutline& effect) {
	auto& settings = effect.GetSettings();
	ImGui::DragFloat("Edge Gain", &settings.edgeGain, 0.1f, 0.0f, 10.0f);
	settings.edgeGain = std::clamp(settings.edgeGain, 0.0f, 10.0f);
}

template<class T>
T* CastEffect(PostEffect::IPostEffect& effect) {
	return dynamic_cast<T*>(&effect);
}

}

void AOENGINE::PostProcessInspector::Draw(PostProcess& postProcess) {
	ImGui::TextUnformatted("Post effects are evaluated in hierarchy order.");
	ImGui::Separator();

	for (PostEffectType type : postProcess.GetEffectOrder()) {
		auto effect = postProcess.GetEffect(type);
		if (!effect) {
			continue;
		}

		bool enabled = effect->GetIsEnable();
		if (ImGui::Checkbox(PostProcess::GetEffectName(type), &enabled)) {
			effect->SetIsEnable(enabled);
		}
	}
}

void AOENGINE::PostEffectInspector::Draw(PostEffectType type, PostEffect::IPostEffect& effect) {
	switch (type) {
	case PostEffectType::Grayscale:
		if (auto* value = CastEffect<PostEffect::Grayscale>(effect)) DrawGrayscale(*value);
		break;
	case PostEffectType::RadialBlur:
		if (auto* value = CastEffect<PostEffect::RadialBlur>(effect)) DrawRadialBlur(*value);
		break;
	case PostEffectType::GlitchNoise:
		if (auto* value = CastEffect<PostEffect::GlitchNoise>(effect)) DrawGlitchNoise(*value);
		break;
	case PostEffectType::Vignette:
		if (auto* value = CastEffect<PostEffect::Vignette>(effect)) DrawVignette(*value);
		break;
	case PostEffectType::Dissolve:
		if (auto* value = CastEffect<PostEffect::Dissolve>(effect)) DrawDissolve(*value);
		break;
	case PostEffectType::ToonMap:
		if (auto* value = CastEffect<PostEffect::ToonMap>(effect)) DrawToonMap(*value);
		break;
	case PostEffectType::Bloom:
		if (auto* value = CastEffect<PostEffect::Bloom>(effect)) DrawBloom(*value);
		break;
	case PostEffectType::Smoothing:
		if (auto* value = CastEffect<PostEffect::Smoothing>(effect)) DrawSmoothing(*value);
		break;
	case PostEffectType::GaussianFilter:
		if (auto* value = CastEffect<PostEffect::GaussianFilter>(effect)) DrawGaussianFilter(*value);
		break;
	case PostEffectType::LuminanceOutline:
		if (auto* value = CastEffect<PostEffect::LuminanceBasedOutline>(effect)) DrawLuminanceOutline(*value);
		break;
	case PostEffectType::DepthOutline:
		if (auto* value = CastEffect<PostEffect::DepthBasedOutline>(effect)) DrawDepthOutline(*value);
		break;
	case PostEffectType::MotionBlur:
		ImGui::TextUnformatted("No editable parameters.");
		break;
	default:
		ImGui::TextUnformatted("No inspector registered for this effect.");
		break;
	}
}
