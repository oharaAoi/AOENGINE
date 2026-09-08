#include "ComboTextUIManager.h"

/// stl
#include <algorithm>

void ComboTextUIManager::Init(){
	parameter_.Load();
}

void ComboTextUIManager::Spawn(int groupId,int comboCount,const Math::Vector3& position){
	// groupId は表示とブロックグループを結ぶ手掛かりなので、重複させない。
	// 既に出ているグループへ二重に出すと、後から出した方を消せなくなる
	if(groupId < 0 || Find(groupId) != nullptr){
		return;
	}

	auto ui = std::make_unique<ComboTextUI>();
	if(!ui->Spawn(comboCount,position,parameter_)){
		return;
	}

	Entry entry{};
	entry.groupId = groupId;
	entry.ui = std::move(ui);
	entries_.push_back(std::move(entry));
}

void ComboTextUIManager::StartFadeOut(int groupId){
	ComboTextUI* ui = Find(groupId);
	if(ui == nullptr){
		return;
	}

	ui->StartFadeOut();
}

void ComboTextUIManager::StartFadeOutAll(){
	for(Entry& entry : entries_){
		if(entry.ui == nullptr){
			continue;
		}
		entry.ui->StartFadeOut();
	}
}

void ComboTextUIManager::Update(){
	for(Entry& entry : entries_){
		if(entry.ui == nullptr){
			continue;
		}
		entry.ui->Update();
	}

	// 消え終わったものはGameObjectごと破棄して、表からも外す
	entries_.erase(
		std::remove_if(entries_.begin(),entries_.end(),
					   [](Entry& entry){
						   if(entry.ui == nullptr){
							   return true;
						   }
						   if(!entry.ui->IsFinished()){
							   return false;
						   }
						   entry.ui->Destroy();
						   return true;
					   }),
		entries_.end());
}

void ComboTextUIManager::Clear(){
	for(Entry& entry : entries_){
		if(entry.ui == nullptr){
			continue;
		}
		entry.ui->Destroy();
	}

	entries_.clear();
}

ComboTextUI* ComboTextUIManager::Find(int groupId) const{
	for(const Entry& entry : entries_){
		if(entry.groupId == groupId){
			return entry.ui.get();
		}
	}

	return nullptr;
}
