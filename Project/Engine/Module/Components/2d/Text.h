#pragma once
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/System/Manager/FontManager.h"

namespace AOENGINE {

struct TextParameter : public AOENGINE::IJsonConverter {
	bool isActive = true;
	Math::SRT transform = Math::SRT();
	std::string text = "New Text";
	std::string fontPath = AOENGINE::FontManager::GetDefaultFontPath();
	float fontSize = 32.0f;
	AOENGINE::Color color = Colors::Linear::white;
	Math::Vector2 anchorPoint = Math::Vector2(0.5f, 0.5f);
	int renderQueue = 0;
	uint32_t windowWidth = 1280;
	uint32_t windowHeight = 720;

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("isActive", isActive)
			.Add("transform", transform)
			.Add("text", text)
			.Add("fontPath", fontPath)
			.Add("fontSize", fontSize)
			.Add("color", color)
			.Add("anchorPoint", anchorPoint)
			.Add("renderQueue", renderQueue)
			.Add("windowWidth", windowWidth)
			.Add("windowHeight", windowHeight)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "isActive", isActive);
		Convert::fromJson(jsonData, "transform", transform);
		Convert::fromJson(jsonData, "text", text);
		Convert::fromJson(jsonData, "fontPath", fontPath);
		Convert::fromJson(jsonData, "fontSize", fontSize);
		Convert::fromJson(jsonData, "color", color);
		Convert::fromJson(jsonData, "anchorPoint", anchorPoint);
		Convert::fromJson(jsonData, "renderQueue", renderQueue);
		Convert::fromJson(jsonData, "windowWidth", windowWidth);
		Convert::fromJson(jsonData, "windowHeight", windowHeight);
	}

	void Debug_Gui() override {};
};

class Text : public AOENGINE::Sprite {
public:
	Text();
	~Text() override = default;

	void Init(const std::string& text = "New Text", const std::string& fontPath = "", float fontSize = 32.0f);

	void Update() override;
	void Debug_Gui() override;
	void Resize() override;
	void ApplyParam() override;
	void Load(const std::string& _group, const std::string& _key) override;
	void Save(const std::string& _group, const std::string& _key) override;

	const char* GetCanvasItemType() const override { return "Text"; }

	void SetText(const std::string& text);
	void SetFontPath(const std::string& fontPath);
	void SetFontSize(float fontSize);
	void SetTextColor(const AOENGINE::Color& color);
	void SetTextAnchorPoint(const Math::Vector2& anchorPoint);
	void RequestRebuild() { isTextDirty_ = true; }

	const std::string& GetText() const { return text_; }
	const std::string& GetFontPath() const { return fontPath_; }
	float GetFontSize() const { return fontSize_; }
	const AOENGINE::Color& GetTextColor() const { return color_; }
	const Math::Vector2& GetTextAnchorPoint() const { return anchorPoint_; }

	void SetSaveGroupName(const std::string& groupName) { saveGroupName_ = groupName; }
	const std::string& GetSaveGroupName() const { return saveGroupName_; }
	void SetSaveKeyName(const std::string& keyName) { saveKeyName_ = keyName; }
	const std::string& GetSaveKeyName() const { return saveKeyName_; }

private:
	void RebuildTexture();

	std::string text_ = "New Text";
	std::string fontPath_ = AOENGINE::FontManager::GetDefaultFontPath();
	std::string generatedTextureName_;
	float fontSize_ = 32.0f;
	AOENGINE::Color color_ = Colors::Linear::white;
	Math::Vector2 anchorPoint_ = Math::Vector2(0.5f, 0.5f);
	bool isTextDirty_ = true;

	AOENGINE::TextParameter saveParam_;
	std::string saveGroupName_;
	std::string saveKeyName_;
};

}
