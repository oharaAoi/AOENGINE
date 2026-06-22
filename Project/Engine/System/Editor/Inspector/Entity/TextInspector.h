#pragma once

#include "Engine/Module/Components/2d/Text.h"

namespace AOENGINE {

/// <summary>
/// Textの編集UIを描画するEditor専用Inspector。
/// </summary>
class TextInspector {
public:
	/// <summary>
	/// Text全体のInspectorを描画する。
	/// </summary>
	static void Draw(Text& text);

private:
	static void DrawTransform(ScreenTransform& transform);
	static void DrawText(Text& text);
	static void DrawFont(Text& text);
	static void DrawSaveLoad(Text& text);
};

}
