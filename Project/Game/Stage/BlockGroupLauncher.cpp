#include "BlockGroupLauncher.h"

/// game
#include "Game/Actor/Common/ScreenWorldPlaneAnchor.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"
#include "Game/Path/LineLoader.h"
#include "Game/UI/ComboTextUIManager.h"
#include "Game/UI/DamageTextUIManager.h"
#include "Game/Battle/BlockDamageCalculator.h"

/// engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Render/Render.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/System/Manager/ParticleEffectManager.h"

/// stl
#include <algorithm>
#include <cmath>
#include <limits>

using namespace AOENGINE;

namespace{
	/// 1フレームで重なりを解き直す回数。3グループ以上が重なると押した先でまた重なるため数回繰り返す
	constexpr int kSeparationIteration = 4;
	/// 重なり判定の許容誤差。隣り合って接しているだけの状態を重なりとみなさないための下限
	constexpr float kOverlapEpsilon = 1.0e-4f;
	/// 速さの上限が指定されていない時に使う、実質上限なしとみなせる値
	constexpr float kUnlimitedLaunchSpeed = 1.0e4f;
	/// Lineが指定されていない時に作る、真上への経路の最短の長さ
	constexpr float kMinLaunchPathLength = 1.0f;
	/// 真上への経路に持たせる余裕。寿命が尽きる前に経路が尽きないように少し長くしておく
	constexpr float kLaunchPathMargin = 1.1f;
	/// 終点へ着いてから制御を手放すまでの時間。当たり判定が入るのを待つためのもの
	constexpr float kArrivalGraceTime = 0.5f;
	/// 向きの計算で「長さが無い」とみなす下限
	constexpr float kAimEpsilon = 1.0e-4f;
	/// これ以上向きが揃っていたら回さない(acosの誤差でNaNになるのを避ける)
	constexpr float kAimParallelDot = 0.99999f;
	/// 進む向きを求める時に、経路の何ユニット先を見るか
	constexpr float kDirectionSampleDistance = 0.5f;

	/// <summary>
	/// 1軸分について、2つのブロックが重なっている「残り距離」の範囲を絞り込む。
	/// 中心の差は offset + direction * 残り距離 になるため、|offset + direction * 残り距離| < blockSize を解く
	/// </summary>
	/// <returns>絞り込んだ範囲が空になったら false(この軸では触れ合わない)</returns>
	bool ClipContactRange(float offset,float direction,float blockSize,float& nearDistance,float& farDistance){
		if(std::fabs(direction) <= kOverlapEpsilon){
			// この軸には近づいていかないため、最初から重なっていなければ触れることはない
			return std::fabs(offset) < blockSize;
		}

		const float edge0 = (-blockSize - offset) / direction;
		const float edge1 = (blockSize - offset) / direction;

		nearDistance = (std::max)(nearDistance,(std::min)(edge0,edge1));
		farDistance = (std::min)(farDistance,(std::max)(edge0,edge1));
		return nearDistance <= farDistance;
	}
}

void BlockGroupLauncher::BeginGather(const GatherRequest& request,const Params& params){
	Clear();

	params_ = params;

	if(pField_ == nullptr){
		return;
	}

	currentGatherIndex_ = 0;

	// 総ダメージはここへ出す。集合中に変わることはないので、依頼された時点で覚えておく
	gatherPoint_ = request.gatherPoint;

	for(size_t index = 0; index < request.targets.size(); ++index){
		GatheringGroup group{};
		if(!MakeGatheringGroup(request,index,group)){
			// 集合させられなかったグループは動き出す機会が無いため、
			// ここで表示を消し始めておかないと出しっぱなしになる。
			// 集合しないので、集めた数にも入れない
			FadeOutComboText(request.targets[index].groupId);
			continue;
		}

		// 集めたグループはもうグリッド上の足場ではないので、連結グループの表から外し、
		// 段の所有からも切り離す（元の段がストリーミングで消えても一緒に破棄されないようにする）。
		// ブロックの実体はこの後もこのクラスが動かすため、Destroy はしない。
		pField_->DetachGroup(request.targets[index].groupId);

		groups_.push_back(std::move(group));
	}

	if(groups_.empty()){
		return;
	}

	// 最初に接続したグループだけが最初から動き、後ろのグループは1つ前の塊が触れてから動き出す
	groups_.front().isMoving = true;
	// 先頭は UpdateGatherRelease() を通らずに動き出すため、数え上げもここで行う。
	// 1グループだけの場合はこの時点で集合しきるため、総ダメージもここで出る
	NotifyGatherStarted(groups_.front());
	SetupReleaseProgress();

	state_ = State::Gathering;
}

bool BlockGroupLauncher::MakeGatheringGroup(const GatherRequest& request,size_t targetIndex,GatheringGroup& outGroup) const{
	const Target& target = request.targets[targetIndex];

	const std::vector<Block*>* members = pField_->GetGroup(target.groupId);
	if(members == nullptr || members->empty()){
		return false;
	}

	// コンボ表示は groupId で引き当てるため、集合中も元のIDを持ち続ける
	outGroup.groupId = target.groupId;

	// DetachGroup() で members が無効になるため、ここで実体をコピーしておく(非所有ポインタの配列)
	outGroup.blocks = *members;

	// 接続した瞬間のプレイヤーとの位置関係を保ったまま移動させる。
	// こうすると集合地点でも「乗った時と同じようにプレイヤーの足元にある」状態になる
	outGroup.offsets.reserve(outGroup.blocks.size());
	for(const Block* block : outGroup.blocks){
		outGroup.offsets.push_back(GetBlockPosition(block) - target.connectPosition);
	}

	// 経路: 自分の接続地点 -> 次に接続したブロック -> ... -> 集合地点。
	// 各区間は直線なので、次のブロックへは最短距離で渡っていく
	outGroup.path.reserve(request.targets.size() - targetIndex + 1);
	outGroup.path.push_back(target.connectPosition);
	for(size_t index = targetIndex + 1; index < request.targets.size(); ++index){
		outGroup.path.push_back(request.targets[index].connectPosition);
	}
	outGroup.path.push_back(request.gatherPoint);

	outGroup.pathLength = ComputePathLength(outGroup.path);
	outGroup.progress = 0.0f;

	// 集合中は落下させたくないので重力を切り、残っている速度も消しておく
	for(Block* block : outGroup.blocks){
		if(block == nullptr || !block->IsValid()){
			continue;
		}
		AOENGINE::BaseGameObject* gameObject = block->GetGameObject();
		if(gameObject == nullptr){
			continue;
		}
		if(AOENGINE::Rigidbody* rigidbody = gameObject->GetRigidbody()){
			rigidbody->SetGravity(false);
			rigidbody->SetVelocity(CVector3::ZERO);
		}
	}

	return true;
}

