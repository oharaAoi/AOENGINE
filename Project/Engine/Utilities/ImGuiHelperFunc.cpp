#include "ImGuiHelperFunc.h"
#include "imgui.h"
#include "ImGuiFileDialog.h"

bool InputTextWithString(const char* filedName, const char* label, std::string& str, size_t maxLength) {
	// std::vector<char> をバッファとして使用
	std::vector<char> buffer(str.begin(), str.end());
	buffer.resize(maxLength); // 必要なサイズにリサイズ

	// ImGui入力フィールド
	ImGui::Text(filedName);  // 左側に表示
	ImGui::SameLine();        // 同じ行に描画
	bool changed = ImGui::InputText(label, buffer.data(), buffer.size());

	if (changed) {
		str = buffer.data(); // 入力された文字列をstd::stringに反映
	}

	return changed;
}

bool ButtonOpenDialog(const char* buttonLabel, const char* dialogKey, const char* windowTitle, const char* filter, std::string& outPath) {
	static bool isDialogOpen = false;

	if (ImGui::Button(buttonLabel)) {
		IGFD::FileDialogConfig config;
		config.path = "./Game/Assets/";
		ImGuiFileDialog::Instance()->OpenDialog(dialogKey, windowTitle, filter, config);
		isDialogOpen = true;
	}

	if (isDialogOpen && ImGuiFileDialog::Instance()->Display(dialogKey)) {
		isDialogOpen = false;

		if (ImGuiFileDialog::Instance()->IsOk()) {
			outPath = ImGuiFileDialog::Instance()->GetFilePathName();
			ImGuiFileDialog::Instance()->Close();
			return true; // OKボタンで選択された
		}

		// Cancelが押された、もしくはウィンドウを閉じた
		ImGuiFileDialog::Instance()->Close();
		return false;
	}

	return false;
}

int ContainerOfComb(const std::vector<std::string>& items, int& selectedIndex, const char* label) {
	std::vector<const char*> itemPtrs;
	itemPtrs.reserve(items.size());

	for (const auto& str : items) {
		itemPtrs.push_back(str.c_str());
	}

	if (ImGui::Combo(label, &selectedIndex, itemPtrs.data(), static_cast<int>(itemPtrs.size()))) {
		return selectedIndex; // 選択が変更された
	}

	return -1; // 選択は変更されなかった
}
