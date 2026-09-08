#pragma once

/// engine
#include "Engine/Lib/Math/Vector3.h"

/// game
#include "Game/UI/PopupTextUI.h"
#include "Game/UI/ComboTextUIParameter.h"

/// <summary>
/// 足場を何個消したかを表示するUI。
/// 出る・跳ねる・消えるの動きは PopupTextUI が持っているため、
/// このクラスは「どのPrefabで何と出すか」だけを決める
/// </summary>
class ComboTextUI 
	: public PopupTextUI{
public:
	ComboTextUI() = default;
	~ComboTextUI() override = default;

	/// <summary>
	/// Prefabから実体を作って表示を始める
	/// </summary>
	/// <param name="comboCount">何個目の接続か</param>
	/// <param name="position">表示する位置(接続したグループの中心)</param>
	/// <param name="parameter">調整値。管理側が持っているものを参照し続ける</param>
	/// <returns>生成できた場合はtrue</returns>
	bool Spawn(int comboCount, const Math::Vector3& position, const ComboTextUIParameter& parameter);
};
