#include "PopupTextUI.h"

/// stl
#include <algorithm>

/// engine
#include "Engine/Module/Components/3d/WorldTextComponent.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Lib/GameTimer.h"

/// math
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/Easing.h"

using namespace AOENGINE;

namespace {
	/// 消えている時の大きさ。ここは調整対象ではないので定数のまま持つ
	constexpr float kHiddenSize = 0.f;
}

bool PopupTextUI::SpawnText(const std::string& prefabName,const std::string& text,
							const Math::Vector3& position,const PopupTextParams& params){
	// 1つのUIは1回しか使わないため、既に実体を持っている場合は作り直さない
	if(IsValid()){
		return false;
	}

	// 調整値は管理側が持ち続けるものを指す。実行中に編集した値がそのまま効く
	pParams_ = &params;

	BaseGameObject* thisGameObject = this->InstantiatePrefab(prefabName);
	if(thisGameObject == nullptr){
		return false;
	}

	// InstantiatePrefab() は生成するだけで関連付けまではしないため、ここで結び付ける。
	// これをしないと GetGameObject() が null のままになり、以降の更新が何も効かなくなる
	this->Bind(thisGameObject);

	auto textComp = thisGameObject->GetComponent<AOENGINE::WorldTextComponent>();
	if(textComp == nullptr){
		// 文字を出せないものを掴んだままにしないよう、作ったものはここで消しておく
		this->Destroy();
		return false;
	}

	timer_ = 0.f;
	textSize_ = kHiddenSize;

	bounceCount_ = 0;

	basePosition_ = position;
	currentY_ = position.y;
	baseY_ = position.y;

	velocity_ = pParams_->firstVelocity;

	isFadingOut_ = false;
	isFinished_ = false;

	textComp->SetText(text);
	textComp->SetHeight(textSize_);

	if(WorldTransform* transform = thisGameObject->GetTransform()){
		transform->SetTranslate(basePosition_);
	}

	fadeCallback_ = [this](){
		this->FadeIn();
		};

	return true;
}

void PopupTextUI::Update(){
	if(isFinished_ || pParams_ == nullptr){
		return;
	}

	auto thisGameObject = this->GetGameObject();

	if(thisGameObject == nullptr){
		// 何らかの理由で実体が先に消えている場合は、管理側に回収させる
		isFinished_ = true;
		return;
	}

	auto textComp = thisGameObject->GetComponent<AOENGINE::WorldTextComponent>();
	if(textComp == nullptr){
		isFinished_ = true;
		return;
	}

	// 出る・消えるのどちらを進めるかは SpawnText() / StartFadeOut() で差し替えている
	if(fadeCallback_){
		fadeCallback_();
	}

	UpdateBounce();

	// X/Zは生成された位置のまま。跳ねる分だけYを動かす
	thisGameObject->GetTransform()->SetTranslate(Math::Vector3(basePosition_.x,currentY_,basePosition_.z));
	textComp->SetHeight(textSize_);
}

void PopupTextUI::SetText(const std::string& text){
	BaseGameObject* thisGameObject = this->GetGameObject();
	if(thisGameObject == nullptr){
		return;
	}

	if(auto textComp = thisGameObject->GetComponent<AOENGINE::WorldTextComponent>()){
		textComp->SetText(text);
	}
}

void PopupTextUI::Bounce(){
	if(pParams_ == nullptr || isFinished_){
		return;
	}

	// 跳ね終わっていても跳ね直せるように、回数と位置ごと戻す
	bounceCount_ = 0;
	currentY_ = baseY_;
	velocity_ = pParams_->firstVelocity;
}

void PopupTextUI::Replay(){
	if(pParams_ == nullptr || isFinished_){
		return;
	}

	// 消えかけていた場合もここで出る側へ戻す
	timer_ = 0.f;
	isFadingOut_ = false;

	fadeCallback_ = [this](){
		this->FadeIn();
		};

	Bounce();
}

void PopupTextUI::StartFadeOut(){
	if(isFadingOut_ || isFinished_ || pParams_ == nullptr){
		return;
	}

	isFadingOut_ = true;

	// 出きる前に呼ばれた場合、timer_ は出る側の残り時間になっている。
	// 消える側の時間へ割合で置き換えて、今出ている大きさから続けて消えるようにする
	const float rate = (pParams_->fadeInTime > 0.f) ? (timer_ / pParams_->fadeInTime) : 1.f;
	timer_ = pParams_->fadeOutTime * (std::min)(rate,1.f);

	fadeCallback_ = [this](){
		this->FadeOut();
		};
}

void PopupTextUI::FadeIn(){

	const float fadeInTime = pParams_->fadeInTime;

	timer_ += GameTimer::DeltaTime();
	if(timer_ > fadeInTime){
		timer_ = fadeInTime;
	}

	// 時間を0にされた場合は一瞬で出しきる
	float t = (fadeInTime > 0.f) ? (timer_ / fadeInTime) : 1.f;
	textSize_ = Lerp(kHiddenSize,pParams_->visibleSize,Ease::Out::Quad(t));

}

void PopupTextUI::FadeOut(){

	const float fadeOutTime = pParams_->fadeOutTime;

	timer_ -= GameTimer::DeltaTime();
	if(timer_ < 0.f){
		timer_ = 0.f;
	}

	// 時間を0にされた場合は一瞬で消しきる
	float t = (fadeOutTime > 0.f) ? (timer_ / fadeOutTime) : 0.f;
	textSize_ = Lerp(kHiddenSize,pParams_->visibleSize,Ease::In::Quad(t));

	// 完全に消えたら管理側に破棄させる
	if(timer_ <= 0.f){
		isFinished_ = true;
	}
}

void PopupTextUI::UpdateBounce(){

	// 目標以上なら更新しない
	if(bounceCount_ >= pParams_->targetBounceCount){
		currentY_ = baseY_;
		return;
	}

	const float deltaTime = GameTimer::DeltaTime();

	velocity_ += kGravity * pParams_->gravityRate * deltaTime;
	currentY_ += velocity_ * deltaTime;

	// Bounce。生成位置より下がった所で跳ね返す
	if(currentY_ < baseY_){
		currentY_ = baseY_;
		velocity_ = -velocity_ * pParams_->damping;
		bounceCount_++;
	}
}
