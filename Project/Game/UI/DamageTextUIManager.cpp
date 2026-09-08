#include "DamageTextUIManager.h"

/// stl
#include <algorithm>

void DamageTextUIManager::Init(){
	parameter_.Load();
}

void DamageTextUIManager::ShowGatheredCount(int blockCount,int comboCount,const Math::Vector3& position){
	// 数が増えただけなので、その場で跳ね直すだけにする
	Show(DamageTextUI::MakeCountText(blockCount,comboCount,parameter_),position,false);
}

void DamageTextUIManager::ShowDamage(float damage,const Math::Vector3& position){
	// 集めた数から総ダメージへ変わる区切りなので、出るところからやり直す
	Show(DamageTextUI::MakeDamageText(damage,parameter_),position,true);
}

void DamageTextUIManager::Show(const std::string& text,const Math::Vector3& position,bool isReplay){
	// 出ている最中のものがあれば、作り直さずに文字だけ差し替える
	if(current_ != nullptr){
		current_->SetText(text);

		if(isReplay){
			current_->Replay();
		} else{
			current_->Bounce();
		}
		return;
	}

	auto ui = std::make_unique<DamageTextUI>();
	if(!ui->Spawn(text,position,parameter_)){
		return;
	}

	current_ = ui.get();
	entries_.push_back(std::move(ui));
}

void DamageTextUIManager::StartFadeOutAll(){
	for(std::unique_ptr<DamageTextUI>& ui : entries_){
		if(ui == nullptr){
			continue;
		}
		ui->StartFadeOut();
	}

	// 消し始めたものはもう書き換えない
	current_ = nullptr;
}

void DamageTextUIManager::Update(){
	for(std::unique_ptr<DamageTextUI>& ui : entries_){
		if(ui == nullptr){
			continue;
		}
		ui->Update();
	}

	// 消え終わったものはGameObjectごと破棄して、表からも外す
	entries_.erase(
		std::remove_if(entries_.begin(),entries_.end(),
					   [this](std::unique_ptr<DamageTextUI>& ui){
						   if(ui == nullptr){
							   return true;
						   }
						   if(!ui->IsFinished()){
							   return false;
						   }
						   // 消えた実体を指したままにしない
						   if(current_ == ui.get()){
							   current_ = nullptr;
						   }
						   ui->Destroy();
						   return true;
					   }),
		entries_.end());
}

void DamageTextUIManager::Clear(){
	for(std::unique_ptr<DamageTextUI>& ui : entries_){
		if(ui == nullptr){
			continue;
		}
		ui->Destroy();
	}

	entries_.clear();
	current_ = nullptr;
}