void BlockGroupLauncher::SetupReleaseProgress(){
	for(size_t index = 1; index < groups_.size(); ++index){
		GatheringGroup& group = groups_[index];
		const GatheringGroup& previous = groups_[index - 1];

		// 後ろのグループの経路は、1つ前のグループの経路の後半がそのまま使われている。
		// そのためノード数の差が「1つ前のグループが自分の接続地点へ入ってくる区間」を指し、
		// 経路長の差がそこへ辿り着くまでに進む距離になる(経路を辿り直さなくてよい)
		if(previous.path.size() <= group.path.size()){
			continue;
		}
		const size_t nodeIndex = previous.path.size() - group.path.size();
		const float arriveDistance = previous.pathLength - group.pathLength;

		Math::Vector3 approach = previous.path[nodeIndex] - previous.path[nodeIndex - 1];
		const float approachLength = approach.Length();
		if(approachLength <= kOverlapEpsilon){
			// ほぼ同じ場所で接続された場合は最初から重なっているため、待たずに一緒に動き出す
			group.releaseProgress = 0.0f;
			continue;
		}
		approach = approach * (1.0f / approachLength);

		// 触れる瞬間の残り距離だけ手前で動き出させる。
		// 接続地点へ着くまで待たせると、待っている側の塊にめり込んでから動き出すことになる
		const float contactDistance = ComputeContactDistance(previous,group,approach,params_.blockSize);
		group.releaseProgress = arriveDistance - contactDistance;
	}
}

void BlockGroupLauncher::Launch(){
	if(state_ != State::Gathering){
		return;
	}

	// 打ち上げると集合の更新が止まるため、待機中のグループはここで打ち切る
	ReleaseRemainingGroups();

	state_ = State::Launched;
	launchTimer_ = params_.launchLifeTime;
	arrivalTimer_ = kArrivalGraceTime;

	// CollisionCatgoryを 切り替えて、Bossと衝突するように
	for(const GatheringGroup& group : groups_){
		for(Block* block : group.blocks){
			if(block == nullptr || !block->IsValid()){
				continue;
			}
			BaseCollider* collider = block->GetCollider("Block");
			collider->SetCategory("LaunchedBlock");
			collider->SetTarget("Boss");    // マスクに Boss を足す
		}
	}

	// 集合が終わった時点の塊を1つの座標系にまとめる。ここから先はこの座標系だけを動かす
	BuildLaunchRoot();

	// 狙い先を持っている場合は必ず当てにいく。
	// 進めない設定(初速も加速度も0)の場合は終点へ辿り着けないため、今まで通り寿命で打ち切る
	isHoming_ = hasTarget_ && (params_.launchSpeed > 0.0f || params_.launchAccel > 0.0f);
	aimPosition_ = GetAimGoal();

	// 座標系の原点(噴射位置)を始点にした経路を組み立てる。打ち上げ中はこの経路上を進む
	BuildLaunchPath(launchRoot_ != nullptr ? launchRoot_->GetTranslate() : CVector3::ZERO);

	// 打ち上げた直後は集合した時の向き(真上)のまま。ここから進む向きへ傾けていく
	launchDirection_ = CVector3::UP;
	launchRotate_ = Math::Quaternion();
	if(launchRoot_ != nullptr){
		launchRoot_->SetRotate(launchRotate_);
		launchRoot_->Update();
	}

	// 演出オブジェクトは座標系を親にしておく。座標系の原点が噴射位置なので、
	// ローカル座標は原点のままでよく、座標系が上昇すれば一緒に上がってくれる。
	// オブジェクトは打ち上げごとに作り直さず、このランチャーが生きている間は使い回す
	if(launchEffects_.IsEmpty()){

		// launchEffects_.AddParticleEffect("RocketJet_AllParticle");

		launchEffects_.AddParticleEffect("RocketJet_SubParticle");
		launchEffects_.AddPrefab("JetFire",Math::Vector3(0.f,1.3f,0.f));
	}

	launchEffects_.SetParent(launchRoot_.get());
	launchEffects_.Play();

	// 打ち上げの合図は画面の中央に出す。launchRoot_ を親にすると塊と一緒に上がってしまうため、
	// 親は付けずにワールド座標で置き、画面中央へ毎フレーム合わせ直す
	if(shotUiEffects_.IsEmpty()){
		shotUiEffects_.AddParticle("HasshaParticle");
	}

	shotUiEffects_.SetParent(nullptr);
	shotUiEffects_.SetLocalPosition(CalclateShotUiPos());
	shotUiEffects_.Play();

	//se
	Engine::GetSoundManager()->Play("LaunchBlocks");
}

