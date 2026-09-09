#include "BlockGroupLauncherManager.h"

void BlockGroupLauncherManager::BeginGather(const BlockGroupLauncher::GatherRequest& request,const BlockGroupLauncher::Params& params){
	// Idle なランチャーを探して使い回す。打ち上げ中や集合中のものを流用すると
	// BlockGroupLauncher::BeginGather() 冒頭の Clear() で今動いている集合・打ち上げが切れてしまうため、
	// 必ず空いているものだけを選ぶ
	BlockGroupLauncher* target = nullptr;
	for(BlockGroupLauncher& launcher : launchers_){
		if(launcher.GetState() == BlockGroupLauncher::State::Idle){
			target = &launcher;
			break;
		}
	}

	if(target == nullptr){
		// 空きが無ければ増やす。同時に動く数は launchLifeTime/(connectableTime+launchWaitTime) 程度で
		// 自然に頭打ちになるため、ここで erase して数を絞る必要はない
		launchers_.emplace_back();
		target = &launchers_.back();
	}

	target->SetField(pField_);
	// 増やしたばかりのランチャーにも狙い先を渡しておく
	if(hasTarget_){
		target->SetTarget(targetPosition_);
	}
	// 増やしたばかりのランチャーにも画面基準の演出位置用の viewProjection を渡しておく
	if(hasScreenViewProjection_){
		target->SetScreenViewProjection(screenViewProjection_);
	}
	target->SetComboTextUIManager(pComboTextUI_);
	target->SetDamageTextUIManager(pDamageTextUI_);
	target->SetDamageCalculator(pDamageCalculator_);

	// BeginGather() の中で集合しきる場合(1グループだけの時)があるため、
	// 総ダメージを出せるようにしてから始める
	target->BeginGather(request,params);
}

void BlockGroupLauncherManager::Launch(){
	// BlockGroupLauncher::Launch() は Gathering 以外を自分で弾くので、全ランチャーへそのまま伝える
	for(BlockGroupLauncher& launcher : launchers_){
		launcher.Launch();
	}
}

void BlockGroupLauncherManager::SetTarget(const Math::Vector3& position){
	targetPosition_ = position;
	hasTarget_ = true;

	for(BlockGroupLauncher& launcher : launchers_){
		launcher.SetTarget(position);
	}
}

void BlockGroupLauncherManager::ClearTarget(){
	hasTarget_ = false;

	for(BlockGroupLauncher& launcher : launchers_){
		launcher.ClearTarget();
	}
}

void BlockGroupLauncherManager::SetScreenViewProjection(const Math::Matrix4x4& viewProjection){
	screenViewProjection_ = viewProjection;
	hasScreenViewProjection_ = true;

	for(BlockGroupLauncher& launcher : launchers_){
		launcher.SetScreenViewProjection(viewProjection);
	}
}

void BlockGroupLauncherManager::Update(float deltaTime){
	for(BlockGroupLauncher& launcher : launchers_){
		launcher.Update(deltaTime);
	}
}

void BlockGroupLauncherManager::Clear(){
	for(BlockGroupLauncher& launcher : launchers_){
		launcher.Clear();
	}
}

bool BlockGroupLauncherManager::NotifyBossHit(const AOENGINE::BaseCollider* collider){
	for(BlockGroupLauncher& launcher : launchers_){
		if(!launcher.HasCollider(collider)){
			continue;
		}
		return launcher.NotifyBossHit();
	}
	return false;
}

void BlockGroupLauncherManager::DrawConnectLine(const AOENGINE::Color& color,float thickness) const{
	for(const BlockGroupLauncher& launcher : launchers_){
		if(!launcher.IsActive()){
			continue;
		}
		launcher.DrawConnectLine(color,thickness);
	}
}

void BlockGroupLauncherManager::SetField(StageBlockField* field){
	pField_ = field;

	// 既に持っているランチャーにも伝えておかないと、使い回した時に古い field を参照したままになる
	for(BlockGroupLauncher& launcher : launchers_){
		launcher.SetField(field);
	}
}

void BlockGroupLauncherManager::SetComboTextUIManager(ComboTextUIManager* manager){
	pComboTextUI_ = manager;

	for(BlockGroupLauncher& launcher : launchers_){
		launcher.SetComboTextUIManager(manager);
	}
}

void BlockGroupLauncherManager::SetDamageTextUIManager(DamageTextUIManager* manager){
	pDamageTextUI_ = manager;

	for(BlockGroupLauncher& launcher : launchers_){
		launcher.SetDamageTextUIManager(manager);
	}
}

void BlockGroupLauncherManager::SetDamageCalculator(const BlockDamageCalculator* calculator){
	pDamageCalculator_ = calculator;

	for(BlockGroupLauncher& launcher : launchers_){
		launcher.SetDamageCalculator(calculator);
	}
}

bool BlockGroupLauncherManager::IsActive() const{
	for(const BlockGroupLauncher& launcher : launchers_){
		if(launcher.IsActive()){
			return true;
		}
	}
	return false;
}

int BlockGroupLauncherManager::GetActiveCount() const{
	int count = 0;
	for(const BlockGroupLauncher& launcher : launchers_){
		if(launcher.IsActive()){
			++count;
		}
	}
	return count;
}

int BlockGroupLauncherManager::GetGroupCount() const{
	int count = 0;
	for(const BlockGroupLauncher& launcher : launchers_){
		count += launcher.GetGroupCount();
	}
	return count;
}

int BlockGroupLauncherManager::GetBlockCount() const{
	int count = 0;
	for(const BlockGroupLauncher& launcher : launchers_){
		count += launcher.GetBlockCount();
	}
	return count;
}

int BlockGroupLauncherManager::GetBlockCountByCollider(const AOENGINE::BaseCollider* collider) const{
	for(const BlockGroupLauncher& launcher : launchers_){
		if(launcher.HasCollider(collider)){
			return launcher.GetBlockCount();
		}
	}
	return 0;
}
