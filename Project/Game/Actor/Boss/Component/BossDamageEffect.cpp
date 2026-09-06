#include "BossDamageEffect.h"

#include <algorithm>

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"

using namespace AOENGINE;

namespace {
	// 色を見に行くマテリアルの番号
	const uint32_t kBaseMaterialSlot = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void BossDamageEffect::Init(BaseGameObject* body) {

	shake_.Stop(true);
	positionOffset_ = CVector3::ZERO;
	timer_ = 0.0f;
	isPlaying_ = false;

	// 演出が終わった時に戻す色を覚えておく
	if (body != nullptr) {
		if (BaseMaterial* material = body->GetMaterial(kBaseMaterialSlot)) {
			baseColor_ = material->GetColor();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  再生
///////////////////////////////////////////////////////////////////////////////////////////////

void BossDamageEffect::Play(const Params& params) {

	// 連続で当たった時は、途中でも頭から流し直す
	timer_ = 0.0f;
	isPlaying_ = params.duration > 0.0f;
	shake_.Play(params.shake);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossDamageEffect::Update(float deltaTime, BaseGameObject* body, const Params& params) {

	// 揺れはリクエスト側が終わりを持っているので、毎フレーム進めるだけでよい
	shake_.Update(deltaTime);
	positionOffset_ = shake_.GetResult().positionOffset;

	if (!isPlaying_) {
		return;
	}

	timer_ += deltaTime;

	// 被弾した瞬間が一番濃く、時間が経つほど元の色へ戻る
	const float ratio = std::clamp(timer_ / params.duration, 0.0f, 1.0f);
	ApplyColor(body, params, ratio);

	if (ratio >= 1.0f) {
		isPlaying_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  色を入れる
///////////////////////////////////////////////////////////////////////////////////////////////

void BossDamageEffect::ApplyColor(BaseGameObject* body, const Params& params, float ratio) const {

	if (body == nullptr) {
		return;
	}

	body->SetColor(Color::Lerp(params.color, baseColor_, ratio));
}