void BlockGroupLauncher::BuildLaunchRoot(){
	// 噴射位置(塊の下端中央)を座標系の原点にする。
	// こうしておくとパーティクルは親からの相対位置が原点のままで済む
	Math::Vector3 rootPos{};
	CalclateJetPos(rootPos);

	// 座標系はランチャーを使い回しても作り直さない(WorldTransform は定数バッファを持つため)
	if(launchRoot_ == nullptr){
		launchRoot_ = std::make_unique<AOENGINE::WorldTransform>();
		launchRoot_->Init();
	}
	launchRoot_->SetTranslate(rootPos);
	launchRoot_->Update();

	// 各グループの位置を、経路上の位置から座標系の原点を基準にした相対位置へ置き換える。
	// 打ち上げ中はこの相対位置を保ったまま座標系ごと動かすので、集合した並びがそのまま上がっていく
	for(GatheringGroup& group : groups_){
		group.rootOffset = (group.basePoint + group.separation) - rootPos;
	}

	// 座標系の原点は塊の下端の更に下(噴射位置)にあるため、原点を狙い先へ向けると
	// 塊は狙い先の上を通り過ぎてしまう。中心を合わせられるように、原点から見た中心を覚えておく
	launchCenterOffset_ = CVector3::ZERO;

	Math::Vector3 centerSum = CVector3::ZERO;
	int validCount = 0;
	for(const GatheringGroup& group : groups_){
		for(const Block* block : group.blocks){
			if(block == nullptr || !block->IsValid()){
				continue;
			}
			centerSum = centerSum + GetBlockPosition(block);
			++validCount;
		}
	}

	if(validCount > 0){
		launchCenterOffset_ = centerSum * (1.0f / static_cast<float>(validCount)) - rootPos;
	}

	// 塊ごと進む向きへ回すため、打ち上げた時点の各ブロックの向きを覚えておく。
	// 毎フレームこの向きに回転を掛け直すので、回し続けても向きがずれていかない
	for(GatheringGroup& group : groups_){
		group.launchRotates.clear();
		group.launchRotates.reserve(group.blocks.size());

		for(const Block* block : group.blocks){
			const AOENGINE::WorldTransform* transform = (block != nullptr) ? block->GetTransform() : nullptr;
			group.launchRotates.push_back(transform != nullptr ? transform->GetRotate() : Math::Quaternion());
		}
	}
}

void BlockGroupLauncher::BuildLaunchPath(const Math::Vector3& startPos){
	// 打ち上げの初速と加速度は、そのまま経路上を進む速さとして使う
	LinePathMover::Params moverParams{};
	moverParams.accel = params_.launchAccel;
	moverParams.initVelocity = params_.launchSpeed;
	// 上限が指定されていない場合は、寿命が尽きるまで加速し続けられるように十分大きな値を入れておく
	moverParams.maxVelocity = (params_.launchMaxSpeed > 0.0f) ? params_.launchMaxSpeed : kUnlimitedLaunchSpeed;
	// 塊はブロックの並びを保ったまま進ませるため、進行方向を向く回転は使わない
	moverParams.rotateLerpRate = 0.0f;

	// 初速は SetPoints() の中の Reset() で入るため、先に調整値を渡しておく
	launchMover_.SetParams(moverParams);
	// 終点まで来たらそこで止める(閉じたLineでも周回はさせない)
	launchMover_.SetEndAction(LinePathMover::EndAction::Stop);
	const std::vector<Math::Vector3> points = MakeLaunchPoints(startPos);

	// 狙い先へ向け直す時の基準になるため、組み立てた経路の始点と終点を覚えておく
	launchPathStart_ = points.front();
	launchPathEnd_ = points.back();

	launchMover_.SetPoints(points,false);
}

std::vector<Math::Vector3> BlockGroupLauncher::MakeLaunchPoints(const Math::Vector3& startPos) const{
	// Lineが指定されていれば、その形をそのまま辿る。
	// Lineの始点は書き出した場所のままなので、集合しきった位置から動き出すように平行移動させる
	if(!params_.launchLineName.empty()){
		const LineLoader::LineData* lineData = LineLoader::GetInstance()->GetLineData(params_.launchLineName);
		if(lineData != nullptr && lineData->points.size() >= 2){
			std::vector<Math::Vector3> points = lineData->points;
			const Math::Vector3 offset = startPos - points.front();
			for(Math::Vector3& point : points){
				point = point + offset;
			}
			return points;
		}
	}

	// Lineが無く狙い先がある場合は、狙い先までまっすぐ飛ばす
	if(isHoming_){
		const Math::Vector3 aimGoal = GetAimGoal();
		if((aimGoal - startPos).Length() > kMinLaunchPathLength){
			return {startPos,aimGoal};
		}
	}

	// Lineも狙い先も無い場合は今まで通り真上へ。
	// 制御を手放すまでに進みうる距離より長くしておき、寿命より先に経路が尽きないようにする
	const float lifeTime = (std::max)(params_.launchLifeTime,0.0f);
	const float reachLength = (params_.launchSpeed * lifeTime + 0.5f * params_.launchAccel * lifeTime * lifeTime) * kLaunchPathMargin;

	return {startPos,startPos + CVector3::UP * (std::max)(reachLength,kMinLaunchPathLength)};
}

void BlockGroupLauncher::Update(float deltaTime){
	// ボスに当たった塊は当たり判定の外側であるここで消す
	if(isBossHit_){
		DestroyBlocks();
		return;
	}

	switch(state_){
		case State::Gathering:
			UpdateGathering(deltaTime);
			break;
		case State::Launched:
			UpdateLaunched(deltaTime);
			break;
		case State::Idle:
		default:
			break;
	}

	launchEffects_.Update(deltaTime);

	// カメラが動いても画面の中央に出ているように見せたいので、毎フレーム画面中央へ合わせ直す
	shotUiEffects_.SetLocalPosition(CalclateShotUiPos());
	shotUiEffects_.Update(deltaTime);
}

void BlockGroupLauncher::UpdateGathering(float deltaTime){
	// 各グループは自分が接続された地点で待機し、1つ前のグループの塊が触れる所まで来てから、
	// 後から接続したブロックを順に辿って集合地点へ向かう。
	// 動き出した後は全グループが同じ速さで同じ道を進むため間隔は変わらず、
	// 触れ合ったまま数珠つなぎで進んで順番に集合する
	UpdateGatherRelease(deltaTime);

	for(GatheringGroup& group : groups_){
		if(group.isMoving){
			group.progress = (std::min)(group.progress + params_.gatherSpeed * deltaTime,group.pathLength);
		}
		// 待機中は progress が 0 のままなので、接続地点から動かない
		group.basePoint = SamplePath(group.path,group.progress);
	}

	// 経路上の位置のままだと集合地点で全グループが重なってしまうため、
	// 重なった分だけ互いに押し戻す。後から来たグループが先に居るグループを押し出すので、
	// 集まるほど塊が段々と大きくなる
	ResolveGroupSeparation(deltaTime);

	for(const GatheringGroup& group : groups_){
		// 集合中は塊を回さないので単位回転を渡す
		MoveGroup(group,group.basePoint + group.separation,Math::Quaternion(),deltaTime);
	}
}

