#include "SceneTransition.h"

// engine
#include <Engine/Utilities/SceneObjectFinder.h>
#include <Engine/Utilities/Logger.h>
#include <Engine/Lib/GameTimer.h>

void SceneTransition::Init() {
	parameter_.Load();

	fade_ = FindSceneObject<AOENGINE::Sprite>("Fade");
	if (!fade_) {
		AOENGINE::Logger::AssertLog("Fadeが見つかりません");
		return;
	}
	fade_->SetIsActive(false);
}

void SceneTransition::Update() {
	spriteAlphaTween_.Update(AOENGINE::GameTimer::DeltaTime());
	if (!spriteAlphaTween_.GetIsFinish()) {
		AOENGINE::Color color = fade_->GetColor();
		color.a = spriteAlphaTween_.GetValue();
		fade_->SetColor(color);
	}
}

void SceneTransition::FadeIn() {
	if (!fade_) {
		AOENGINE::Logger::AssertLog("Fadeが見つかりません");
		return;
	}

	fade_->SetIsActive(true);
	// アルファ値の設定
	spriteAlphaTween_.Init(0.0f, 1.0f, parameter_.transitionTime, static_cast<int>(EasingType::In::Sine), LoopType::Stop);
}


void SceneTransition::FadeOut() {
	if (!fade_) {
		AOENGINE::Logger::AssertLog("Fadeが見つかりません");
		return;
	}

	fade_->SetIsActive(true);
	// アルファ値の設定
	spriteAlphaTween_.Init(1.0f, 0.0f, parameter_.transitionTime, static_cast<int>(EasingType::In::Sine), LoopType::Stop);
}
