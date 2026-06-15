#pragma once
#include "Engine/Module/Components/GameObject/SceneObject.h"

namespace AOENGINE {

/// <summary>
/// UI関連のObjectの継承元となるクラス
/// </summary>
class UIObject :
	public SceneObject {
public:

	UIObject() = default;
	~UIObject() = default;

};
}