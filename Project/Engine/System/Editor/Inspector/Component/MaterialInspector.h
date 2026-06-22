#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

/// <summary>
/// BaseGameObjectが所有するMaterial一覧を編集するInspector。
/// </summary>
class MaterialInspector {
public:
	/// <summary>
	/// Material一覧を折りたたみ項目として描画する。
	/// </summary>
	/// <param name="object">Materialを所有するBaseGameObject。</param>
	static void Draw(BaseGameObject& object);
};

}