void BlockGroupLauncher::UpdateGatherRelease(float deltaTime){
	// このフレームで進む分を先に足してから調べる。触れてから動き出すと、
	// 1フレームで進んだ分だけめり込んだ間隔のまま並走することになり、
	// 集合地点へ着くまでずっと押し戻しが効き続けてしまう
	const float step = params_.gatherSpeed * deltaTime;

	for(size_t index = 1; index < groups_.size(); ++index){
		GatheringGroup& group = groups_[index];
		if(group.isMoving){
			continue;
		}

		// 1つ前が止まっているなら、そこから後ろは全部止まったまま
		const GatheringGroup& previous = groups_[index - 1];
		if(!previous.isMoving){
			break;
		}

		if(previous.progress + step < group.releaseProgress){
			break;
		}

		// 同じ場所で接続されていた場合はここで続けて動き出すため、後ろも続けて調べる
		group.isMoving = true;
		currentGatherIndex_ = static_cast<int>(index);

		// このグループの分を集めた数へ足し込む(コンボ表示もここで消え始める)
		NotifyGatherStarted(group);

		// effect
		ParticleEffectManager::GetInstance()->Play("ComboParticle",group.basePoint);
		// se
		Engine::GetSoundManager()->Play("GatherBlocks");
	}
}

void BlockGroupLauncher::ResolveGroupSeparation(float deltaTime){
	const size_t groupCount = groups_.size();
	if(groupCount < 2 || params_.blockSize <= 0.0f){
		return;
	}

	// 今の押し戻し量を初期値にして、重なりが無くなるまで少しずつ離していく
	std::vector<Math::Vector3> offsets;
	offsets.reserve(groupCount);
	for(const GatheringGroup& group : groups_){
		offsets.push_back(group.separation);
	}

	for(int iteration = 0; iteration < kSeparationIteration; ++iteration){
		bool pushed = false;

		for(size_t indexA = 0; indexA < groupCount; ++indexA){
			for(size_t indexB = indexA + 1; indexB < groupCount; ++indexB){
				// ぴったり重なっていると押し出す向きが決まらないため、
				// グループごとに違う向きへ逃がして集合地点の周りへ放射状に広がるようにする
				const Math::Vector3 fallback = MakeFallbackDirection(indexB,groupCount);

				Math::Vector3 push{};
				if(!ComputeGroupPush(groups_[indexA],groups_[indexA].basePoint + offsets[indexA],
				   groups_[indexB],groups_[indexB].basePoint + offsets[indexB],
				   fallback,push)){
					continue;
				}

				// めり込んだ分を半分ずつ分け合い、両方が同じだけ離れるようにする
				offsets[indexA] = offsets[indexA] - push * 0.5f;
				offsets[indexB] = offsets[indexB] + push * 0.5f;
				pushed = true;
			}
		}

		if(!pushed){
			break;
		}
	}

	// 一気に弾き飛ばさず、押し戻しの速さの分だけ広がるようにする
	const float maxMove = params_.separationSpeed * deltaTime;
	for(size_t index = 0; index < groupCount; ++index){
		Math::Vector3 diff = offsets[index] - groups_[index].separation;
		const float length = diff.Length();
		if(length > maxMove){
			diff = diff * (maxMove / length);
		}
		groups_[index].separation = groups_[index].separation + diff;
	}
}

bool BlockGroupLauncher::ComputeGroupPush(const GatheringGroup& groupA,const Math::Vector3& originA,
										  const GatheringGroup& groupB,const Math::Vector3& originB,
										  const Math::Vector3& fallbackDirection,Math::Vector3& outPush) const{
	// ブロックはグリッド上に並ぶ同じ大きさの箱なので、箱同士のめり込み量から押し出す量を求める
	const float blockSize = params_.blockSize;

	Math::Vector3 sum = CVector3::ZERO;
	int overlapCount = 0;

	for(size_t indexA = 0; indexA < groupA.blocks.size(); ++indexA){
		const Block* blockA = groupA.blocks[indexA];
		if(blockA == nullptr || !blockA->IsValid()){
			continue;
		}
		const Math::Vector3 posA = originA + groupA.offsets[indexA];

		for(size_t indexB = 0; indexB < groupB.blocks.size(); ++indexB){
			const Block* blockB = groupB.blocks[indexB];
			if(blockB == nullptr || !blockB->IsValid()){
				continue;
			}
			const Math::Vector3 posB = originB + groupB.offsets[indexB];

			const Math::Vector3 diff = posB - posA;
			const float overlapX = blockSize - std::fabs(diff.x);
			const float overlapY = blockSize - std::fabs(diff.y);
			const float overlapZ = blockSize - std::fabs(diff.z);

			// 1軸でも離れていれば重なっていない(隣り合って接しているだけの場合もここで弾く)
			if(overlapX <= kOverlapEpsilon || overlapY <= kOverlapEpsilon || overlapZ <= kOverlapEpsilon){
				continue;
			}

			// 中心がぴったり重なっている場合はどちらへ押すかが決まらないので、決めた向きへ逃がす
			if(std::fabs(diff.x) <= kOverlapEpsilon && std::fabs(diff.y) <= kOverlapEpsilon){
				sum = sum + fallbackDirection * blockSize;
				++overlapCount;
				continue;
			}

			// ブロックは XY 平面に並ぶため、押し出しは X と Y の浅い方だけを使う。
			// Z へ押すと盤面から浮いてしまう
			if(overlapX <= overlapY){
				sum.x += (diff.x >= 0.0f) ? overlapX : -overlapX;
			} else{
				sum.y += (diff.y >= 0.0f) ? overlapY : -overlapY;
			}
			++overlapCount;
		}
	}

	if(overlapCount == 0){
		return false;
	}

	// ブロック1個ずつの押し出しを平均して、グループ全体を1つの塊として動かす
	outPush = sum * (1.0f / static_cast<float>(overlapCount));

	// 押し出しが打ち消し合って向きが決まらなかった場合も、決めた向きへ逃がす
	if(outPush.Length() <= kOverlapEpsilon){
		outPush = fallbackDirection * blockSize;
	}

	return true;
}

