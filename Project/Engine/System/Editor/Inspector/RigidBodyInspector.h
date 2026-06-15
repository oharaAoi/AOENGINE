#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

/// <summary>
/// BaseGameObjectが所有するRigidbodyを編集するInspector。
/// </summary>
class RigidBodyInspector {
public:
	/// <summary>
	/// Rigidbodyが存在する場合に編集UIを描画する。
	/// </summary>
	/// <param name="object">Rigidbodyを所有するBaseGameObject。</param>
	static void Draw(BaseGameObject& object);
};

}
