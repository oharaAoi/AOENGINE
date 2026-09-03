#include "BlockGroupLauncher.h"

/// game
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"

/// engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/WorldTransform.h"

/// stl
#include <algorithm>

using namespace AOENGINE;

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

		const Math::Vector3 pathPoint = SamplePath(group.path,group.progress);
		MoveGroup(group,pathPoint,deltaTime);
	}
}

void BlockGroupLauncher::UpdateLaunched(float deltaTime){
	launchVelocityY_ += params_.launchAccel * deltaTime;

	const Math::Vector3 velocity{ 0.0f,launchVelocityY_,0.0f };
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
