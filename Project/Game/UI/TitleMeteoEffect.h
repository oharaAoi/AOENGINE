#pragma once

#include <string>
#include <vector>

#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/UI/TitleMeteoParameter.h"

/// <summary>
/// タイトルの背景へ隕石を落とす
/// </summary>
class TitleMeteoEffect {
public:

	TitleMeteoEffect() = default;
	~TitleMeteoEffect() = default;

	// 初期化、更新
	void Init();
	void Update(float deltaTime);

	// パラメータ編集
	void Debug_Gui();

private:

	// 隕石1つぶん。中身は火玉と同じ
	struct Meteo {
		AOENGINE::BaseEntity entity;	// 火玉本体
		Math::Vector3 position{};		// 今いるところ
		float speed = 0.0f;				// 落ちる速さ
		float traveled = 0.0f;			// 湧いてから進んだ距離
	};

	/// <summary>火玉のプレハブを1つ用意する</summary>
	void SpawnMeteo(Meteo& meteo);

	/// <summary>調整値の角度から、落ちていく向きを作る</summary>
	Math::Vector3 CalcDirection() const;

	/// <summary>
	/// 湧く帯へ戻して落とし直す
	/// </summary>
	void Respawn(Meteo& meteo, bool isSpread);

	/// <summary>今の位置と大きさを見た目へ入れる</summary>
	void ApplyTransform(const Meteo& meteo) const;

private:

	// 調整値
	TitleMeteoParameter parameter_;

	// 落としている隕石
	std::vector<Meteo> meteos_;

	// 火玉のプレハブ名とCollider名。ボスの火玉と同じものを使う
	const std::string kPrefabName = "Fireball";
	const std::string kColliderTag = "Fireball";
};
