#include "ExplosionEffectInspector.h"

#include <algorithm>
#include <array>
#include <cstring>
#include "Engine/Module/Components/Effect/ExplosionEffectComponent.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/System/Manager/ImGuiManager.h"

using namespace AOENGINE;

void ExplosionEffectInspector::Draw(BaseGameObject& object) {
	ExplosionEffectComponent* component = object.GetExplosionEffectComponent();
	if (!component) { return; }
	bool remove = false;
	const bool open = ImGui::CollapsingHeader("Explosion Effect");
	if (ImGui::BeginPopupContextItem("ExplosionEffectContext")) {
		remove = ImGui::MenuItem("Remove Component");
		ImGui::EndPopup();
	}
	if (open) {
		ExplosionEffectSettings settings = component->GetSettings();
		bool changed = false;
		changed |= ImGui::Checkbox("Enabled##Explosion", &settings.enabled);
		changed |= ImGui::Checkbox("Auto Play##Explosion", &settings.autoPlay);
		changed |= ImGui::DragFloat("Duration##Explosion", &settings.duration, 0.01f, 0.01f, 30.0f);
		changed |= ImGui::DragFloat("Core Start Scale", &settings.coreStartScale, 0.01f, 0.001f, 100.0f);
		changed |= ImGui::DragFloat("Core End Scale", &settings.coreEndScale, 0.01f, 0.001f, 100.0f);
		changed |= ImGui::DragFloat("Shockwave End Scale", &settings.shockwaveEndScale, 0.01f, 0.001f, 100.0f);
		changed |= ImGui::DragFloat("Noise Scale##Explosion", &settings.noiseScale, 0.05f, 0.01f, 100.0f);
		changed |= ImGui::DragFloat("Noise Strength##Explosion", &settings.noiseStrength, 0.01f, 0.0f, 10.0f);
		changed |= ImGui::DragFloat("Turbulence##Explosion", &settings.turbulence, 0.01f, 0.0f, 10.0f);
		changed |= ImGui::DragFloat("Animation Speed##Explosion", &settings.animationSpeed, 0.01f, 0.0f, 10.0f);
		changed |= ImGui::DragFloat("Density##Explosion", &settings.density, 0.01f, 0.01f, 5.0f);
		changed |= ImGui::DragFloat("Rim Intensity##Explosion", &settings.rimIntensity, 0.01f, 0.0f, 5.0f);
		changed |= ImGui::DragFloat("Emissive Intensity", &settings.emissiveIntensity, 0.05f, 0.0f, 100.0f);
		changed |= ImGui::DragFloat("Shockwave Width", &settings.shockwaveWidth, 0.01f, 0.001f, 1.0f);
		changed |= ImGui::ColorEdit4("Inner Color##Explosion", &settings.innerColor.r);
		changed |= ImGui::ColorEdit4("Outer Color##Explosion", &settings.outerColor.r);
		std::array<char, 256> particle{};
		std::memcpy(particle.data(), settings.particleEffect.c_str(),
			(std::min)(settings.particleEffect.size(), particle.size() - 1));
		if (ImGui::InputText("Particle Effect##Explosion", particle.data(), particle.size())) {
			settings.particleEffect = particle.data();
			changed = true;
		}
		if (changed) { component->SetSettings(settings); }
		if (ImGui::Button("Play##Explosion")) { component->Play(); }
		ImGui::SameLine();
		if (ImGui::Button("Stop##Explosion")) { component->Stop(); }
	}
	if (remove) { object.RemoveExplosionEffectComponent(); }
}
