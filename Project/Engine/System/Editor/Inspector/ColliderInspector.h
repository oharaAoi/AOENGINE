#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

/// <summary>
/// BaseGameObjectが所有するCollider一覧を編集するInspector。
/// </summary>
class ColliderInspector {
public:
	/// <summary>
	/// Collider一覧を折りたたみ項目として描画する。
	/// </summary>
	/// <param name="object">Colliderを所有するBaseGameObject。</param>
	static void Draw(BaseGameObject& object);
};

}
