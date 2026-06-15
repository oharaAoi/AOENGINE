#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "FontManager.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include "Engine/Lib/Path.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

namespace {

std::string NormalizePath(std::string path) {
	std::replace(path.begin(), path.end(), '\\', '/');
	return path;
}

std::vector<uint32_t> DecodeUtf8(const std::string& text) {
	std::vector<uint32_t> result;
	for (size_t i = 0; i < text.size();) {
		const unsigned char c = static_cast<unsigned char>(text[i]);
		if (c < 0x80) {
			result.push_back(c);
			++i;
		} else if ((c & 0xE0) == 0xC0 && i + 1 < text.size()) {
			const unsigned char c1 = static_cast<unsigned char>(text[i + 1]);
			result.push_back(((c & 0x1F) << 6) | (c1 & 0x3F));
			i += 2;
		} else if ((c & 0xF0) == 0xE0 && i + 2 < text.size()) {
			const unsigned char c1 = static_cast<unsigned char>(text[i + 1]);
			const unsigned char c2 = static_cast<unsigned char>(text[i + 2]);
			result.push_back(((c & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F));
			i += 3;
		} else if ((c & 0xF8) == 0xF0 && i + 3 < text.size()) {
			const unsigned char c1 = static_cast<unsigned char>(text[i + 1]);
			const unsigned char c2 = static_cast<unsigned char>(text[i + 2]);
			const unsigned char c3 = static_cast<unsigned char>(text[i + 3]);
			result.push_back(((c & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F));
			i += 4;
		} else {
			result.push_back('?');
			++i;
		}
	}
	return result;
}

std::vector<std::vector<uint32_t>> SplitLines(const std::string& text) {
	std::vector<std::vector<uint32_t>> lines(1);
	for (uint32_t cp : DecodeUtf8(text)) {
		if (cp == '\r') {
			continue;
		}
		if (cp == '\n') {
			lines.emplace_back();
			continue;
		}
		lines.back().push_back(cp);
	}
	return lines;
}

uint32_t ResolveCodepoint(const stbtt_fontinfo& info, uint32_t codepoint) {
	if (stbtt_FindGlyphIndex(&info, static_cast<int>(codepoint)) != 0) {
		return codepoint;
	}
	if (stbtt_FindGlyphIndex(&info, '?') != 0) {
		return '?';
	}
	return codepoint;
}

float MeasureLine(const stbtt_fontinfo& info, const std::vector<uint32_t>& line, float scale) {
	float width = 0.0f;
	for (size_t i = 0; i < line.size(); ++i) {
		const uint32_t cp = ResolveCodepoint(info, line[i]);
		int advance = 0;
		int bearing = 0;
		stbtt_GetCodepointHMetrics(&info, static_cast<int>(cp), &advance, &bearing);
		width += static_cast<float>(advance) * scale;

		if (i + 1 < line.size()) {
			const uint32_t nextCp = ResolveCodepoint(info, line[i + 1]);
			width += static_cast<float>(stbtt_GetCodepointKernAdvance(&info, static_cast<int>(cp), static_cast<int>(nextCp))) * scale;
		}
	}
	return width;
}

bool IsFontExtension(const std::filesystem::path& path) {
	std::string ext = path.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
				   });
	return ext == ".ttf" || ext == ".otf" || ext == ".ttc";
}

}

FontManager* FontManager::GetInstance() {
	static FontManager instance;
	return &instance;
}

bool FontManager::LoadFont(const std::string& fontPath) {
	const std::string resolvedPath = ResolveFontPath(fontPath);
	if (resolvedPath.empty()) {
		return false;
	}

	if (fonts_.contains(resolvedPath)) {
		return true;
	}

	std::ifstream file(resolvedPath, std::ios::binary);
	if (!file) {
		AOENGINE::Logger::Log("[Load][Font] Failed to open: " + resolvedPath);
		return false;
	}

	FontData data{};
	file.seekg(0, std::ios::end);
	const std::streamoff fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	if (fileSize <= 0) {
		AOENGINE::Logger::Log("[Load][Font] Empty font file: " + resolvedPath);
		return false;
	}

	data.bytes.resize(static_cast<size_t>(fileSize));
	file.read(reinterpret_cast<char*>(data.bytes.data()), fileSize);

	const int offset = stbtt_GetFontOffsetForIndex(data.bytes.data(), 0);
	if (offset < 0 || stbtt_InitFont(&data.info, data.bytes.data(), offset) == 0) {
		AOENGINE::Logger::Log("[Load][Font] Invalid font file: " + resolvedPath);
		return false;
	}

	fonts_[resolvedPath] = std::move(data);
	return true;
}

