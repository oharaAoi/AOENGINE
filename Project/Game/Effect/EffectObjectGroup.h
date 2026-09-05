#pragma once

/// stl
#include <memory>
#include <string>
#include <vector>

/// engine
#include "Engine/Lib/Math/Vector3.h"

/// game
#include "Game/Effect/IEffectObject.h"
#include "Game/Effect/ParticleEffectObject.h"
#include "Game/Effect/PrefabEffectObject.h"

namespace AOENGINE{
class WorldTransform;
}

/// <summary>
/// 演出用オブジェクト(パーティクルやモデルなど)をまとめて持ち、ライフサイクルを管理するクラス。
/// このグループが破棄されれば、持っている演出オブジェクトも全て破棄される
/// </summary>
class EffectObjectGroup{
public:

	EffectObjectGroup() = default;
	// objects_ が unique_ptr<IEffectObject> を持ち、デストラクタで完全型が必要になるため .cpp 側で定義する
	~EffectObjectGroup();

	// 演出オブジェクトの二重所有を防ぐためコピーは禁止する。
	// BlockGroupLauncher が vector で使い回されて再確保される時にムーブされるため、ムーブは残す。
	// ムーブ後の元オブジェクトは objects_ が空になるので、その後に破棄されても
	// 中身の演出オブジェクト(移動先が今も使っている)は消えない
	EffectObjectGroup(const EffectObjectGroup&) = delete;
	EffectObjectGroup& operator=(const EffectObjectGroup&) = delete;
	EffectObjectGroup(EffectObjectGroup&&) noexcept = default;
	EffectObjectGroup& operator=(EffectObjectGroup&&) noexcept = default;

	/// <summary>演出オブジェクトの所有権を受け取って登録する。nullptr は無視する</summary>
	/// <returns>登録した演出オブジェクトの非所有ポインタ(nullptr を渡した場合は nullptr)</returns>
	IEffectObject* Add(std::unique_ptr<IEffectObject> object);

	/// <summary>パーティクルの演出オブジェクトを作って追加する</summary>
	/// <param name="particleName">ParticleManager に渡すパーティクル名</param>
	/// <param name="localPosition">親から見たローカル座標</param>
	/// <param name="delay">Play() してから射出を始めるまでの待ち時間</param>
	ParticleEffectObject* AddParticle(const std::string& particleName,const Math::Vector3& localPosition = Math::Vector3(0.0f,0.0f,0.0f),float delay = 0.0f);

	/// <summary>prefab から生成する演出オブジェクトを作って追加する</summary>
	/// <param name="prefabName">PrefabManager に渡す prefab 名</param>
	/// <param name="localPosition">親から見たローカル座標</param>
	PrefabEffectObject* AddPrefab(const std::string& prefabName,const Math::Vector3& localPosition = Math::Vector3(0.0f,0.0f,0.0f));

	/// <summary>
	/// 複合エフェクト(ParticleEffectManager が扱う "Composite" のアセット)を
	/// Particle 1個ずつに分解し、それぞれを ParticleEffectObject として追加する。
	/// ParticleEffectManager 経由で再生すると寿命の管理が向こう任せになるため、
	/// このグループ(ひいては親のオブジェクト)と一緒に消えてほしい場合はこちらを使う。
	/// 複合エフェクトの duration(自動停止)は再現しない。停止はこのグループの Stop() で行うこと
	/// </summary>
	/// <param name="effectName">複合エフェクトの名前</param>
	/// <param name="localPosition">親から見たローカル座標。各ノードの位置はここからの相対位置になる</param>
	/// <returns>追加できた Particle の数。エフェクトが見つからない場合は0</returns>
	size_t AddParticleEffect(const std::string& effectName,const Math::Vector3& localPosition = Math::Vector3(0.0f,0.0f,0.0f));

	/// <summary>全ての演出オブジェクトに追従先の座標系を伝える</summary>
	void SetParent(AOENGINE::WorldTransform* parent);
	/// <summary>全ての演出オブジェクトを再生する</summary>
	void Play();
	/// <summary>全ての演出オブジェクトを止める</summary>
	void Stop();
	/// <summary>全ての演出オブジェクトを更新する</summary>
	void Update(float deltaTime);

	/// <summary>持っている演出オブジェクトを全て破棄する</summary>
	void Destroy();

	bool IsEmpty() const{ return objects_.empty(); }
	size_t GetCount() const{ return objects_.size(); }

private:

	/// <summary>演出オブジェクト1個分の管理データ。実体が消えた時に作り直せるよう、作り方も覚えておく</summary>
	struct Entry{
		/// <summary>作り方。External は外から Add() された物で、作り直せない</summary>
		enum class Kind{
			External,
			Particle,
			Prefab,
		};

		std::unique_ptr<IEffectObject> object;
		Kind kind = Kind::External;
		std::string name;					// Particle / Prefab の生成に使う名前
		Math::Vector3 localPosition{};		// 親から見たローカル座標
		float delay = 0.0f;				// Particle が Play() から射出を始めるまでの待ち時間
	};

	/// <summary>
	/// 実体が消えてしまった演出オブジェクトを作り直す。
	/// PrefabEffectObject が指す GameObject は SceneWorld のクリア(シーン切り替えなど)や
	/// エディタからの削除で消えることがあり、そのままだと以後ずっと何も出なくなるため、
	/// 再生のたびに生存を確認して作り直す
	/// </summary>
	void RebuildDeadObjects();

	std::vector<Entry> objects_;
	AOENGINE::WorldTransform* pParent_ = nullptr;	// 非所有。作り直した演出オブジェクトに設定し直すために覚えておく

};