void BlockGroupLauncher::UpdateLaunched(float deltaTime){
	// 加速も含めて経路上の位置は LinePathMover が持つ
	launchMover_.Update(deltaTime);

	// 進めた後の進捗で向ける先を決める。終点へ着いたフレームで狙い先と一致させたいため、
	// 進める前ではなくここで寄せる
	UpdateLaunchAim(deltaTime);

	if(launchRoot_ != nullptr && launchMover_.IsValid()){
		// ブロックを1個ずつ動かすのではなく、まとめた座標系を経路上の位置へ移す。
		// 噴射パーティクルはこの座標系を親にしているため、何もしなくても一緒に付いてくる
		const Math::Vector3 rootPos = ApplyLaunchAim(launchMover_.GetPosition());
		launchDirection_ = CalclateLaunchDirection(rootPos);

		UpdateLaunchRotate(deltaTime);

		launchRoot_->SetTranslate(rootPos);
		// 噴射などの演出はこの座標系を親にしているため、座標系ごと進む向きへ向ける。
		// 真上へ打ち上げる前提で置いた演出(JetFireのローカル座標や炎の向き)が
		// そのまま進む向きの後ろへ付いてくるように、真上を進む向きへ合わせる回転にしている
		launchRoot_->SetRotate(launchRotate_);
		launchRoot_->Update();

		// ブロックは座標系から見た相対位置へ追従させる。
		// 相対位置も向きも同じ回転を掛けるので、塊は形を保ったまま進む向きへ倒れていく
		for(const GatheringGroup& group : groups_){
			MoveGroup(group,rootPos + launchRotate_ * group.rootOffset,launchRotate_,deltaTime);
		}
	}

	// 終点まで着いた後は、当たり判定が入るまで少しの間その場に留める。
	// 着いた瞬間に手放すと、ランチャーから切り離された後に当たってダメージが乗らなくなる
	if(launchMover_.IsFinished()){
		arrivalTimer_ -= deltaTime;
		if(arrivalTimer_ > 0.0f){
			return;
		}

		// 着いたのに当たり判定が入らなかった場合の保険。
		// そのまま手放すと空中で止まって見えるため、重力を戻して落ちるようにする
		ReleaseBlocksWithGravity();
		Clear();
		return;
	}

	launchTimer_ -= deltaTime;
	// 狙い先を追いかけている間は、必ず当てたいので寿命では打ち切らない
	if(isHoming_ || launchTimer_ > 0.0f){
		return;
	}

	// 画面外まで飛んで行った後は速度を残したまま制御を手放す
	Clear();
}

void BlockGroupLauncher::ReleaseBlocksWithGravity(){
	for(const GatheringGroup& group : groups_){
		for(Block* block : group.blocks){
			if(block == nullptr || !block->IsValid()){
				continue;
			}
			AOENGINE::BaseGameObject* gameObject = block->GetGameObject();
			if(gameObject == nullptr){
				continue;
			}
			if(AOENGINE::Rigidbody* rigidbody = gameObject->GetRigidbody()){
				// 集合を始めた時に切った重力を戻す
				rigidbody->SetGravity(true);
			}
		}
	}
}

void BlockGroupLauncher::SetTarget(const Math::Vector3& position){
	targetPosition_ = position;
	hasTarget_ = true;
}

void BlockGroupLauncher::ClearTarget(){
	hasTarget_ = false;
	// 飛んでいる最中に外された場合は、今向けている先へそのまま飛んでいく
	isHoming_ = false;
}

void BlockGroupLauncher::SetScreenViewProjection(const Math::Matrix4x4& viewProjection){
	screenViewProjection_ = viewProjection;
	hasScreenViewProjection_ = true;

	// Update() はカメラの更新より前に呼ばれるため、そこで求めた位置は1フレーム前のカメラのものになる。
	// これはカメラの更新後に渡されるので、ここで取り直しておくと
	// Particle が実際に射出される PostUpdate までに今フレームのカメラの見え方が反映される
	shotUiEffects_.SetLocalPosition(CalclateShotUiPos());
}

void BlockGroupLauncher::UpdateLaunchAim(float deltaTime){
	if(!isHoming_ || !hasTarget_){
		return;
	}

	// 狙い先(ボス)は画面上の位置に貼り付いているため、カメラが揺れるとワールド座標も揺れる。
	// そのまま経路へ乗せるとガタつくので少しずつ寄せる
	float rate = 1.0f;
	if(params_.launchHomingRate > 0.0f){
		rate = std::clamp(params_.launchHomingRate * deltaTime,0.0f,1.0f);
	}

	// 終点へ近づくほど強く寄せる。着く時(進捗1)には必ず狙い先と一致するため、狙いが外れない
	const float progress = launchMover_.GetProgress();
	rate = (std::max)(rate,progress * progress);

	aimPosition_ = Math::Vector3::Lerp(aimPosition_,GetAimGoal(),rate);
}

