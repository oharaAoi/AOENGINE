#pragma once

#include "Engine/Module/Components/2d/Sprite.h"

namespace AOENGINE {

/// <summary>
/// Spriteの編集UIを描画するEditor専用Inspector。
/// </summary>
class SpriteInspector {
public:
	/// <summary>
	/// Sprite全体のInspectorを描画する。
	/// </summary>
	static void Draw(Sprite& sprite);

private:
	static void DrawTransform(ScreenTransform& transform);
	static void DrawUv(Sprite& sprite);
	static void DrawTexture(Sprite& sprite);
	static void DrawFill(Sprite& sprite);
	static void DrawSaveLoad(Sprite& sprite);
};

}
