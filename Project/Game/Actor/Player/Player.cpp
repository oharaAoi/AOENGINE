#include "Player.h"

#include <algorithm>
#include <cmath>

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Render/Render.h"

#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"

using namespace AOENGINE;

namespace
{
	// 接続されたブロックグループに付ける色
	constexpr AOENGINE::Color kConnectedGroupColor{ 1.0f, 0.55f, 0.15f, 1.0f };
	// 接続したブロックグループ同士を結ぶ線の色
	constexpr AOENGINE::Color kConnectLineColor{ 1.0f, 0.9f, 0.2f, 1.0f };
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init(BaseGameObject* body)
{

	Bind(body);

	// 保存済みの調整値を読み込む
	parameter_.Load();
	facing_ = 1.0f;
	scaleMultiplier_ = CVector3::UNIT;
	animation_.Init();

	// HPを満タンにする
	currentHp_ = parameter_.maxHp;
	invincibleTimer_ = 0.0f;
	invincibleBlinkTimer_ = 0.0f;
	isBlinkVisible_ = true;
	SetRendering(true);

	if (BaseGameObject* object = GetGameObject())
	{
		// SceneやPrefabにRigidbodyが無い場合はここで用意する
		if(object->GetRigidbody() == nullptr){
			object->SetPhysics();
		}

		// rigidbody初期状態
		if (Rigidbody* rigidbody = object->GetRigidbody())
		{

			rigidbody->SetGravity(false);
			rigidbody->SetVelocity(CVector3::ZERO);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update(){
	if(!IsValid()){
		return;
	}

	Rigidbody* rigidbody = GetRigidbody();
	if (!rigidbody)
	{
		return;
	}

	const float deltaTime = GameTimer::DeltaTime();

	// 無敵時間を進める
	UpdateInvincible(deltaTime);

	// 前フレームの結果に対して接地判定を行う
	ResolveGround(deltaTime);

	// 入力をサンプリング
	input_.Update(parameter_.stickDeadZone);

	// 移動更新
	UpdateMove(rigidbody, deltaTime);

	// ジャンプパラメータセット
	const PlayerJump::Params jumpParams{
		parameter_.jumpPower,
		parameter_.hangTime,
		parameter_.riseGravity,
		parameter_.fallGravity,
		parameter_.maxFallSpeed,
		parameter_.groundKeepSpeed,
	};

	// ジャンプ処理
	jump_.Update(deltaTime,input_.IsJumpTriggered(),input_.IsJumpHeld(),jumpParams);
	rigidbody->SetVelocityY(jump_.GetVelocityY());

	// 胴体が埋まっているグループは、抜けきったものから判定を戻していく
	blockIgnore_.Update(MakeBlockIgnoreContext());

	// ブロックグループの接続受付・集合・打ち上げ
	UpdateBlockGroupConnect(deltaTime);

	// 接地中は足場の上面へ吸着させ、落下の下限と奥行きを揃える
	const PlayerGroundState::Context groundContext = MakeGroundContext();
	const PlayerGroundState::Params groundParams = MakeGroundParams();
	groundState_.SnapToGround(groundContext, groundParams);
	groundState_.ClampFallLimit(groundContext, groundParams);
	groundState_.FixZPosition(groundContext, groundParams);

	// 見た目まわり
	UpdateScale();
	UpdateFacingRotate(deltaTime);
	UpdateAnimation(deltaTime);
}


///////////////////////////////////////////////////////////////////////////////////////////////
//  スケール
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateScale() {

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	// 基準の大きさに演出用の倍率を掛けたものが、実際の見た目の大きさになる
	const Math::Vector3 scale = parameter_.baseScale * scaleMultiplier_;
	transform->SetScale(scale);

	BoxCollider* box = dynamic_cast<BoxCollider*>(GetCollider(kColliderTag));
	if (box == nullptr) {
		return;
	}

	Math::Vector3 size = parameter_.hitSize;

	// AABBのColliderは毎フレーム「回転させた8頂点」から作り直される
	// そのため左右を向く途中で底面が最大√2倍まで膨らみ
	size.z = size.x;
	const Math::Vector3 forward = transform->GetRotate().Rotate(CVector3::FORWARD);
	const float footprintFactor = std::abs(forward.x) + std::abs(forward.z);
	if (footprintFactor > 0.0f) {
		size.x /= footprintFactor;
		size.z /= footprintFactor;
	}

	// Colliderのsize・localPositionにはtransformのscaleが掛かるので、
	// 見た目を大きくしても当たり判定が一緒に膨らまないように割り戻しておく
	Math::Vector3 offset = parameter_.hitOffset;
	if (scale.x != 0.0f) { size.x /= scale.x; offset.x /= scale.x; }
	if (scale.y != 0.0f) { size.y /= scale.y; offset.y /= scale.y; }
	if (scale.z != 0.0f) { size.z /= scale.z; offset.z /= scale.z; }

	box->SetSize(size);
	box->SetLocalPos(offset);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  向き
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateFacingRotate(float deltaTime) {

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	// 向いている方向の角度を決める。モデルの正面がどちらを向いているかは
	// パラメータ側で合わせられるようにしてある
	float targetYaw = parameter_.facingYawLeft;
	if (facing_ > 0.0f) {
		targetYaw = parameter_.facingYawRight;
	}

	const Math::Quaternion target = Math::Quaternion::AngleAxis(targetYaw * kToRadian, CVector3::UP);

	// 指数的に近づける。こうしておくとフレームレートが変わっても振り向きの速さが変わらない
	const float t = 1.0f - std::exp(-parameter_.facingTurnSpeed * deltaTime);
	transform->SetRotate(Math::Quaternion::Slerp(transform->GetRotate(), target, t).Normalize());
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  アニメーション
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateAnimation(float deltaTime) {

	PlayerAnimation::Context context{};
	if (BaseGameObject* body = GetGameObject()) {
		context.animator = body->GetAnimator();
	}
	context.horizontal = input_.GetHorizontal();
	context.velocityX = GetVelocity().x;
	context.isGrounded = jump_.IsGrounded();

	const PlayerAnimation::Params params{
		parameter_.animationBlendSpeed,
		parameter_.animationSpeed,
		parameter_.walkAnimationSpeed,
		parameter_.idleAnimationDelay,
		parameter_.moveInputThreshold,
	};

	animation_.Update(deltaTime, context, params);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ブロックグループの接続受付・集合・打ち上げ
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateBlockGroupConnect(float deltaTime){
	const BlockGroupConnectState::Params connectParams{
		parameter_.connectableTime,
		parameter_.launchWaitTime,
	};

	BlockGroupConnectState::Context context{};
	if (const WorldTransform* transform = GetTransform())
	{
		context.playerPosition = transform->GetTranslate();
	}
	context.isGrounded = jump_.IsGrounded();
	context.launchTriggered = input_.IsLaunchTriggered();

	// 先に前フレームまでの接続受付時間を消化してから、今フレームのジャンプで新しい受付を開始する
	blockGroupConnectState_.Update(deltaTime,context,connectParams);
	if(jump_.IsJumpStarted()){
		blockGroupConnectState_.Begin();
	}

	const BlockGroupLauncher::Params launcherParams{
		parameter_.gatherSpeed,
		parameter_.launchSpeed,
		parameter_.launchAccel,
		parameter_.launchLifeTime,
		parameter_.gatherBlockSize,
		parameter_.gatherSeparationSpeed,
	};

	// 受付が終わったら、接続したグループを次のブロックへ順に渡らせて集合地点へ集める
	if(blockGroupConnectState_.IsGatherStarted()){
		BlockGroupLauncher::GatherRequest request{};
		request.gatherPoint = blockGroupConnectState_.GetGatherPoint();

		const std::vector<BlockGroupConnectState::ConnectedGroup>& connectedGroups =
			blockGroupConnectState_.GetConnectedGroups();
		request.targets.reserve(connectedGroups.size());
		for (const BlockGroupConnectState::ConnectedGroup& group : connectedGroups)
		{
			request.targets.push_back(BlockGroupLauncher::Target{ group.groupId, group.connectPosition });
		}

		blockGroupLauncherManager_.BeginGather(request,launcherParams);
	}

	// 専用タイマーが尽きるか打ち上げ入力が来たら上へ打ち上げる
	if(blockGroupConnectState_.IsLaunchRequested()){
		blockGroupLauncherManager_.Launch();
	}

	blockGroupLauncherManager_.Update(deltaTime);

	DrawBlockGroupConnectLine();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  接続したブロックグループ同士を線で結ぶ
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::DrawBlockGroupConnectLine() const{
	// 飛んでいるセットの線と、接続受付中の線を両方描く。
	// 集合・打ち上げ中のグループは BeginGather() 時点で StageBlockField の表から外れており、
	// 接続受付中のグループはまだ表に残っているため、両者が同じグループを二重に結ぶことはない
	if(blockGroupLauncherManager_.IsActive()){
		blockGroupLauncherManager_.DrawConnectLine(kConnectLineColor,6.f);
	}

	// 接続受付中は StageBlockField からグループの中心を引いて結ぶ
	if(!pBlockField_){
		return;
	}

	const std::vector<BlockGroupConnectState::ConnectedGroup>& connectedGroups =
		blockGroupConnectState_.GetConnectedGroups();

	std::vector<Math::Vector3> points;
	points.reserve(connectedGroups.size());
	for(const BlockGroupConnectState::ConnectedGroup& group : connectedGroups){
		Math::Vector3 center{};
		if(!pBlockField_->TryGetGroupCenter(group.groupId,center)){
			continue;
		}
		center.z += 0.5f;
		points.push_back(center);
	}

	if(points.size() < 2){
		return;
	}

	for(size_t index = 1; index < points.size(); ++index){
 		AOENGINE::Render::DrawThickLine(points[index - 1],points[index],kConnectLineColor,6.f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ブロックグループの接続
///////////////////////////////////////////////////////////////////////////////////////////////

bool Player::TryConnectBlockGroup(int groupId){
	if(!blockGroupConnectState_.TryAdd(groupId)){
		return false;
	}

	// 接続できたグループは色を変えて、どれを繋いだかが見て分かるようにする
	if(pBlockField_){
		pBlockField_->SetGroupColor(groupId,kConnectedGroupColor);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ダメージ床のノックバック
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ApplyDamageFloorKnockback(float power)
{
	damageFloorAirborne_ = true;
	// 飛んでいる間はBlockだけ無視する。Wallやダメージ床には当たったまま
	blockIgnore_.Begin(MakeBlockIgnoreContext());
	jump_.Knockback(power);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  被弾
///////////////////////////////////////////////////////////////////////////////////////////////

bool Player::TakeDamage(float amount)
{
	// 無敵中や、既に倒れている時は受け付けない
	if (IsInvincible() || IsDead())
	{
		return false;
	}

	currentHp_ -= amount;
	if (currentHp_ < 0.0f)
	{
		currentHp_ = 0.0f;
	}

	// 連続ヒットを防ぐために無敵時間を入れる
	invincibleTimer_ = parameter_.invincibleTime;
	invincibleBlinkTimer_ = 0.0f;
	isBlinkVisible_ = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  無敵時間中の点滅
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateInvincible(float deltaTime)
{
	if (invincibleTimer_ <= 0.0f)
	{
		return;
	}

	invincibleTimer_ -= deltaTime;

	// 無敵が切れたら必ず表示状態へ戻す
	if (invincibleTimer_ <= 0.0f)
	{
		invincibleBlinkTimer_ = 0.0f;
		isBlinkVisible_ = true;
		SetRendering(true);
		return;
	}

	// 一定間隔で表示と非表示を切り替えてちかちかさせる
	invincibleBlinkTimer_ += deltaTime;
	if (invincibleBlinkTimer_ < parameter_.invincibleBlinkInterval)
	{
		return;
	}
	invincibleBlinkTimer_ = 0.0f;

	isBlinkVisible_ = !isBlinkVisible_;
	SetRendering(isBlinkVisible_);
}


void Player::SetBlockField(StageBlockField* field)
{
	pBlockField_ = field;
	blockGroupLauncherManager_.SetField(field);
}

void Player::ResetStageReferences()
{
	// ステージのブロックが破棄されるため、実体やグループIDを指しているものを全て手放す
	blockGroupLauncherManager_.Clear();
	blockGroupConnectState_.Clear();
	blockIgnore_.ClearGroups();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  Rigidbodyの取得
///////////////////////////////////////////////////////////////////////////////////////////////

Rigidbody* Player::GetRigidbody() const
{
	BaseGameObject* object = GetGameObject();
	if (!object)
	{
		return nullptr;
	}
	return object->GetRigidbody();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  左右移動
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::UpdateMove(Rigidbody* rigidbody, float deltaTime)
{
	const float horizontal = input_.GetHorizontal();

	float speedX = 0.0f;
	if (std::abs(horizontal) > parameter_.moveInputThreshold)
	{
		const float targetSpeedX = horizontal * parameter_.moveSpeed;
		float currentSpeedX = rigidbody->GetVelocity().x;

		// 左右切り替えの時はスピードを0からリセット
		const bool isReversing = (targetSpeedX > 0.0f && currentSpeedX < 0.0f) ||
			(targetSpeedX < 0.0f && currentSpeedX > 0.0f);
		if (isReversing)
		{
			currentSpeedX = 0.0f;
		}

		// 入力がある間だけmoveAccelerationで加速し、moveSpeedで頭打ちにする
		const float maxDeltaSpeed = parameter_.moveAcceleration * deltaTime;
		speedX = currentSpeedX + std::clamp(targetSpeedX - currentSpeedX, -maxDeltaSpeed, maxDeltaSpeed);
	}

	// 移動反映
	rigidbody->SetVelocityX(speedX);
	rigidbody->SetVelocityZ(0.0f);

	// 向き切り替え
	if (horizontal > parameter_.moveInputThreshold)
	{
		facing_ = 1.0f;
	} else if(horizontal < -parameter_.moveInputThreshold){
		facing_ = -1.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  接地判定
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ResolveGround(float deltaTime){

	const PlayerGroundState::Result result =
		groundState_.Resolve(deltaTime, MakeGroundContext(), MakeGroundParams());

	// 頭をぶつけていたら上昇を打ち切る
	if (result.hitCeiling) {
		jump_.HitCeiling();
	}

	if (result.isSupported)
	{
		// 大ジャンプの着地では押し戻しが使えないので、選んだ足場の上面へ直接置く
		if (damageFloorAirborne_ && result.hasGroundTop) {
			if (WorldTransform* transform = GetTransform()) {
				Math::Vector3 position = transform->GetTranslate();
				position.y = groundState_.CalcStandY(result.groundTopY, MakeGroundParams());
				transform->SetTranslate(position);
			}
		}

		// 着地したらダメージ床による飛行も終わり。
		// 胴体が埋まっているグループはここで猶予に入れる
		blockIgnore_.OnLanded(MakeBlockIgnoreContext());
		damageFloorAirborne_ = false;
		jump_.Land();
	} else{
		// 足場から外れたら落下させる
		jump_.LeaveGround();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  接地判定へ渡す情報
///////////////////////////////////////////////////////////////////////////////////////////////

PlayerGroundState::Context Player::MakeGroundContext() const {

	PlayerGroundState::Context context{};
	context.transform = GetTransform();
	context.rigidbody = GetRigidbody();
	context.collider = GetCollider(kColliderTag);
	context.blockField = pBlockField_;
	context.velocityY = jump_.GetVelocityY();
	context.isGrounded = jump_.IsGrounded();
	// 大ジャンプ中は当たり判定を切っているので、着地の決め方が変わる
	context.isBigJump = damageFloorAirborne_;

	return context;
}

PlayerBlockIgnore::Context Player::MakeBlockIgnoreContext() const {

	PlayerBlockIgnore::Context context{};
	context.playerCollider = GetCollider(kColliderTag);
	context.transform = GetTransform();
	context.blockField = pBlockField_;
	context.bodySize = parameter_.bodySize;
	context.bodyOffset = parameter_.bodyOffset;

	return context;
}

PlayerGroundState::Params Player::MakeGroundParams() const {

	return PlayerGroundState::Params{
		parameter_.footSize,
		parameter_.footOffset,
		parameter_.groundCheckDistance,
		parameter_.fallLimitY,
		parameter_.fixedZ,
	};
}



///////////////////////////////////////////////////////////////////////////////////////////////
//  速度の取得
///////////////////////////////////////////////////////////////////////////////////////////////

Math::Vector3 Player::GetVelocity() const
{
	const Rigidbody* rigidbody = GetRigidbody();
	if (!rigidbody)
	{
		return CVector3::ZERO;
	}
	return rigidbody->GetVelocity();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  デバッグ表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Debug_Gui()
{
	const char* bodyState = "null";
	if (IsValid())
	{
		bodyState = "resolved";
	}

	ImGui::Text("body: %s", bodyState);
	ImGui::Text("rigidbody: %s", GetRigidbody() != nullptr ? "resolved" : "null");
	ImGui::Text("jumpState: %s", jump_.GetStateName().c_str());
	ImGui::Text("animation: %s", animation_.GetCurrentName().c_str());
	ImGui::Text("hp: %.1f / %.1f%s", currentHp_, parameter_.maxHp, IsInvincible() ? " (invincible)" : "");

	ImGui::Text("connectPhase: %s", blockGroupConnectState_.GetPhaseName().c_str());
	ImGui::Text("connectRemain: %.2f", blockGroupConnectState_.GetRemainingTime());
	ImGui::Text("connectGroups: %d", static_cast<int>(blockGroupConnectState_.GetConnectedGroups().size()));
	ImGui::Text("launchTimer: %.2f", blockGroupConnectState_.GetLaunchTimer());
	ImGui::Text("launchGroups: %d", blockGroupLauncherManager_.GetGroupCount());
	ImGui::Text("activeLaunchers: %d", blockGroupLauncherManager_.GetActiveCount());

	if (WorldTransform *transform = GetTransform())
	{
		Math::Vector3 position = transform->GetTranslate();
		if(ImGui::DragFloat3("position",&position.x,0.1f)){
			transform->SetTranslate(position);
		}
	}

	if (Rigidbody *rigidbody = GetRigidbody())
	{
		Math::Vector3 velocity = rigidbody->GetVelocity();
		if(ImGui::DragFloat3("velocity",&velocity.x,0.1f)){
			rigidbody->SetVelocity(velocity);
		}
	}

	if(ImGui::CollapsingHeader("Parameter")){
		// 調整 + Save/Load
		parameter_.Debug_Gui();
	}
}
