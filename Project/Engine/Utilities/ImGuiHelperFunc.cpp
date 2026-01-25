#include "ImGuiHelperFunc.h"

bool InputTextWithString(const char* filedName, const char* label, std::string& str, size_t maxLength, float inputFiledSize) {
	// std::vector<char> をバッファとして使用
	std::vector<char> buffer(str.begin(), str.end());
	buffer.resize(maxLength); // 必要なサイズにリサイズ

	// ImGui入力フィールド
	ImGui::Text(filedName);  // 左側に表示
	ImGui::SameLine();        // 同じ行に描画
	ImGui::SetNextItemWidth(inputFiledSize);
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

int ContainerOfComb(const std::vector<std::string>& items, int& selectedIndex, const char* label, float inputFiledSize) {
	std::vector<const char*> itemPtrs;
	itemPtrs.reserve(items.size());

	for (const auto& str : items) {
		itemPtrs.push_back(str.c_str());
	}

	ImGui::SetNextItemWidth(inputFiledSize);
	if (ImGui::Combo(label, &selectedIndex, itemPtrs.data(), static_cast<int>(itemPtrs.size()))) {
		return selectedIndex; // 選択が変更された
	}

	return selectedIndex; // 選択は変更されなかった
}

std::string EllipsisText(const std::string& text, float maxWidth) {
	ImFont* font = ImGui::GetFont();
	float fontSize = ImGui::GetFontSize();

	// すでに収まるならそのまま
	if (ImGui::CalcTextSize(text.c_str()).x <= maxWidth) {
		return text;
	}

	const char* ellipsis = "...";
	float ellipsisWidth = ImGui::CalcTextSize(ellipsis).x;

	std::string result;
	for (size_t i = 0; i < text.size(); ++i) {
		result += text[i];
		float w = ImGui::CalcTextSize(result.c_str()).x;
		if (w + ellipsisWidth > maxWidth) {
			result += ellipsis;
			break;
		}
	}
	return result;
}

bool DrawImageButtonWithLabel(ImTextureID tex, const std::string& label, ImVec2 size) {
	ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

	// 実際に必要な横幅 = 画像幅と文字幅の大きい方
	float itemWidth = std::max(size.x, textSize.x);

	// スペース
	float spacing = ImGui::GetStyle().ItemSpacing.x;

	// 残り幅
	float availX = ImGui::GetContentRegionAvail().x;

	// 折り返し判定
	if (itemWidth > availX) {
		ImGui::NewLine();
	}

	ImGui::BeginGroup();

	// 一意なID
	std::string id = "##" + label;

	// 画像ボタン
	if (ImGui::ImageButton(id.c_str(), tex, size)) {
		ImGui::EndGroup();
		return true;
	}

	// ラベル中央寄せ
	float textPosX = ImGui::GetCursorPosX() + (itemWidth - textSize.x) * 0.5f;
	ImGui::SetCursorPosX(textPosX);
	ImGui::TextUnformatted(label.c_str());

	ImGui::EndGroup();

	// 横に並べる（被らない）
	ImGui::SameLine(0.0f, spacing);

	return false;
}
