#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include <nlohmann/json.hpp>

namespace AOENGINE {

struct GradientKey {
	float position = 0.0f;
	std::array<float, 4> color{0.0f, 0.0f, 0.0f, 1.0f};
};

// Shared by the editor preview, JSON persistence and GPU parameter upload.
struct GradientData {
	static constexpr size_t kMaxKeys = 8;
	std::vector<GradientKey> keys{{0.0f, {0, 0, 0, 1}}, {1.0f, {1, 1, 1, 1}}};

	void Normalize() {
		if (keys.empty()) { keys = GradientData{}.keys; }
		if (keys.size() > kMaxKeys) { keys.resize(kMaxKeys); }
		for (auto& key : keys) {
			key.position = std::isfinite(key.position) ? std::clamp(key.position, 0.0f, 1.0f) : 0.0f;
			for (float& component : key.color) {
				component = std::isfinite(component) ? std::clamp(component, 0.0f, 1.0f) : 0.0f;
			}
		}
		std::stable_sort(keys.begin(), keys.end(), [](const auto& a, const auto& b) { return a.position < b.position; });
	}

	std::array<float, 4> Sample(float value) const {
		if (keys.empty()) { return {0, 0, 0, 1}; }
		auto color = keys.front().color;
		for (size_t i = 1; i < keys.size(); ++i) {
			const auto& previous = keys[i - 1];
			const auto& next = keys[i];
			// Coincident keys form a hard edge; the last key wins at that position.
			const float width = next.position - previous.position;
			const float t = width > 0.0f ? std::clamp((value - previous.position) / width, 0.0f, 1.0f)
				: (value >= next.position ? 1.0f : 0.0f);
			for (size_t c = 0; c < color.size(); ++c) { color[c] += (next.color[c] - color[c]) * t; }
		}
		return color;
	}

	nlohmann::json ToJson() const {
		auto result = nlohmann::json::array();
		for (const auto& key : keys) {
			result.push_back({{"position", key.position}, {"color", {
				{"r", key.color[0]}, {"g", key.color[1]}, {"b", key.color[2]}, {"a", key.color[3]}}}});
		}
		return result;
	}

	void FromJson(const nlohmann::json& source) {
		keys.clear();
		if (source.is_array()) {
			for (const auto& entry : source) {
				if (!entry.is_object() || keys.size() == kMaxKeys) { continue; }
				GradientKey key;
				key.position = entry.value("position", 0.0f);
				if (entry.contains("color") && entry["color"].is_object()) {
					const auto& color = entry["color"];
					key.color = {color.value("r", 0.0f), color.value("g", 0.0f), color.value("b", 0.0f), color.value("a", 1.0f)};
				}
				keys.push_back(key);
			}
		}
		Normalize();
	}
};

}
