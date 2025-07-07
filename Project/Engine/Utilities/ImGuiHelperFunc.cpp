#include "ImGuiHelperFunc.h"
#include "imgui.h"

bool InputTextWithString(const char* label, std::string& str, size_t maxLength) {
	// std::vector<char> をバッファとして使用
	std::vector<char> buffer(str.begin(), str.end());
	buffer.resize(maxLength); // 必要なサイズにリサイズ

	// ImGui入力フィールド
	bool changed = ImGui::InputText(label, buffer.data(), buffer.size());

	if (changed) {
		str = buffer.data(); // 入力された文字列をstd::stringに反映
	}

	return changed;
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
