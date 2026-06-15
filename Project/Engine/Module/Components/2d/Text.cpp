#include "Text.h"
#include <algorithm>
#include <cfloat>
#include <cstring>
#include <filesystem>
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/WinApp/WinApp.h"

using namespace AOENGINE;

Text::Text() {
	generatedTextureName_ = "__Text_" + std::to_string(GetID());
}

void Text::Init(const std::string& text, const std::string& fontPath, float fontSize) {
	Sprite::Init("white.png");
	text_ = text;
	fontPath_ = fontPath.empty() ? AOENGINE::FontManager::GetDefaultFontPath() : fontPath;
	fontSize_ = std::clamp(fontSize, 1.0f, 512.0f);
	color_ = Colors::Linear::white;
	anchorPoint_ = Math::Vector2(0.5f, 0.5f);
	isTextDirty_ = true;

	SetAnchorPoint(anchorPoint_);
	Sprite::SetColor(color_);
	RebuildTexture();
}

void Text::Update() {
	if (isTextDirty_) {
		RebuildTexture();
	}
	SetAnchorPoint(anchorPoint_);
	Sprite::SetColor(color_);
	Sprite::Update();
}

void Text::Debug_Gui() {
	int renderQueue = GetRenderQueue();
	if (ImGui::DragInt("renderQueue", &renderQueue, 1)) {
		SetRenderQueue(renderQueue);
	}

	GetTransform()->Debug_Gui();

	constexpr size_t kMaxTextLength = 4096;
	std::vector<char> textBuffer(kMaxTextLength, '\0');
	std::memcpy(textBuffer.data(), text_.c_str(), (std::min)(text_.size(), kMaxTextLength - 1));
	ImGui::Text("Text");
	if (ImGui::InputTextMultiline("##text", textBuffer.data(), textBuffer.size(), ImVec2(ImGui::GetContentRegionAvail().x, 90.0f))) {
		SetText(textBuffer.data());
	}

	const std::vector<std::string>& fontPaths = AOENGINE::FontManager::GetInstance()->GetFontPaths();
	std::string selectedFontName = std::filesystem::path(fontPath_).filename().string();
	if (selectedFontName.empty()) {
		selectedFontName = fontPath_;
	}

	if (ImGui::BeginCombo("Font", selectedFontName.c_str())) {
		for (const std::string& path : fontPaths) {
			const std::string fileName = std::filesystem::path(path).filename().string();
			const bool selected = (path == fontPath_);
			if (ImGui::Selectable(fileName.c_str(), selected)) {
				SetFontPath(path);
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (InputTextWithString("FontPath", "##fontPath", fontPath_, 512, 260.0f)) {
		isTextDirty_ = true;
	}

	if (ImGui::DragFloat("FontSize", &fontSize_, 1.0f, 1.0f, 512.0f)) {
		fontSize_ = std::clamp(fontSize_, 1.0f, 512.0f);
		isTextDirty_ = true;
	}

	if (ImGui::ColorEdit4("Color", &color_.r)) {
		Sprite::SetColor(color_);
	}

	if (ImGui::DragFloat2("anchorPoint", &anchorPoint_.x, 0.01f, 0.0f, 1.0f)) {
		anchorPoint_.x = std::clamp(anchorPoint_.x, 0.0f, 1.0f);
		anchorPoint_.y = std::clamp(anchorPoint_.y, 0.0f, 1.0f);
		SetAnchorPoint(anchorPoint_);
	}

	if (ImGui::Button("Rebuild")) {
		isTextDirty_ = true;
	}

	if (ImGui::CollapsingHeader("Save & Load")) {
		InputTextWithString("GroupName", "##textGroup", saveGroupName_);
		InputTextWithString("KeyName", "##textKey", saveKeyName_);

		if (ImGui::Button("Save")) {
			Save(saveGroupName_, saveKeyName_);
		}
		if (ImGui::Button("Load")) {
			Load(saveGroupName_, saveKeyName_);
		}
	}
}

void Text::Resize() {
	float scaleX = static_cast<float>(WinApp::sClientWidth) / static_cast<float>(WinApp::sWindowWidth);
	float scaleY = static_cast<float>(WinApp::sClientHeight) / static_cast<float>(WinApp::sWindowHeight);

	GetTransform()->SetTranslate({
		saveParam_.transform.translate.x * scaleX,
		saveParam_.transform.translate.y * scaleY });

	GetTransform()->SetScale({
		saveParam_.transform.scale.x * scaleX,
		saveParam_.transform.scale.y * scaleY
								 });
}

void Text::ApplyParam() {
	SetIsActive(saveParam_.isActive);
	GetTransform()->SetSRT(saveParam_.transform);
	text_ = saveParam_.text;
	fontPath_ = saveParam_.fontPath;
	fontSize_ = std::clamp(saveParam_.fontSize, 1.0f, 512.0f);
	color_ = saveParam_.color;
	anchorPoint_ = saveParam_.anchorPoint;
	SetRenderQueue(saveParam_.renderQueue);
	SetAnchorPoint(anchorPoint_);
	Sprite::SetColor(color_);
	isTextDirty_ = true;
}

void Text::Load(const std::string& _group, const std::string& _key) {
	SetName(_key);
	saveGroupName_ = _group;
	saveKeyName_ = _key;

	saveParam_.SetGroupName(_group);
	saveParam_.SetName(_key);
	saveParam_.Load();
	ApplyParam();

	Resize();
	RebuildTexture();
}

void Text::Save(const std::string& _group, const std::string& _key) {
	float scaleX = static_cast<float>(WinApp::sWindowWidth) / static_cast<float>(WinApp::sClientWidth);
	float scaleY = static_cast<float>(WinApp::sWindowHeight) / static_cast<float>(WinApp::sClientHeight);

	saveParam_.transform = GetTransform()->GetTransform();
	GetTransform()->SetTranslate({
		saveParam_.transform.translate.x * scaleX,
		saveParam_.transform.translate.y * scaleY });

	GetTransform()->SetScale({
		saveParam_.transform.scale.x * scaleX,
		saveParam_.transform.scale.y * scaleY
								 });

	saveParam_.isActive = GetIsActive();
	saveParam_.transform = GetTransform()->GetTransform();
	saveParam_.text = text_;
	saveParam_.fontPath = fontPath_;
	saveParam_.fontSize = fontSize_;
	saveParam_.color = color_;
	saveParam_.anchorPoint = anchorPoint_;
	saveParam_.renderQueue = GetRenderQueue();
	saveParam_.windowWidth = WinApp::sClientWidth;
	saveParam_.windowHeight = WinApp::sClientHeight;

	saveGroupName_ = _group;
	saveKeyName_ = _key;
	saveParam_.SetGroupName(_group);
	saveParam_.SetName(_key);
	saveParam_.Save();
}

void Text::SetText(const std::string& text) {
	if (text_ == text) {
		return;
	}
	text_ = text;
	isTextDirty_ = true;
}

void Text::SetFontPath(const std::string& fontPath) {
	if (fontPath_ == fontPath) {
		return;
	}
	fontPath_ = fontPath;
	isTextDirty_ = true;
}

void Text::SetFontSize(float fontSize) {
	fontSize = std::clamp(fontSize, 1.0f, 512.0f);
	if (fontSize_ == fontSize) {
		return;
	}
	fontSize_ = fontSize;
	isTextDirty_ = true;
}

void Text::SetTextColor(const AOENGINE::Color& color) {
	color_ = color;
	Sprite::SetColor(color_);
}

void Text::SetTextAnchorPoint(const Math::Vector2& anchorPoint) {
	anchorPoint_.x = std::clamp(anchorPoint.x, 0.0f, 1.0f);
	anchorPoint_.y = std::clamp(anchorPoint.y, 0.0f, 1.0f);
	SetAnchorPoint(anchorPoint_);
}

void Text::RebuildTexture() {
	AOENGINE::FontManager::TextTexture texture = AOENGINE::FontManager::GetInstance()->CreateTextTexture(text_, fontPath_, fontSize_, generatedTextureName_);
	ReSetTexture(texture.textureName);
	ReSetTextureSize(texture.size);
	SetDrawRange(texture.size);
	SetLeftTop(CMath::Vector2::ZERO);
	SetAnchorPoint(anchorPoint_);
	Sprite::SetColor(color_);
	isTextDirty_ = false;
}