Math::Vector3 BlockGroupLauncher::ApplyLaunchAim(const Math::Vector3& pathPosition) const{
	if(!isHoming_){
		return pathPosition;
	}

	// 始点は動かさず、終点が今向けている先に来るように経路ごと回して伸び縮みさせる。
	// 経路の形は保たれるため、終点だけ差し替えた時のように途中が折れ曲がらない
	const Math::Vector3 baseVector = launchPathEnd_ - launchPathStart_;
	const Math::Vector3 aimVector = aimPosition_ - launchPathStart_;

	const float baseLength = baseVector.Length();
	const float aimLength = aimVector.Length();
	if(baseLength <= kAimEpsilon || aimLength <= kAimEpsilon){
		return pathPosition;
	}

	const Math::Vector3 baseDirection = baseVector * (1.0f / baseLength);
	const Math::Vector3 aimDirection = aimVector * (1.0f / aimLength);
	const Math::Quaternion rotate = MakeAimRotation(baseDirection,aimDirection);

	// 始点から終点へ向かう成分と、そこから横へ膨らんでいる成分に分ける
	const Math::Vector3 local = pathPosition - launchPathStart_;
	const float along = Math::Vector3::Dot(local,baseDirection);
	const Math::Vector3 side = local - baseDirection * along;

	// 伸縮させるのは進む向きの成分だけにする。
	// 全体を相似で伸ばすと、狙い先が遠いほどLineの膨らみも大きくなって画面外へ出てしまう
	return launchPathStart_ + aimDirection * (aimLength * (along / baseLength)) + rotate * side;
}

void BlockGroupLauncher::UpdateLaunchRotate(float deltaTime){
	// 真上に置かれた状態から、進む向きへ合わせる回転
	const Math::Quaternion target = MakeAimRotation(CVector3::UP,launchDirection_);

	if(params_.launchRotateRate <= 0.0f){
		launchRotate_ = target;
		return;
	}

	// 打ち上げた瞬間に倒れると不自然なので、少しずつ進む向きへ傾けていく
	const float rate = std::clamp(params_.launchRotateRate * deltaTime,0.0f,1.0f);
	launchRotate_ = Math::Quaternion::Slerp(launchRotate_,target,rate).Normalize();
}

Math::Vector3 BlockGroupLauncher::CalclateLaunchDirection(const Math::Vector3& rootPos) const{
	// ApplyLaunchAim() は進む向きの成分と横向きの成分で伸縮が違うため、
	// 経路上の向きをそのまま使わず、向け直した後の座標の差から求める
	const Math::Vector3 aheadPosition =
		ApplyLaunchAim(launchMover_.GetPointAtDistance(launchMover_.GetDistance() + kDirectionSampleDistance));

	const Math::Vector3 diff = aheadPosition - rootPos;
	if(diff.Length() <= kAimEpsilon){
		// 終点まで着いた後など、向きが求められない場合は今の向きを保つ
		return launchDirection_;
	}

	return diff.Normalize();
}

Math::Vector3 BlockGroupLauncher::GetAimGoal() const{
	// 座標系の原点ではなく塊の中心を狙い先へ運びたいので、中心のぶんだけ手前へずらす。
	// 塊は進む向きへ倒れていくため、中心の位置も同じ回転を掛けてから引く
	return targetPosition_ - launchRotate_ * launchCenterOffset_;
}

Math::Quaternion BlockGroupLauncher::MakeAimRotation(const Math::Vector3& baseDirection,const Math::Vector3& aimDirection){
	const float dot = std::clamp(Math::Vector3::Dot(baseDirection,aimDirection),-1.0f,1.0f);

	// ほぼ同じ向きなら回す必要は無い
	if(dot >= kAimParallelDot){
		return Math::Quaternion();
	}

	// 真逆を向いている場合は回転軸が決まらないため、垂直な軸を1つ選んで半回転させる
	if(dot <= -kAimParallelDot){
		Math::Vector3 axis = Math::Vector3::Cross(baseDirection,CVector3::UP);
		if(axis.Length() <= kAimEpsilon){
			axis = Math::Vector3::Cross(baseDirection,CVector3::RIGHT);
		}
		return Math::Quaternion::AngleAxis(kPI,axis.Normalize());
	}

	return Math::Quaternion::FromToRotation(baseDirection,aimDirection);
}

bool BlockGroupLauncher::NotifyBossHit(){
	// 既に当たっている塊は、同じフレームに別のブロックが当たっても二重に扱わない
	if(!IsActive() || isBossHit_){
		return false;
	}

	isBossHit_ = true;
	return true;
}

void BlockGroupLauncher::DestroyBlocks(){
	// ブロックの実体は StageBlockField が所有しているため、破棄も向こうに任せる
	if(pField_ != nullptr){
		for(const GatheringGroup& group : groups_){
			for(Block* block : group.blocks){
				pField_->DestroyDetachedBlock(block);
			}
		}
	}

	// 破棄したブロックを持ち続けないよう、噴射の停止も含めてここで制御を手放す
	Clear();
}

void BlockGroupLauncher::MoveGroup(const GatheringGroup& group,const Math::Vector3& basePoint,
								   const Math::Quaternion& rotate,float deltaTime) const{
	for(size_t index = 0; index < group.blocks.size(); ++index){
		Block* block = group.blocks[index];
		if(block == nullptr || !block->IsValid()){
			continue;
		}

		AOENGINE::WorldTransform* transform = block->GetTransform();
		if(transform == nullptr){
			continue;
		}

		// 目標位置との差から速度を求める。実際の位置を毎回見るのでズレが溜まらない
		const Math::Vector3 desired = basePoint + rotate * group.offsets[index];
		const Math::Vector3 diff = desired - transform->GetTranslate();

		MoveBlock(block,diff,deltaTime);

		// 打ち上げ中は塊ごと回すため、ブロック自身の向きも打ち上げた時の向きから回す。
		// 集合中は向きを覚えていないので、ここは素通りして今までの向きのままになる
		if(index < group.launchRotates.size()){
			transform->SetRotate(rotate * group.launchRotates[index]);
		}
	}
}

void BlockGroupLauncher::NotifyGatherStarted(const GatheringGroup& group){
	// 動き出したグループのコンボ表示はもう用済み
	FadeOutComboText(group.groupId);

	// 動き出した分だけ、ブロック数とコンボ(グループ数)を足していく
	gatheredBlockCount_ += static_cast<int>(group.blocks.size());
	++gatheredGroupCount_;

	// 足した結果を集合地点へ出す。既に出ていれば数が書き換わって跳ね直す
	ShowGatheredCount();

	// 全グループが動き出したら集合しきったとみなし、貯めた数から総ダメージへ切り替える
	if(gatheredGroupCount_ >= static_cast<int>(groups_.size())){
		ShowGatherDamage();
	}
}

