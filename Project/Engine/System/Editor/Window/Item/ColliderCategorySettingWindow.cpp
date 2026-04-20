#include "ColliderCategorySettingWindow.h"
#include <fstream>
#include <iostream>
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Utilities/Logger.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::ColliderCategorySettingWindow::ColliderCategorySettingWindow() {
	name_ = "Collider Category";
	isActive_ = false;
}

void AOENGINE::ColliderCategorySettingWindow::Init() {
	// categoryが保存されているファイルを読み込む
	std::ifstream file("./Project/Packages/Game/GameData/collisionCategory.txt");

	if (!file.is_open()) {
		AOENGINE::Logger::AssertLog("ColliderCategory.texを開けませんでした。");
	}

	std::string line;

	while (std::getline(file, line)) {
		if (!line.empty()) {
			categoties_.push_back(line);
		}
	}

	file.close();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::ColliderCategorySettingWindow::Edit() {
	if (ImGui::Begin("Collider Category", &isActive_)) {
		// 保存
		if (ImGui::Button("save")) {
			Save("./Project/Packages/Game/GameData/collisionCategory.txt");
		}

		// categoryの追加
		static std::string addCategoryName = "";
		InputTextWithString("Add Collider", "##add collider category", addCategoryName);
		ImGui::SameLine();
		if (ImGui::Button("+")) {
			categoties_.push_back(addCategoryName);
		}

		// リストの表示
		if (ImGui::BeginTable("CategoryTable", 2,
							  ImGuiTableFlags_Borders |
							  ImGuiTableFlags_RowBg |
							  ImGuiTableFlags_SizingStretchSame)) {
			// 列設定
			ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 40.0f);

			for (int i = 0; i < (int)categoties_.size(); ++i) {
				ImGui::TableNextRow();

				ImGui::PushID(i);

				// ===== 名前列 =====
				ImGui::TableSetColumnIndex(0);

				ImGui::Text(categoties_[i].c_str());

				// ===== ボタン列 =====
				ImGui::TableSetColumnIndex(1);

				if (ImGui::Button("-")) {
					categoties_.erase(categoties_.begin() + i);

					ImGui::PopID();
					break; // ← 超重要
				}

				ImGui::PopID();
			}

			ImGui::EndTable();
		}
	}


	ImGui::End();
}

void AOENGINE::ColliderCategorySettingWindow::Save(const std::string& path) {
	std::ofstream file(path, std::ios::trunc); // ← 上書きモード
	if (!file.is_open()) return;

	for (const auto& str : categoties_) {
		if (!str.empty()) {
			file << str << "\n";
		}
	}
}
