#pragma once

/// stl
#include <string>

/// engine
#include "Engine/Lib/Math/Vector3.h"

/// game
#include "Game/UI/PopupTextUI.h"
#include "Game/UI/DamageTextUIParameter.h"

/// <summary>
/// 集合地点に出す表示。
/// 集合中はそこまでに集まったブロック数とコンボを出し、
/// 全部集まったら総ダメージへ切り替える。
/// 出る・跳ねる・消えるの動きは PopupTextUI が持っているため、
/// このクラスは「どのPrefabで何と出すか」だけを決める
/// </summary>
class DamageTextUI 
	: public PopupTextUI{
public:
	DamageTextUI() = default;
	~DamageTextUI() override = default;

	/// <summary>
	/// Prefabから実体を作って表示を始める
	/// </summary>
	/// <param name="text">表示する文字</param>
	/// <param name="position">表示する位置(集合地点)</param>
	/// <param name="parameter">調整値。管理側が持っているものを参照し続ける</param>
	/// <returns>生成できた場合はtrue</returns>
	bool Spawn(const std::string& text, const Math::Vector3& position, const DamageTextUIParameter& parameter);

	/// <summary>集めた数の文字を作る</summary>
	static std::string MakeCountText(int blockCount, int comboCount, const DamageTextUIParameter& parameter);
	/// <summary>総ダメージの文字を作る</summary>
	static std::string MakeDamageText(float damage, const DamageTextUIParameter& parameter);
};
