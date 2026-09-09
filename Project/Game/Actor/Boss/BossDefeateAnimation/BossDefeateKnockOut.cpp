#include "BossDefeateKnockOut.h"

#include "Engine/Module/Components/WorldTransform.h"

#include "Game/Actor/Boss/BossDefeateAnimation/BossDefeateAnimation.h"
#include "Game/Actor/Boss/Boss.h"

void BossDefeateKnockOut::Init() {
	knockOutParameter_.Load();
}

void BossDefeateKnockOut::Update(Boss& boss) {
	AOENGINE::WorldTransform* transform = boss.GetTransform();

	// 座標の更新
	Math::Vector3 pos = transform->GetWorldPos();
	pos += knockOutParameter_.knockOutDirection * knockOutParameter_.knockOutSpeed;
	transform->SetTranslate(pos);

	// 回転の更新
	Math::Quaternion rotate = transform->GetRotate();
	Math::Quaternion knockOutRotate = Math::Quaternion::AngleAxis(knockOutParameter_.knockOutRotateSpeed * kToRadian, CVector3::RIGHT);
	transform->SetRotate(knockOutRotate * rotate);

}