void BlockGroupLauncher::ShowGatheredCount() const{
	if(pDamageTextUI_ == nullptr){
		return;
	}

	pDamageTextUI_->ShowGatheredCount(gatheredBlockCount_,gatheredGroupCount_,gatherPoint_);
}

void BlockGroupLauncher::FadeOutComboText(int groupId) const{
	if(pComboTextUI_ == nullptr){
		return;
	}

	// 表示が無いIDを渡しても向こうで弾かれる
	pComboTextUI_->StartFadeOut(groupId);
}

void BlockGroupLauncher::ShowGatherDamage(){
	// 1回の集合につき1度だけ。打ち上げに追い越された場合もここで弾く
	if(isDamageShown_){
		return;
	}
	isDamageShown_ = true;

	if(pDamageTextUI_ == nullptr || pDamageCalculator_ == nullptr){
		return;
	}

	// 実際にボスへ当たった時と同じ式で求める
	BlockDamageCalculator::HitContext hitContext{};
	hitContext.blockCount = gatheredBlockCount_;
	hitContext.groupCount = gatheredGroupCount_;

	pDamageTextUI_->ShowDamage(pDamageCalculator_->Calculate(hitContext),gatherPoint_);
}

void BlockGroupLauncher::ReleaseRemainingGroups(){
	// 打ち上げに追い越された場合は集合しきっていないため、総ダメージは出さない。
	// 出した印だけ先に立てて、コンボ表示の後始末だけを行う
	isDamageShown_ = true;

	for(GatheringGroup& group : groups_){
		if(group.isMoving){
			continue;
		}
		group.isMoving = true;
		NotifyGatherStarted(group);
	}
}

void BlockGroupLauncher::Clear(){
	// 演出オブジェクトは launchRoot_ を親にしているため、座標系が止まる前に演出を止める。
	// 既に出ている分(パーティクルなど)は消えるまでその場に残る。
	// オブジェクト本体と座標系は次の打ち上げで使い回すので、ここでは破棄しない(Destroy() は呼ばない)
	launchEffects_.Stop();
	// 使い回した時に前回の射出が残らないようにする。既に出た Particle は寿命まで残る
	shotUiEffects_.Stop();

	groups_.clear();
	state_ = State::Idle;
	// 使い回した時に前回の経路が残らないように空にしておく
	launchMover_.SetPoints({});
	launchTimer_ = 0.0f;
	arrivalTimer_ = 0.0f;
	// 狙い先そのものは外から毎フレーム渡されるため、ここでは追いかけている状態だけを解く
	isHoming_ = false;
	launchCenterOffset_ = CVector3::ZERO;
	launchDirection_ = CVector3::UP;
	launchRotate_ = Math::Quaternion();
	isBossHit_ = false;

	gatheredBlockCount_ = 0;
	gatheredGroupCount_ = 0;
	isDamageShown_ = false;
}

void BlockGroupLauncher::DrawConnectLine(const AOENGINE::Color& color,float thickness) const{
	if(state_ == State::Idle){
		return;
	}

	std::vector<Math::Vector3> points;
	points.reserve(groups_.size());
	for(const GatheringGroup& group : groups_){
		Math::Vector3 center{};
		if(!TryGetGroupCenter(group,center)){
			continue;
		}
		points.push_back(center);
	}

	if(points.size() < 2){
		return;
	}

	for(size_t index = 1; index < points.size(); ++index){
		AOENGINE::Render::DrawThickLine(points[index - 1],points[index],color,thickness);
	}
}

void BlockGroupLauncher::CalclateJetPos(Math::Vector3& outJetPos) const{
	constexpr float kJetOffsetY = -1.5f;	// 噴射パーティクルの出る位置をブロックの下にずらす
	if(state_ != State::Launched){
		return;
	}

	float minX = (std::numeric_limits<float>::max)();
	float maxX = (std::numeric_limits<float>::lowest)();

	float minY = (std::numeric_limits<float>::max)();

	float minZ = (std::numeric_limits<float>::max)();
	float maxZ = (std::numeric_limits<float>::lowest)();

	int validCount = 0;

	for(const auto& group : groups_){
		for(const auto& block : group.blocks){
			if(block == nullptr || !block->IsValid()){
				continue;
			}
			const auto position = GetBlockPosition(block);
			minX = (std::min)(minX,position.x);
			maxX = (std::max)(maxX,position.x);
			minY = (std::min)(minY,position.y);
			minZ = (std::min)(minZ,position.z);
			maxZ = (std::max)(maxZ,position.z);
			++validCount;
		}
	}

	// 1つも残っていない場合は求めようがないので、呼び出し元の値をそのままにしておく
	if(validCount == 0){
		return;
	}

	outJetPos.x = (minX + maxX) * 0.5f;
	outJetPos.y = minY + kJetOffsetY;
	outJetPos.z = (minZ + maxZ) * 0.5f;
}

Math::Vector3 BlockGroupLauncher::CalclateShotUiPos() const{
	// 画面中央をスクリーン座標として使う
	const Math::Vector2 screenPosition(
		static_cast<float>(AOENGINE::WinApp::sClientWidth) * 0.4f,
		static_cast<float>(AOENGINE::WinApp::sClientHeight) * 0.5f);

	// このゲームの盤面はXY平面なので、打ち上げの座標系の乗っているZへ合わせる
	const ScreenWorldPlaneAnchor::Params anchorParams{
		screenPosition,
		launchRoot_ != nullptr ? launchRoot_->GetTranslate().z : 0.0f
	};

	// 本来はゲーム用カメラ(FollowCamera)の行列を外から受け取って使う。
	// Render が持つ行列は最後に描画したカメラ(開発ビルドでは DebugCamera)のものになり、
	// 1フレーム遅れることもあるため、渡されていない場合のみ Render の行列で代用する
	const Math::Matrix4x4 viewProjection =
		hasScreenViewProjection_ ? screenViewProjection_ : AOENGINE::Render::GetViewProjectionMat();

	return shotUiScreenAnchor_.Solve(viewProjection,anchorParams);
}

