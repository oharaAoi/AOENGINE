#pragma once

/// engine
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE{
class WorldTransform;
}

/// <summary>
/// 演出用オブジェクト1個分のインターフェース。
/// EffectObjectGroup に登録して、まとめてライフサイクルを管理してもらう。
/// パーティクル以外(モデル等)を足す場合もこのインターフェースを実装する
/// </summary>
class IEffectObject{
public:
	virtual ~IEffectObject() = default;

	/// <summary>追従させる座標系を設定する。nullptr ならワールド座標として扱う</summary>
	virtual void SetParent(AOENGINE::WorldTransform* parent) = 0;
	/// <summary>親から見たローカル座標を設定する</summary>
	virtual void SetLocalPosition(const Math::Vector3& position) = 0;
	/// <summary>演出を再生する</summary>
	virtual void Play() = 0;
	/// <summary>演出を止める。オブジェクト自体は残るので、また Play() できる</summary>
	virtual void Stop() = 0;
	/// <summary>更新。毎フレーム自前で動かす必要がある演出のためのフック</summary>
	virtual void Update(float deltaTime){ static_cast<void>(deltaTime); }
	/// <summary>演出の実体がまだ生きているか。false ならこのオブジェクトは作り直しが必要</summary>
	virtual bool IsAlive() const = 0;
};
