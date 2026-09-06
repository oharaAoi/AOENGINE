#pragma once

#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Entity/Camera/Component/CameraShake.h"

namespace AOENGINE {
	class BaseGameObject;
}

/// <summary>
/// ボスが被弾した時の見た目の反応
/// </summary>
class BossDamageEffect {
public:

	// 調整値
	struct Params {
		float duration = 0.0f;			// 演出の長さ
		AOENGINE::Color color;			// 被弾した瞬間に寄せる色
		CameraShakeRequest shake;		// 揺れ方。カメラと同じ仕組みを使う
	};

	BossDamageEffect() = default;
	~BossDamageEffect() = default;

	/// <summary>元の色を覚えておく。モデルを差し替えた後に呼ぶ</summary>
	void Init(AOENGINE::BaseGameObject* body);

	/// <summary>被弾した瞬間に呼ぶ。演出を頭から流し直す</summary>
	void Play(const Params& params);

	/// <summary>演出を進める。色はここで直接入れる</summary>
	void Update(float deltaTime, AOENGINE::BaseGameObject* body, const Params& params);

private:

	/// <summary>今の進み具合から色を決めて入れる</summary>
	void ApplyColor(AOENGINE::BaseGameObject* body, const Params& params, float ratio) const;

private:

	// 揺れの計算。カメラシェイクをそのまま流用する
	CameraShake shake_;

	// 演出を始める前の色。終わったらここへ戻す
	AOENGINE::Color baseColor_{ 1.0f, 1.0f, 1.0f, 1.0f };

	// 揺れによる位置のずらし量
	Math::Vector3 positionOffset_{};

	// 演出の経過時間
	float timer_ = 0.0f;
	bool isPlaying_ = false;

public: // accessor

	/// <summary>基準位置へ足す、揺れのずらし量</summary>
	const Math::Vector3& GetPositionOffset() const { return positionOffset_; }

	bool IsPlaying() const { return isPlaying_; }
};
