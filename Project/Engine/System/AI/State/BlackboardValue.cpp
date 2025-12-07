#include "BlackboardValue.h"
#include "Engine/System/Manager/ImGuiManager.h"

using json = nlohmann::json;
using namespace AI;

json BlackboardValue::to_json(const BlackboardValue& v) const {
    json result;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int32_t>) {
            result["type"] = "int";
            result["value"] = arg;
        } else if constexpr (std::is_same_v<T, int32_t*>) {
            result["type"] = "int";
            result["value"] = *arg;               // ★参照先の値を書き込む
        } else if constexpr (std::is_same_v<T, float>) {
            result["type"] = "float";
            result["value"] = arg;
        } else if constexpr (std::is_same_v<T, float*>) {
            result["type"] = "float";
            result["value"] = *arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result["type"] = "bool";
            result["value"] = arg;
        } else if constexpr (std::is_same_v<T, bool*>) {
            result["type"] = "bool";
            result["value"] = *arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result["type"] = "string";
            result["value"] = arg;
        } else if constexpr (std::is_same_v<T, std::string*>) {
            result["type"] = "string";
            result["value"] = *arg;
        }

               }, v.Get());

    return result;
}

void BlackboardValue::from_json(const json& j, BlackboardValue& v) {
    std::string type = j.at("type");
    if (type == "int") {
        v = BlackboardValue(j.at("value").get<int>());
    } else if (type == "float") {
        v = BlackboardValue(j.at("value").get<float>());
    } else if (type == "bool") {
        v = BlackboardValue(j.at("value").get<bool>());
    } else if (type == "string") {
        v = BlackboardValue(j.at("value").get<std::string>());
    }
}

void BlackboardValue::DebugValue(const std::string& name, BlackboardValue& wv) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, int32_t>) {
            int32_t v = arg;
            if (ImGui::DragInt(name.c_str(), &v)) {
                wv.value_ = v;
            }
        } else if constexpr (std::is_same_v<T, int32_t*>) {
            if (arg) {
                ImGui::DragInt(name.c_str(), arg);
            } else {
                ImGui::Text("%s : null", name.c_str());
            }
        } else if constexpr (std::is_same_v<T, float>) {
            float v = arg;
            if (ImGui::DragFloat(name.c_str(), &v, 0.01f)) {
                wv.value_ = v;
            }
        } else if constexpr (std::is_same_v<T, float*>) {
            if (arg) {
                ImGui::DragFloat(name.c_str(), arg, 0.01f);
            } else {
                ImGui::Text("%s : null", name.c_str());
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            bool v = arg;
            if (ImGui::Checkbox(name.c_str(), &v)) {
                wv.value_ = v;
            }
        } else if constexpr (std::is_same_v<T, bool*>) {
            if (arg) {
                ImGui::Checkbox(name.c_str(), arg);
            } else {
                ImGui::Text("%s : null", name.c_str());
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::string v = arg;
            char buf[256];
            strcpy_s(buf, v.c_str());
            if (ImGui::InputText(name.c_str(), buf, sizeof(buf))) {
                wv.value_ = std::string(buf);
            }
        } else if constexpr (std::is_same_v<T, std::string*>) {
            if (arg) {
                char buf[256];
                strcpy_s(buf, arg->c_str());
                if (ImGui::InputText(name.c_str(), buf, sizeof(buf))) {
                    *arg = buf;
                }
            } else {
                ImGui::Text("%s : null", name.c_str());
            }
        }

               }, wv.value_);
}