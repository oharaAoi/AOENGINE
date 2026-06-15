#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "Engine/Lib/Math/Vector2.h"
#include "Externals/ImGui/imstb_truetype.h"

namespace AOENGINE {

class FontManager {
public:
	struct TextTexture {
		std::string textureName;
		Math::Vector2 size;
	};

	static FontManager* GetInstance();

	bool LoadFont(const std::string& fontPath);
	TextTexture CreateTextTexture(const std::string& text, const std::string& fontPath, float fontSize, const std::string& textureName);

	const std::vector<std::string>& GetFontPaths();
	void ClearFontPathCache();

	static std::string GetDefaultFontPath();

private:
	struct FontData {
		std::vector<unsigned char> bytes;
		stbtt_fontinfo info{};
	};

	FontData* GetFont(const std::string& fontPath);
	std::string ResolveFontPath(const std::string& fontPath);
	void ScanFontDirectory(const std::string& directoryPath);

	std::unordered_map<std::string, FontData> fonts_;
	std::vector<std::string> fontPaths_;
};

}
