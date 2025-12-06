#include "Profiler.h"
#include "Engine/System/Manager/ImGuiManager.h"

using namespace AOENGINE;

void Profiler::Start() {
	start_ = std::chrono::high_resolution_clock::now();
}

void Profiler::End(const std::string& _name) {
	auto end = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration<double, std::milli>(end - start_).count();

	timeQueue_.push(std::make_pair(_name, ms));
}

void Profiler::DisPlay() {
    if (ImGui::Begin("Profile")) {
        if (ImGui::BeginTable("TimeQueueTable", 2)) {
            ImGui::TableSetupColumn("Process Name");
            ImGui::TableSetupColumn("Time (ms)");
            ImGui::TableHeadersRow();

            // timeQueue_を破壊しながらループ
            while (!timeQueue_.empty()) {
                auto& [name, time] = timeQueue_.front();

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%.3f", time);

                timeQueue_.pop(); // ここで都度削除
            }

            ImGui::EndTable();
        }
    }
    ImGui::End();
}
