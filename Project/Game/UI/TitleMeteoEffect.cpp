#include "TitleMeteoEffect.h"

#include <algorithm>
#include <cmath>

#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/PrefabManager.h"

using namespace AOENGINE;

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleMeteoEffect::Init() {

	parameter_.Load();

	// 本体の寿命はSceneWorldが持っていて、シーンの切り替えでまとめて片付く。
	if (meteos_.empty()) {
		const int32_t count = (std::max)(parameter_.count, 1);
		meteos_.resize(static_cast<std::size_t>(count));

		for (Meteo& meteo : meteos_) {
			SpawnMeteo(meteo);
		}
	}

	// 開始直後から空に散らばっているようにする
	for (Meteo& meteo : meteos_) {
		Respawn(meteo, true);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 火玉の用意
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleMeteoEffect::SpawnMeteo(Meteo& meteo) {

	// ボスの火玉と同じプレハブ。玉の見た目も尾を引くParticleも中に入っている
	SceneObject* root = PrefabManager::GetInstance()->Instantiate(kPrefabName);
	BaseGameObject* object = dynamic_cast<BaseGameObject*>(root);
	if (object == nullptr) {
		return;
	}

	meteo.entity.Bind(object);

	// タイトルでは当てる相手がいないので、判定は切っておく
	if (BaseCollider* collider = meteo.entity.GetCollider(kColliderTag)) {
		collider->SetIsActive(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleMeteoEffect::Update(float deltaTime) {

	const Math::Vector3 direction = CalcDirection();

	for (Meteo& meteo : meteos_) {
		if (!meteo.entity.IsValid()) {
			continue;
		}

		const float step = meteo.speed * deltaTime;
		meteo.position += direction * step;
		meteo.traveled += step;

		// 落ちきったら、また帯の方へ戻す
		if (meteo.traveled >= parameter_.travelDistance) {
			Respawn(meteo, false);
			continue;
		}

		ApplyTransform(meteo);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 落ちていく向き
//////////////////////////////////////////////////////////////////////////////////////////////////

Math::Vector3 TitleMeteoEffect::CalcDirection() const {

	const float radian = parameter_.fallAngle * kToRadian;
	return Math::Vector3(std::cos(radian), std::sin(radian), 0.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 落とし直し
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleMeteoEffect::Respawn(Meteo& meteo, bool isSpread) {

	const Math::Vector3& center = parameter_.spawnCenter;
	const Math::Vector3& size = parameter_.spawnSize;

	meteo.position = Math::Vector3(
		center.x + Random::RandomFloat(-size.x, size.x),
		center.y + Random::RandomFloat(-size.y, size.y),
		center.z + Random::RandomFloat(-size.z, size.z));

	// min/maxが逆に設定されても壊れないように、小さい方を先にしておく
	float minSpeed = parameter_.minSpeed;
	float maxSpeed = parameter_.maxSpeed;
	if (minSpeed > maxSpeed) {
		const float swapped = minSpeed;
		minSpeed = maxSpeed;
		maxSpeed = swapped;
	}
	meteo.speed = Random::RandomFloat(minSpeed, maxSpeed);

	meteo.traveled = 0.0f;

	// 途中から始める場合は、その分だけ進んだところへ置く
	if (isSpread) {
		meteo.traveled = Random::RandomFloat(0.0f, parameter_.travelDistance);
		meteo.position += CalcDirection() * meteo.traveled;
	}

	ApplyTransform(meteo);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 見た目への反映
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleMeteoEffect::ApplyTransform(const Meteo& meteo) const {

	WorldTransform* transform = meteo.entity.GetTransform();
	if (transform == nullptr) {
		return;
	}

	transform->SetTranslate(meteo.position);
	transform->SetScale(Math::Vector3(parameter_.scale, parameter_.scale, parameter_.scale));

	// 同じフレームに子のParticleが先に更新されても、置き直した後の位置を
	// 親行列として参照できるよう、ここで行列を確定する
	transform->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ編集
//////////////////////////////////////////////////////////////////////////////////////////////////

void TitleMeteoEffect::Debug_Gui() {

	parameter_.Debug_Gui();

	ImGui::Text("meteos: %d", static_cast<int>(meteos_.size()));
	ImGui::TextUnformatted("Countはシーンを入り直すと反映される");

	if (ImGui::Button("Respawn All")) {
		for (Meteo& meteo : meteos_) {
			Respawn(meteo, true);
		}
	}

	parameter_.SaveAndLoad();
}