FontManager::TextTexture FontManager::CreateTextTexture(const std::string& text, const std::string& fontPath, float fontSize, const std::string& textureName) {
	TextTexture result{};
	result.textureName = "white.png";
	result.size = { 1.0f, 1.0f };

	FontData* font = GetFont(fontPath);
	if (font == nullptr) {
		return result;
	}

	fontSize = std::clamp(fontSize, 1.0f, 512.0f);
	const float scale = stbtt_ScaleForPixelHeight(&font->info, fontSize);

	int ascent = 0;
	int descent = 0;
	int lineGap = 0;
	stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &lineGap);

	const int lineHeight = (std::max)(1, static_cast<int>(std::ceil(static_cast<float>(ascent - descent + lineGap) * scale)));
	const int baselineOffset = static_cast<int>(std::ceil(static_cast<float>(ascent) * scale));
	const int padding = (std::max)(2, static_cast<int>(std::ceil(fontSize * 0.15f)));

	std::vector<std::vector<uint32_t>> lines = SplitLines(text);
	float maxLineWidth = 0.0f;
	for (const auto& line : lines) {
		maxLineWidth = (std::max)(maxLineWidth, MeasureLine(font->info, line, scale));
	}

	const uint32_t width = (std::max<uint32_t>)(1, static_cast<uint32_t>(std::ceil(maxLineWidth)) + static_cast<uint32_t>(padding * 2));
	const uint32_t height = (std::max<uint32_t>)(1, static_cast<uint32_t>(lineHeight * static_cast<int>(lines.size()) + padding * 2));

	std::vector<unsigned char> alpha(static_cast<size_t>(width) * static_cast<size_t>(height), 0);

	for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
		float penX = static_cast<float>(padding);
		const int baseline = padding + baselineOffset + static_cast<int>(lineIndex) * lineHeight;
		const std::vector<uint32_t>& line = lines[lineIndex];

		for (size_t i = 0; i < line.size(); ++i) {
			const uint32_t cp = ResolveCodepoint(font->info, line[i]);

			int x0 = 0;
			int y0 = 0;
			int x1 = 0;
			int y1 = 0;
			stbtt_GetCodepointBitmapBox(&font->info, static_cast<int>(cp), scale, scale, &x0, &y0, &x1, &y1);

			const int glyphWidth = x1 - x0;
			const int glyphHeight = y1 - y0;
			const int glyphX = static_cast<int>(std::round(penX)) + x0;
			const int glyphY = baseline + y0;

			if (glyphWidth > 0 && glyphHeight > 0) {
				std::vector<unsigned char> glyph(static_cast<size_t>(glyphWidth) * static_cast<size_t>(glyphHeight), 0);
				stbtt_MakeCodepointBitmap(&font->info, glyph.data(), glyphWidth, glyphHeight, glyphWidth, scale, scale, static_cast<int>(cp));

				for (int y = 0; y < glyphHeight; ++y) {
					const int dstY = glyphY + y;
					if (dstY < 0 || dstY >= static_cast<int>(height)) {
						continue;
					}
					for (int x = 0; x < glyphWidth; ++x) {
						const int dstX = glyphX + x;
						if (dstX < 0 || dstX >= static_cast<int>(width)) {
							continue;
						}
						const size_t dstIndex = static_cast<size_t>(dstY) * width + static_cast<size_t>(dstX);
						const size_t srcIndex = static_cast<size_t>(y) * glyphWidth + static_cast<size_t>(x);
						alpha[dstIndex] = (std::max)(alpha[dstIndex], glyph[srcIndex]);
					}
				}
			}

			int advance = 0;
			int bearing = 0;
			stbtt_GetCodepointHMetrics(&font->info, static_cast<int>(cp), &advance, &bearing);
			penX += static_cast<float>(advance) * scale;

			if (i + 1 < line.size()) {
				const uint32_t nextCp = ResolveCodepoint(font->info, line[i + 1]);
				penX += static_cast<float>(stbtt_GetCodepointKernAdvance(&font->info, static_cast<int>(cp), static_cast<int>(nextCp))) * scale;
			}
		}
	}

	std::vector<uint8_t> rgba(static_cast<size_t>(width) * static_cast<size_t>(height) * 4ull, 255);
	for (size_t i = 0; i < alpha.size(); ++i) {
		rgba[i * 4 + 0] = 255;
		rgba[i * 4 + 1] = 255;
		rgba[i * 4 + 2] = 255;
		rgba[i * 4 + 3] = alpha[i];
	}

	if (!AOENGINE::TextureManager::GetInstance()->CreateTextureFromRGBA8(textureName, rgba, width, height, true)) {
		return result;
	}

	result.textureName = textureName;
	result.size = { static_cast<float>(width), static_cast<float>(height) };
	return result;
}

const std::vector<std::string>& FontManager::GetFontPaths() {
	if (fontPaths_.empty()) {
		ScanFontDirectory(kAssetPath + "/Engine/Font");
	}
	return fontPaths_;
}

void FontManager::ClearFontPathCache() {
	fontPaths_.clear();
}

std::string FontManager::GetDefaultFontPath() {
	return NormalizePath(kAssetPath + "/Engine/Font/Cica-Regular.ttf");
}

FontManager::FontData* FontManager::GetFont(const std::string& fontPath) {
	const std::string resolvedPath = ResolveFontPath(fontPath);
	if (resolvedPath.empty()) {
		return nullptr;
	}

	if (!LoadFont(resolvedPath)) {
		return nullptr;
	}
	return &fonts_.at(resolvedPath);
}

std::string FontManager::ResolveFontPath(const std::string& fontPath) {
	const std::string normalizedPath = NormalizePath(fontPath);
	if (!normalizedPath.empty() && std::filesystem::exists(normalizedPath)) {
		return normalizedPath;
	}

	const std::vector<std::string>& paths = GetFontPaths();
	for (const std::string& path : paths) {
		const std::filesystem::path fsPath(path);
		if (fsPath.filename().string() == fontPath || fsPath.stem().string() == fontPath || NormalizePath(path) == normalizedPath) {
			return NormalizePath(path);
		}
	}

	const std::string defaultPath = GetDefaultFontPath();
	if (std::filesystem::exists(defaultPath)) {
		return defaultPath;
	}

	if (!paths.empty()) {
		return NormalizePath(paths.front());
	}

	AOENGINE::Logger::Log("[Load][Font] Font not found: " + fontPath);
	return "";
}

void FontManager::ScanFontDirectory(const std::string& directoryPath) {
	fontPaths_.clear();
	if (!std::filesystem::exists(directoryPath)) {
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
		if (!entry.is_regular_file() || !IsFontExtension(entry.path())) {
			continue;
		}
		fontPaths_.push_back(NormalizePath(entry.path().string()));
	}

	std::sort(fontPaths_.begin(), fontPaths_.end());
}
