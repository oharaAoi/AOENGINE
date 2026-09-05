#include "Engine/System/ShaderGraph/Node/Texture/GradientData.h"
#include <cassert>
#include <iostream>
#include <limits>

int main() {
	using AOENGINE::GradientData;
	GradientData gradient;
	assert(gradient.Sample(-1.0f)[0] == 0.0f);
	assert(gradient.Sample(2.0f)[0] == 1.0f);
	assert(gradient.Sample(0.5f)[0] == 0.5f);
	assert(gradient.Sample(0.5f)[3] == 1.0f);

	gradient.keys = {{1.0f, {1, 1, 0, 1}}, {0.0f, {1, 0, 0, 0}}, {0.5f, {0, 1, 0, 0.5f}}};
	gradient.Normalize();
	const auto middle = gradient.Sample(0.25f);
	assert(middle[0] == 0.5f && middle[1] == 0.5f && middle[3] == 0.25f);
	// Use a textual round trip, as graph Save/Load does.
	const nlohmann::json node = {{"id", 123}, {"name", "Gradient"}, {"pos", {100, 200}},
		{"props", {{"keys", gradient.ToJson()}}}};
	GradientData restored;
	restored.FromJson(nlohmann::json::parse(node.dump())["props"]["keys"]);
	assert(restored.ToJson() == gradient.ToJson());
	assert(restored.Sample(0.25f) == middle);

	gradient.keys = {{0.0f, {0, 0, 0, 0}}, {0.5f, {0, 0, 0, 0}},
		{0.5f, {1, 1, 1, 1}}, {1.0f, {1, 1, 1, 1}}};
	gradient.Normalize();
	assert(gradient.Sample(0.49f)[0] == 0.0f);
	assert(gradient.Sample(0.5f)[0] == 1.0f);
	assert(gradient.Sample(0.51f)[0] == 1.0f);

	restored.FromJson(nlohmann::json::array());
	assert(restored.keys.size() == 2);
	assert(restored.Sample(0.5f)[0] == 0.5f);
	gradient.keys.resize(12);
	gradient.keys[0].position = std::numeric_limits<float>::quiet_NaN();
	gradient.keys[0].color[3] = 2.0f;
	gradient.Normalize();
	assert(gradient.keys.size() == GradientData::kMaxKeys);
	for (const auto& key : gradient.keys) {
		assert(std::isfinite(key.position) && key.position >= 0.0f && key.position <= 1.0f);
		assert(key.color[3] >= 0.0f && key.color[3] <= 1.0f);
	}
	std::cout << "Gradient interpolation, alpha, duplicate keys and JSON round trip passed.\n";
}