Math::Vector3 BlockGroupLauncher::SamplePath(const std::vector<Math::Vector3>& path,float distance){
	if(path.empty()){
		return CVector3::ZERO;
	}
	if(distance <= 0.0f){
		return path.front();
	}

	float remain = distance;
	for(size_t index = 1; index < path.size(); ++index){
		const Math::Vector3 segment = path[index] - path[index - 1];
		const float segmentLength = segment.Length();
		if(segmentLength <= 0.0f){
			continue;
		}

		if(remain <= segmentLength){
			const float t = remain / segmentLength;
			return path[index - 1] + segment * t;
		}

		remain -= segmentLength;
	}

	return path.back();
}

float BlockGroupLauncher::ComputePathLength(const std::vector<Math::Vector3>& path){
	float length = 0.0f;
	for(size_t index = 1; index < path.size(); ++index){
		const Math::Vector3 segment = path[index] - path[index - 1];
		length += segment.Length();
	}
	return length;
}

float BlockGroupLauncher::ComputeContactDistance(const GatheringGroup& approaching,const GatheringGroup& waiting,
												 const Math::Vector3& approachDirection,float blockSize){
	// offsets はどちらも自分の接続地点から見た相対位置で、待っている側はその接続地点に居る。
	// 近づいてくる側はそこから approachDirection の逆向きに「残り距離」だけ離れた所に居るため、
	// ブロック2個の中心の差は (offsets の差) + approachDirection * 残り距離 になる。
	// これが箱の大きさに収まる残り距離の範囲を軸ごとに解き、
	// その中で一番遠い(=一番早く触れる)ものを塊全体の接触距離とする
	float contactDistance = 0.0f;

	for(size_t indexA = 0; indexA < approaching.blocks.size(); ++indexA){
		const Block* blockA = approaching.blocks[indexA];
		if(blockA == nullptr || !blockA->IsValid()){
			continue;
		}

		for(size_t indexB = 0; indexB < waiting.blocks.size(); ++indexB){
			const Block* blockB = waiting.blocks[indexB];
			if(blockB == nullptr || !blockB->IsValid()){
				continue;
			}

			const Math::Vector3 diff = waiting.offsets[indexB] - approaching.offsets[indexA];

			float nearDistance = 0.0f;
			float farDistance = (std::numeric_limits<float>::max)();
			if(!ClipContactRange(diff.x,approachDirection.x,blockSize,nearDistance,farDistance)){
				continue;
			}
			if(!ClipContactRange(diff.y,approachDirection.y,blockSize,nearDistance,farDistance)){
				continue;
			}
			if(!ClipContactRange(diff.z,approachDirection.z,blockSize,nearDistance,farDistance)){
				continue;
			}

			contactDistance = (std::max)(contactDistance,farDistance);
		}
	}

	return contactDistance;
}

Math::Vector3 BlockGroupLauncher::GetBlockPosition(const Block* block){
	if(block == nullptr){
		return CVector3::ZERO;
	}

	const AOENGINE::WorldTransform* transform = block->GetTransform();
	if(transform == nullptr){
		return CVector3::ZERO;
	}

	return transform->GetTranslate();
}

Math::Vector3 BlockGroupLauncher::MakeFallbackDirection(size_t index,size_t groupCount){
	if(groupCount == 0){
		return Math::Vector3(1.0f,0.0f,0.0f);
	}

	// 集合地点の周りへ均等に配る向き(XY平面)
	const float angle = kPI2 * static_cast<float>(index) / static_cast<float>(groupCount);
	return Math::Vector3(std::cos(angle),std::sin(angle),0.0f);
}

bool BlockGroupLauncher::TryGetGroupCenter(const GatheringGroup& group,Math::Vector3& outCenter){
	Math::Vector3 sum = CVector3::ZERO;
	int validCount = 0;

	for(const Block* block : group.blocks){
		if(block == nullptr || !block->IsValid()){
			continue;
		}
		sum = sum + GetBlockPosition(block);
		++validCount;
	}

	if(validCount == 0){
		return false;
	}

	outCenter = sum * (1.0f / static_cast<float>(validCount));
	return true;
}

void BlockGroupLauncher::MoveBlock(Block* block,const Math::Vector3& diff,float deltaTime){
	AOENGINE::BaseGameObject* gameObject = block->GetGameObject();
	if(gameObject == nullptr){
		return;
	}

	AOENGINE::WorldTransform* transform = block->GetTransform();

	// Rigidbody がある場合は速度で動かす(BaseGameObject 側で座標へ反映される)
	if(AOENGINE::Rigidbody* rigidbody = gameObject->GetRigidbody()){
		if(deltaTime > 0.0f){
			rigidbody->SetVelocity(diff * (1.0f / deltaTime));
		} else{
			rigidbody->SetVelocity(CVector3::ZERO);
		}
		return;
	}

	// Rigidbody が無い場合は座標を直接動かす
	if(transform != nullptr){
		transform->SetTranslate(transform->GetTranslate() + diff);
	}
}

int BlockGroupLauncher::GetBlockCount() const{
	int count = 0;
	for(const GatheringGroup& group : groups_){
		count += static_cast<int>(group.blocks.size());
	}
	return count;
}

bool BlockGroupLauncher::HasCollider(const AOENGINE::BaseCollider* collider) const{
	if(collider == nullptr){
		return false;
	}

	for(const GatheringGroup& group : groups_){
		for(Block* block : group.blocks){
			if(block == nullptr || !block->IsValid()){
				continue;
			}
			// Launch() で category を "Block" -> "LaunchedBlock" に付け替えているため、
			// 集合中は "Block"、打ち上げ中は "LaunchedBlock" のどちらでも引けるようにする
			if(block->GetCollider("LaunchedBlock") == collider || block->GetCollider("Block") == collider){
				return true;
			}
		}
	}

	return false;
}
