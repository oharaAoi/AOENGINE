#include "SceneTransition.h"

#include <Engine/Utilities/Logger.h>
#include <Engine/Lib/GameTimer.h>
#include <Engine/Core/Engine.h>
#include <Engine/Render/SceneRenderer.h>
#include <Engine/WinApp/WinApp.h>
#include <limits>

void SceneTransition::Init() {
	parameter_.Load();
	// シーン配置の"Fade"はシーンと同時に破棄されるため流用しない。
	Release();
	AOENGINE::Sprite* fade = Engine::GetCanvas2d()->AddSprite(
		"white.png", "__SceneTransitionFade", (std::numeric_limits<int>::max)());
	if (fade) {
		fadeHandle_ = fade->GetHandle();
		fade->SetScenePersistence(AOENGINE::ScenePersistence::RuntimeOnly);
		fade->ReSetTextureSize({
			static_cast<float>(AOENGINE::WinApp::sClientWidth),
			static_cast<float>(AOENGINE::WinApp::sClientHeight) });
		fade->SetTranslate({
			static_cast<float>(AOENGINE::WinApp::sClientWidth) * 0.5f,
			static_cast<float>(AOENGINE::WinApp::sClientHeight) * 0.5f });
		fade->SetColor(Colors::Linear::black);
		fade->EditorUpdate();
	}
	SetVisible();
}

void SceneTransition::Release() {
	if (GetFade()) {
		AOENGINE::SceneRenderer::GetInstance()->DestroyObject(fadeHandle_);
	}
	fadeHandle_ = {};
}

void SceneTransition::Update() {
	AOENGINE::Sprite* fade = GetFade();
	if (!fade) { return; }
	spriteAlphaTween_.Update(AOENGINE::GameTimer::FixedDeltaTime());
	if (!spriteAlphaTween_.GetIsFinish()) {
		AOENGINE::Color color = fade->GetColor();
		color.a = spriteAlphaTween_.GetValue();
		fade->SetColor(color);
	}
}

void SceneTransition::FadeIn() {
	AOENGINE::Sprite* fade = GetFade();
	if (!fade) {
		AOENGINE::Logger::AssertLog("Fadeが見つかりません");
		return;
	}
	fade->SetIsActive(true);
	spriteAlphaTween_.Init(0.0f, 1.0f, parameter_.transitionTime,
		static_cast<int>(EasingType::In::Sine), LoopType::Stop);
}

void SceneTransition::FadeOut() {
	AOENGINE::Sprite* fade = GetFade();
	if (!fade) {
		AOENGINE::Logger::AssertLog("Fadeが見つかりません");
		return;
	}
	fade->SetIsActive(true);
	spriteAlphaTween_.Init(1.0f, 0.0f, parameter_.transitionTime,
		static_cast<int>(EasingType::In::Sine), LoopType::Stop);
}

void SceneTransition::SetCovered() {
	AOENGINE::Sprite* fade = GetFade();
	if (!fade) { return; }
	AOENGINE::Color color = fade->GetColor();
	color.a = 1.0f;
	fade->SetColor(color);
	fade->SetIsActive(true);
	// シーン切替直後は通常更新を行わないため、ここで描画情報を確定する。
	fade->EditorUpdate();
}

void SceneTransition::SetVisible() {
	AOENGINE::Sprite* fade = GetFade();
	if (!fade) { return; }
	AOENGINE::Color color = fade->GetColor();
	color.a = 0.0f;
	fade->SetColor(color);
	fade->SetIsActive(false);
}

AOENGINE::Sprite* SceneTransition::GetFade() const {
	if (!fadeHandle_.IsValid()) { return nullptr; }
	return dynamic_cast<AOENGINE::Sprite*>(
		AOENGINE::SceneRenderer::GetInstance()->FindObject(fadeHandle_));
}

bool SceneTransition::IsReady() const {
	return GetFade() != nullptr;
}
