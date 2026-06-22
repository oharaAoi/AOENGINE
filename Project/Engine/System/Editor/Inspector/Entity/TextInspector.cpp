#include "TextInspector.h"

#include "Engine/System/Manager/FontManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

using namespace AOENGINE;

//////////////////////////////////////////////////////////////////////////////////////////////////
// Text全体のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void TextInspector::Draw(Text& text) {
	int renderQueue = text.GetRenderQueue();
	if (ImGui::DragInt("renderQueue", &renderQueue, 1)) {
		text.SetRenderQueue(renderQueue);
	}

	if (ScreenTransform* transform = text.GetTransform()) {
		DrawTransform(*transform);
	}

	DrawText(text);
	DrawFont(text);

	AOENGINE::Color color = text.GetTextColor();
	if (ImGui::ColorEdit4("Color", &color.r)) {
		text.SetTextColor(color);
	}

	Math::Vector2 anchorPoint = text.GetTextAnchorPoint();
	if (ImGui::DragFloat2("anchorPoint", &anchorPoint.x, 0.01f, 0.0f, 1.0f)) {
		text.SetTextAnchorPoint(anchorPoint);
	}

	if (ImGui::Button("Rebuild")) {
		text.RequestRebuild();
	}

	DrawSaveLoad(text);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 2D TransformのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void TextInspector::DrawTransform(ScreenTransform& transform) {
	if (!ImGui::TreeNode("transform")) {
		return;
	}

	Math::SRT srt = transform.GetTransform();
	bool changed = false;
	changed |= ImGui::DragFloat3("translation", &srt.translate.x, 0.1f);
	changed |= ImGui::DragFloat2("scale", &srt.scale.x, 0.01f);
	changed |= ImGui::SliderAngle("rotation", &srt.rotate.z);
	if (changed) {
		transform.SetTransform(srt);
	}

	ImGui::TreePop();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Text本文のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void TextInspector::DrawText(Text& text) {
	constexpr size_t kMaxTextLength = 4096;
	std::vector<char> textBuffer(kMaxTextLength, '\0');
	std::memcpy(textBuffer.data(), text.GetText().c_str(), (std::min)(text.GetText().size(), kMaxTextLength - 1));

	ImGui::Text("Text");
	if (ImGui::InputTextMultiline("##text", textBuffer.data(), textBuffer.size(), ImVec2(ImGui::GetContentRegionAvail().x, 90.0f))) {
		text.SetText(textBuffer.data());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Font関連のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void TextInspector::DrawFont(Text& text) {
	const std::vector<std::string>& fontPaths = AOENGINE::FontManager::GetInstance()->GetFontPaths();
	std::string selectedFontName = std::filesystem::path(text.GetFontPath()).filename().string();
	if (selectedFontName.empty()) {
		selectedFontName = text.GetFontPath();
	}

	if (ImGui::BeginCombo("Font", selectedFontName.c_str())) {
		for (const std::string& path : fontPaths) {
			const std::string fileName = std::filesystem::path(path).filename().string();
			const bool selected = path == text.GetFontPath();
			if (ImGui::Selectable(fileName.c_str(), selected)) {
				text.SetFontPath(path);
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	std::string fontPath = text.GetFontPath();
	if (InputTextWithString("FontPath", "##fontPath", fontPath, 512, 260.0f)) {
		text.SetFontPath(fontPath);
	}

	float fontSize = text.GetFontSize();
	if (ImGui::DragFloat("FontSize", &fontSize, 1.0f, 1.0f, 512.0f)) {
		text.SetFontSize(fontSize);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Text単体のSave/Load UIを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void TextInspector::DrawSaveLoad(Text& text) {
	if (!ImGui::CollapsingHeader("Save & Load")) {
		return;
	}

	std::string groupName = text.GetSaveGroupName();
	if (InputTextWithString("GroupName", "##textGroup", groupName)) {
		text.SetSaveGroupName(groupName);
	}

	std::string keyName = text.GetSaveKeyName();
	if (InputTextWithString("KeyName", "##textKey", keyName)) {
		text.SetSaveKeyName(keyName);
	}

	if (ImGui::Button("Save")) {
		text.Save(text.GetSaveGroupName(), text.GetSaveKeyName());
	}
	if (ImGui::Button("Load")) {
		text.Load(text.GetSaveGroupName(), text.GetSaveKeyName());
	}
}
