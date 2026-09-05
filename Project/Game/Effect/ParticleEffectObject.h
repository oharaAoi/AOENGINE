#pragma once

/// stl
#include <string>

/// game
#include "Game/Effect/IEffectObject.h"

namespace AOENGINE{
class BaseParticles;
}

/// <summary>
/// CPU パーティクルによる演出オブジェクト。
/// 実体は ParticleManager が emitterList_ で所有しており、このクラスは非所有ポインタで触るだけ。
/// 破棄時に ParticleManager::DeleteParticles() を呼ぶことで、
/// 「このオブジェクトが消えたら emitter も消える」を保証する
/// </summary>
class ParticleEffectObject : public IEffectObject{
public:

	/// <summary>指定した名前のパーティクルを ParticleManager に作らせて保持する</summary>
	explicit ParticleEffectObject(const std::string& particleName);
	~ParticleEffectObject() override;

	// パーティクルの二重破棄を防ぐため、コピー・ムーブは禁止する。
	// 所有は必ず std::unique_ptr<IEffectObject> 越しに行うこと
	ParticleEffectObject(const ParticleEffectObject&) = delete;
	ParticleEffectObject& operator=(const ParticleEffectObject&) = delete;
	ParticleEffectObject(ParticleEffectObject&&) = delete;
	ParticleEffectObject& operator=(ParticleEffectObject&&) = delete;

	void SetParent(AOENGINE::WorldTransform* parent) override;
	void SetLocalPosition(const Math::Vector3& position) override;
	void Play() override;
	void Stop() override;
	void Update(float deltaTime) override;
	bool IsAlive() const override;

public: // accessor

	/// <summary>パーティクル固有の設定を外から触れるようにするための生ポインタ</summary>
	AOENGINE::BaseParticles* GetParticle() const{ return pParticle_; }

	/// <summary>Play() してから実際に射出を始めるまでの待ち時間を設定する</summary>
	void SetDelay(float delay){ delay_ = delay; }
	float GetDelay() const{ return delay_; }

private:

	AOENGINE::BaseParticles* pParticle_ = nullptr;	// 非所有。実体は ParticleManager が持つ

	// 複合エフェクトを分解した際、ノードごとの delay(再生開始の遅れ)を
	// このオブジェクト単体でも再現できるようにするための待ち時間管理
	float delay_ = 0.0f;		// 再生開始までの待ち時間
	float waitTimer_ = 0.0f;	// 残りの待ち時間
	bool isWaiting_ = false;	// 待っている最中か

};
