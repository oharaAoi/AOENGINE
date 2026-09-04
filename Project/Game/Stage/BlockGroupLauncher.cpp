#include "BlockGroupLauncher.h"

/// game
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"

/// engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Render/Render.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MyMath.h"

/// stl
#include <algorithm>
#include <cmath>

using namespace AOENGINE;

namespace{
/// 1フレームで重なりを解き直す回数。3グループ以上が重なると押した先でまた重なるため数回繰り返す
constexpr int kSeparationIteration = 4;
/// 重なり判定の許容誤差。隣り合って接しているだけの状態を重なりとみなさないための下限
constexpr float kOverlapEpsilon = 1.0e-4f;
}

void BlockGroupLauncher::BeginGather(const GatherRequest& request,const Params& params){
	Clear();

	params_ = params;

	if(pField_ == nullptr){
		return;
	}

	for(size_t index = 0; index < request.targets.size(); ++index){
		GatheringGroup group{};
		if(!MakeGatheringGroup(request,index,group)){
			continue;
		}

		// 集めたグループはもうグリッド上の足場ではないので、連結グループの表から外す。
		// ブロックの実体はこの後もこのクラスが動かすため、Destroy はしない。
		pField_->RemoveGroup(request.targets[index].groupId);

		groups_.push_back(std::move(group));
	}

	if(groups_.empty()){
		return;
	}

	state_ = State::Gathering;
}

bool BlockGroupLauncher::MakeGatheringGroup(const GatherRequest& request,size_t targetIndex,GatheringGroup& outGroup) const{
	const Target& target = request.targets[targetIndex];

	const std::vector<Block*>* members = pField_->GetGroup(target.groupId);
	if(members == nullptr || members->empty()){
		return false;
	}

	// RemoveGroup() で members が無効になるため、ここで実体をコピーしておく(非所有ポインタの配列)
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

void BlockGroupLauncher::Launch(){
	if(state_ != State::Gathering){
		return;
	}

	state_ = State::Launched;
	launchVelocityY_ = params_.launchSpeed;
	launchTimer_ = params_.launchLifeTime;

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
}

void BlockGroupLauncher::Update(float deltaTime){
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
}

void BlockGroupLauncher::UpdateGathering(float deltaTime){
	// 各グループは自分が接続された地点から、後から接続したブロックを順に辿って集合地点へ向かう。
	// 経路の長さがグループごとに違うため、先に接続したグループほど遠くから追いかける形になり、
	// 結果として同じ道を数珠つなぎに進んで順番に集合する
	for(GatheringGroup& group : groups_){
		group.progress = (std::min)(group.progress + params_.gatherSpeed * deltaTime,group.pathLength);
		group.basePoint = SamplePath(group.path,group.progress);
	}

	// 経路上の位置のままだと集合地点で全グループが重なってしまうため、
	// 重なった分だけ互いに押し戻す。後から来たグループが先に居るグループを押し出すので、
	// 集まるほど塊が段々と大きくなる
	ResolveGroupSeparation(deltaTime);

	for(const GatheringGroup& group : groups_){
		MoveGroup(group,group.basePoint + group.separation,deltaTime);
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
	launchVelocityY_ += params_.launchAccel * deltaTime;

	const Math::Vector3 velocity{0.0f,launchVelocityY_,0.0f};
	for(const GatheringGroup& group : groups_){
		SetGroupVelocity(group,velocity,deltaTime);
	}

	launchTimer_ -= deltaTime;
	if(launchTimer_ > 0.0f){
		return;
	}

	// 画面外まで飛んで行った後は速度を残したまま制御を手放す
	Clear();
}

void BlockGroupLauncher::MoveGroup(const GatheringGroup& group,const Math::Vector3& pathPoint,float deltaTime) const{
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
		const Math::Vector3 desired = pathPoint + group.offsets[index];
		const Math::Vector3 diff = desired - transform->GetTranslate();

		MoveBlock(block,diff,deltaTime);
	}
}

void BlockGroupLauncher::SetGroupVelocity(const GatheringGroup& group,const Math::Vector3& velocity,float deltaTime) const{
	for(Block* block : group.blocks){
		if(block == nullptr || !block->IsValid()){
			continue;
		}

		AOENGINE::BaseGameObject* gameObject = block->GetGameObject();
		if(gameObject == nullptr){
			continue;
		}

		if(AOENGINE::Rigidbody* rigidbody = gameObject->GetRigidbody()){
			rigidbody->SetVelocity(velocity);
		} else if(AOENGINE::WorldTransform* transform = block->GetTransform()){
			transform->Translate(velocity,deltaTime);
		}
	}
}

void BlockGroupLauncher::Clear(){
	groups_.clear();
	state_ = State::Idle;
	launchVelocityY_ = 0.0f;
	launchTimer_ = 0.0f;
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
