#include "GradientNode.h"

using namespace AOENGINE;

void GradientNode::Init() {
	addOUT<GradientData>("Gradient", ImFlow::PinStyle::blue())->behaviour([this]() { return gradient_; });
	SetTitleBar(ImColor(46, 139, 87));
}

void GradientNode::draw() {
	auto preview = gradient_;
	preview.Normalize();
	const ImVec2 origin = ImGui::GetCursorScreenPos();
	const ImVec2 size(180.0f, 24.0f);
	auto* drawList = ImGui::GetWindowDrawList();
	// Checkerboard keeps transparent keys visible in the preview.
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 23; ++x) {
			const ImU32 color = ((x + y) % 2) ? IM_COL32(95, 95, 95, 255) : IM_COL32(150, 150, 150, 255);
			drawList->AddRectFilled(origin + ImVec2(x * 8.0f, y * 8.0f),
				origin + ImVec2((std::min)((x + 1) * 8.0f, size.x), (y + 1) * 8.0f), color);
		}
	}
	for (int x = 0; x < 180; ++x) {
		const auto color = preview.Sample(static_cast<float>(x) / 179.0f);
		drawList->AddRectFilled(origin + ImVec2(static_cast<float>(x), 0),
			origin + ImVec2(static_cast<float>(x + 1), size.y),
			ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3])));
	}
	ImGui::Dummy(size);
	ImGui::Text("%zu / %zu keys", gradient_.keys.size(), GradientData::kMaxKeys);
}

void GradientNode::updateGui() {
	ImGui::PushID(this);
	if (ImGui::Button("Grayscale")) { gradient_ = GradientData{}; }
	ImGui::SameLine();
	if (ImGui::Button("Fire")) {
		gradient_.keys = {{0.0f, {0, 0, 0, 0}}, {0.25f, {0.5f, 0, 0, 0.5f}},
			{0.5f, {1, 0.2f, 0, 1}}, {0.8f, {1, 0.8f, 0, 1}}, {1.0f, {1, 1, 0.8f, 1}}};
	}
	int removeIndex = -1;
	for (size_t i = 0; i < gradient_.keys.size(); ++i) {
		ImGui::PushID(static_cast<int>(i));
		auto& key = gradient_.keys[i];
		ImGui::Separator();
		ImGui::Text("Key %zu", i + 1);
		ImGui::SetNextItemWidth(180);
		ImGui::SliderFloat("Position", &key.position, 0.0f, 1.0f);
		ImGui::SetNextItemWidth(220);
		ImGui::ColorEdit4("Color", key.color.data(), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
		ImGui::BeginDisabled(gradient_.keys.size() <= 2);
		if (ImGui::Button("Remove")) { removeIndex = static_cast<int>(i); }
		ImGui::EndDisabled();
		ImGui::PopID();
	}
	if (removeIndex >= 0) { gradient_.keys.erase(gradient_.keys.begin() + removeIndex); }
	ImGui::BeginDisabled(gradient_.keys.size() >= GradientData::kMaxKeys);
	if (ImGui::Button("Add Key")) {
		gradient_.keys.push_back({0.5f, gradient_.Sample(0.5f)});
	}
	ImGui::EndDisabled();
	// Keep row IDs stable while dragging a position across another key.
	if (!ImGui::IsAnyItemActive()) { gradient_.Normalize(); }
	ImGui::PopID();
}

nlohmann::json GradientNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	auto gradient = gradient_;
	gradient.Normalize();
	result["props"]["keys"] = gradient.ToJson();
	return result;
}

void GradientNode::fromJson(const nlohmann::json& json) {
	BaseInfoFromJson(json);
	if (json.contains("props")) { gradient_.FromJson(json["props"].value("keys", nlohmann::json::array())); }
}
