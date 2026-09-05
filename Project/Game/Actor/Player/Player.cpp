#include "Player.h"

#include <algorithm>
#include <cmath>

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Color.h"
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
	ResolveGround();

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

	// 大ジャンプ中から降下中、ブロックの判定を再開する
	if (damageFloorAirborne_ && !blockCollisionResumed_ &&
		jump_.GetState() == PlayerJump::State::Falling &&
		previousJumpState_ != PlayerJump::State::Falling)
	{
		ResumeBlockCollision();
	}
	previousJumpState_ = jump_.GetState();

	// 判定を猶予中のBlockは、離れたものから毎フレーム判定を戻していく
	UpdateIgnoredBlocks();

	// ブロックグループの接続受付・集合・打ち上げ
	UpdateBlockGroupConnect(deltaTime);
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
	blockCollisionResumed_ = false;
	SetBlockCollisionEnabled(false);
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

///////////////////////////////////////////////////////////////////////////////////////////////
//  大ジャンプ中のBlock当たり判定の無効化・再開
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::SetBlockCollisionEnabled(bool enabled)
{
	BaseCollider* collider = GetCollider(kColliderTag);
	if (!collider)
	{
		return;
	}

	const uint32_t blockBit = AOENGINE::CollisionLayerManager::GetInstance().GetCategoryBit(kBlockCategoryName);
	if (enabled)
	{
		collider->SetCollisionMaskBit(blockBit);
	}
	else
	{
		collider->SetCollisionMaskBits(collider->GetCollisionMaskBit() & ~blockBit);
	}
}

std::vector<Block*> Player::GetOverlappingBlocks() const
{
	if (!pBlockField_)
	{
		return {};
	}

	WorldTransform* transform = GetTransform();
	if (!transform)
	{
		return {};
	}

	// Playerの現在のCollider半サイズぶんのAABBと重なるグリッドマスを調べる
	Math::Vector3 halfExtent{ 0.5f, 0.5f, 0.5f };
	if (auto* box = dynamic_cast<BoxCollider*>(GetCollider(kColliderTag)))
	{
		halfExtent = box->GetSize() * 0.5f;
	}

	// プレイヤーがブロックと重なっているかを返す
	const Math::Vector3 position = transform->GetTranslate();
	return pBlockField_->GetBlocksInWorldAABB(position - halfExtent, position + halfExtent);
}

void Player::ResumeBlockCollision()
{
	// カテゴリ判定は先に戻しておく
	SetBlockCollisionEnabled(true);
	blockCollisionResumed_ = true;

	// その上で、プレイヤーに埋まっているBlockだけ個別に無効化する
	ignoredBlocks_ = GetOverlappingBlocks();
	for (Block* block : ignoredBlocks_)
	{
		if (!block) {
			continue;
		}
		// 当たり判定無効化
		if (BaseCollider* blockCollider = block->GetCollider(kBlockCategoryName)) {
			blockCollider->SetIsActive(false);
		}
	}
}

void Player::UpdateIgnoredBlocks()
{
	// collision無効化のBlockが無ければ何もしない
	if (ignoredBlocks_.empty()) {
		return;
	}

	// 今フレーム、プレイヤーと実際に重なっているBlockを毎回取り直す
	const std::vector<Block*> currentlyOverlapping = GetOverlappingBlocks();

	for (auto it = ignoredBlocks_.begin(); it != ignoredBlocks_.end();) {
		Block* block = *it;

		// collision無効化のBlockが、今もcurrentlyOverlappingに含まれているかを見る
		const bool stillOverlapping =
			std::find(currentlyOverlapping.begin(), currentlyOverlapping.end(), block) != currentlyOverlapping.end();

		if (stillOverlapping) {
			// まだ埋まっているので、判定は無効のまま次のBlockへ
			++it;
			continue;
		}

		// 離れたBlockから判定を元に戻す
		if (block) {
			if (BaseCollider* blockCollider = block->GetCollider(kBlockCategoryName)) {
				blockCollider->SetIsActive(true);
			}
		}
		// 判定を戻したので猶予リストからも外す
		it = ignoredBlocks_.erase(it);
	}
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
	ignoredBlocks_.clear();
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

void Player::ResolveGround(){
	bool isSupported = false;

	// Blockからの押し戻しで足場に乗ったかを見る
	if(ResolvePushback()){
		isSupported = true;
	}

	if (isSupported)
	{
		// 降下タイミングでブロックの当たり判定再開
		if (damageFloorAirborne_ && !blockCollisionResumed_)
		{
			ResumeBlockCollision();
		}
		// 着地したらダメージ床による飛行も終わり
		damageFloorAirborne_ = false;
		jump_.Land();
	} else{
		// 足場から外れたら落下させる
		jump_.LeaveGround();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  押し戻しの反映
///////////////////////////////////////////////////////////////////////////////////////////////

bool Player::ResolvePushback(){
	BaseCollider* collider = GetCollider(kColliderTag);
	if (!collider)
	{
		return false;
	}

	const Math::Vector3& pushback = collider->GetPushBackDirection();

	if(pushback.y > kPushbackThreshold){
		// 下から押し戻された = 足場の上に乗っている
		return true;
	}

	if(pushback.y < -kPushbackThreshold){
		// 上から押し戻された = 頭をぶつけた
		jump_.HitCeiling();
	}

	return false;
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
