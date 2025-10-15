#include "Curve.h"
#include "imgui_curve_bezier.h"

float Curve::BezierValue(float dt01) const {
    enum { STEPS = 256 };
    Vector2 Q[4] = { {0, 0}, {controlPoints_[0], controlPoints_[1]}, {controlPoints_[2], controlPoints_[3]}, {1, 1} };
    Vector2 results[STEPS + 1];
    bezier_table<STEPS>(Q, results);
    return results[(int)((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * (float)STEPS)].y;
}

void Curve::Debug_Gui() {
    ImGui::Bezier("curve label", controlPoints_);
}

json Curve::ToJson() const {
    return {
        {"controlPoints_1", toJson(controlPoints_[0])},
        {"controlPoints_2", toJson(controlPoints_[1])},
        {"controlPoints_3", toJson(controlPoints_[2])},
        {"controlPoints_4", toJson(controlPoints_[3])},
    };
}

void Curve::FromJson(const json& _json, const std::string& _name) {
    json curve;

    if (_json.is_object()) {
        // 最上位キー（例: "ActionQuickBoost"）を取得
        auto rootKey = _json.begin().key();

        // rootKey の下に _name ("decelerationCurve") があるか確認
        if (_json.at(rootKey).contains(_name)) {
            // curve に "decelerationCurve" をキーごと保持
            curve[_name] = _json.at(rootKey).at(_name);
        } else {
            return;
        }
    }

    fromJson(curve, "controlPoints_1", controlPoints_[0]);
    fromJson(curve, "controlPoints_2", controlPoints_[1]);
    fromJson(curve, "controlPoints_3", controlPoints_[2]);
    fromJson(curve, "controlPoints_4", controlPoints_[3]);
}