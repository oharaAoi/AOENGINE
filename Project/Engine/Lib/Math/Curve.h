#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Curve :
	public AttributeGui {
public:

	Curve() = default;
	~Curve() override = default;

	template<int steps>
	void bezier_table(Vector2 P[4], Vector2 results[steps + 1]) const {
		static float C[(steps + 1) * 4], * K = 0;
		if (!K) {
			K = C;
			for (unsigned step = 0; step <= steps; ++step) {
				float t = (float)step / (float)steps;
				C[step * 4 + 0] = (1 - t) * (1 - t) * (1 - t);
				C[step * 4 + 1] = 3 * (1 - t) * (1 - t) * t;
				C[step * 4 + 2] = 3 * (1 - t) * t * t;
				C[step * 4 + 3] = t * t * t;
			}
		}
		for (unsigned step = 0; step <= steps; ++step) {
			Vector2 point = {
				K[step * 4 + 0] * P[0].x + K[step * 4 + 1] * P[1].x +
				K[step * 4 + 2] * P[2].x + K[step * 4 + 3] * P[3].x,
				K[step * 4 + 0] * P[0].y + K[step * 4 + 1] * P[1].y +
				K[step * 4 + 2] * P[2].y + K[step * 4 + 3] * P[3].y
			};
			results[step] = point;
		}
	}

	float BezierValue(float dt01) const;

	void Debug_Gui() override;

	json ToJson() const;
	void FromJson(const json& _json, const std::string& _name);

public:

	float controlPoints_[4] = { 0, 0, 1,1 };

};

