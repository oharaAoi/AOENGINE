#include "Player.h"

#include <algorithm>
#include <cmath>

#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Render/Render.h"

#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"
#include "Game/WorldObject/Wall.h"

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
	noMoveInputTimer_ = 0.0f;

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

	// 接地中は足場の上面へ吸着させる
	SnapToGround();

	// 落ちすぎないように下限で止める
	ClampFallLimit();

	// 奥行きは動かさない
	FixZPosition();

	// 見た目まわり
	UpdateScale();
	UpdateFacingRotate(deltaTime);
	UpdateAnimation(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  落下の下限
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ClampFallLimit() {

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	Math::Vector3 position = transform->GetTranslate();
	if (position.y > parameter_.fallLimitY) {
		return;
	}

	// 下限より下がったら、その高さに置き直して止める
	position.y = parameter_.fallLimitY;
	transform->SetTranslate(position);

	// 速度を残すと毎フレーム下限へ引き戻す形になるので、落下を止めておく
	if (Rigidbody* rigidbody = GetRigidbody()) {
		rigidbody->SetVelocityY(0.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  足元の足場判定
///////////////////////////////////////////////////////////////////////////////////////////////

bool Player::TryGetGroundTop(float checkDown, float& outTopY) const {

	if (pBlockField_ == nullptr) {
		return false;
	}

	// 大ジャンプ中はブロックとの判定自体を切っているので、足場としても見ない
	if (damageFloorAirborne_ && !blockCollisionResumed_) {
		return false;
	}

	// 上がっている最中に拾ってしまうと、飛び出した瞬間に着地扱いになる
	if (jump_.GetVelocityY() > 0.0f) {
		return false;
	}

	const WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return false;
	}

	// 当たり判定の中心と足元の高さを出す
	const Math::Vector3 center = transform->GetTranslate() + parameter_.hitOffset;
	const float feetY = center.y - parameter_.hitSize.y * 0.5f;
	const float halfWidth = parameter_.hitSize.x * 0.5f;

	// 足元の下だけを見る
	const Math::Vector3 checkMin{ center.x - halfWidth, feetY - checkDown, center.z };
	const Math::Vector3 checkMax{ center.x + halfWidth, feetY - kGroundCheckEpsilon, center.z };

	bool found = false;
	float topY = 0.0f;

	// 複数マスに跨っている時は、一番高い上面に乗せる
	const auto keepHighest = [&found, &topY](float surfaceY) {
		if (!found || surfaceY > topY) {
			found = true;
			topY = surfaceY;
		}
	};

	for (const Block* block : pBlockField_->GetBlocksInWorldAABB(checkMin, checkMax)) {
		if (block == nullptr || !block->IsValid()) {
			continue;
		}

		// 大ジャンプ後に判定を猶予しているブロックは踏めない扱いにする
		if (std::find(ignoredBlocks_.begin(), ignoredBlocks_.end(), block) != ignoredBlocks_.end()) {
			continue;
		}

		keepHighest(block->GetPosition().y + kBlockHalfHeight);
	}

	// 開幕の床やフィールドの境目はマップチップ2で出来ているので、そちらも足場として見る
	for (const Wall* wall : pBlockField_->GetWallsInWorldAABB(checkMin, checkMax)) {
		if (wall == nullptr || !wall->IsValid()) {
			continue;
		}

		keepHighest(wall->GetPosition().y + kBlockHalfHeight);
	}

	outTopY = topY;
	return found;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  足場への吸着
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::SnapToGround() {

	if (!jump_.IsGrounded()) {
		return;
	}

	float groundTopY = 0.0f;
	if (!TryGetGroundTop(parameter_.groundCheckDistance, groundTopY)) {
		return;
	}

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	// 接地中は足場へ押し付ける速度がかかり続けるので、押し戻し任せだと
	// 毎フレーム沈んでは戻される形になって上下に震える。
	// 下にあるブロックの上面は分かっているので、その高さへ直接置き直す
	Math::Vector3 position = transform->GetTranslate();
	position.y = groundTopY + parameter_.hitSize.y * 0.5f - parameter_.hitOffset.y;
	transform->SetTranslate(position);

	// 沈み込む速度を残さない
	if (Rigidbody* rigidbody = GetRigidbody()) {
		rigidbody->SetVelocityY(0.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  奥行きの固定
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::FixZPosition() {

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	// Zの位置は固定
	Math::Vector3 position = transform->GetTranslate();
	position.z = parameter_.fixedZ;
	transform->SetTranslate(position);

	// 速度が残っていると次のフレームでまたずれるので、こちらも止めておく
	if (Rigidbody* rigidbody = GetRigidbody()) {
		rigidbody->SetVelocityZ(0.0f);
	}
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

	// 移動しているかは、入力と実際の速度の両方で見る
	const bool hasMoveInput = std::abs(input_.GetHorizontal()) > parameter_.moveInputThreshold;
	const bool isMovingX = std::abs(GetVelocity().x) > parameter_.moveInputThreshold;
	if (hasMoveInput || isMovingX) {
		noMoveInputTimer_ = 0.0f;
	} else {
		noMoveInputTimer_ += deltaTime;
	}

	BaseGameObject* body = GetGameObject();
	if (body == nullptr) {
		return;
	}

	Animator* animator = body->GetAnimator();
	if (animator == nullptr) {
		return;
	}

	// 今の状態から流したいアニメーションを決める
	std::string next = kIdleAnimation;
	if (!jump_.IsGrounded()) {
		// 上昇・滞空・落下はまとめてジャンプ扱い
		next = kJumpAnimation;
	} else if (noMoveInputTimer_ < parameter_.idleAnimationDelay) {
		next = kWalkAnimation;
	}

	// 補間中は何も投げず、終わってから実際の名前と見比べて切り替える
	if (animator->GetIsAnimationChange()) {
		return;
	}

	if (animator->GetAnimationName() == next) {
		return;
	}

	animator->TransitionAnimation(next, parameter_.animationBlendSpeed);

	// 歩きだけ少し速く回す
	float speed = parameter_.animationSpeed;
	if (next == kWalkAnimation) {
		speed = parameter_.walkAnimationSpeed;
	}
	animator->SetAnimationSpeed(speed);
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

void Player::ResolveGround(float deltaTime){

	// 天井にぶつかった時の処理が入っているので、押し戻しは必ず通す
	const bool pushedUp = ResolvePushback();

	// 前フレームに進んだぶんも見る範囲に足しておく
	const float fallDistance = std::abs(jump_.GetVelocityY()) * deltaTime;

	//
	float groundTopY = 0.0f;
	const bool onBlock = TryGetGroundTop(parameter_.groundCheckDistance + fallDistance, groundTopY);

	if (pushedUp || onBlock)
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
