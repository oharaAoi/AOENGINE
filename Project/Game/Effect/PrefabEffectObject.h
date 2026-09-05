#pragma once

/// stl
#include <string>

/// engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"

/// game
#include "Game/Effect/IEffectObject.h"

/// <summary>
/// prefab から生成する GameObject 版の演出オブジェクト。
/// 実体は SceneWorld 上の GameObject であり、BaseEntity 経由で ObjectHandle として安全に扱う。
/// SceneRenderer が SceneWorld 上のオブジェクトを毎フレーム更新するため、
/// このクラス自体は Update() をオーバーライドしない(自前更新が不要)
/// </summary>
class PrefabEffectObject : public IEffectObject,public AOENGINE::BaseEntity{
public:

	/// <summary>指定した prefab から GameObject を生成して保持する</summary>
	explicit PrefabEffectObject(const std::string& prefabName);
	// 親(EffectObjectGroup、ひいては Launcher)が消えたら演出も消える実体なので、
	// デストラクタで SceneWorld から実体を削除する
	~PrefabEffectObject() override;

	// 実体の二重破棄を防ぐため、コピー・ムーブは禁止する。
	// 所有は必ず std::unique_ptr<IEffectObject> 越しに行うこと
	PrefabEffectObject(const PrefabEffectObject&) = delete;
	PrefabEffectObject& operator=(const PrefabEffectObject&) = delete;
	PrefabEffectObject(PrefabEffectObject&&) = delete;
	PrefabEffectObject& operator=(PrefabEffectObject&&) = delete;

	// IEffectObject::SetParent(WorldTransform*) と BaseEntity::SetParent(const BaseEntity*) は
	// 名前が衝突しており、下の override 宣言によって BaseEntity 側が名前隠蔽で
	// 呼べなくなってしまうため、using 宣言で両方のオーバーロードを使えるようにする
	using AOENGINE::BaseEntity::SetParent;

	void SetParent(AOENGINE::WorldTransform* parent) override;
	void SetLocalPosition(const Math::Vector3& position) override;
	void Play() override;
	void Stop() override;
	bool IsAlive() const override;

};
