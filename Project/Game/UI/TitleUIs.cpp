#include "TitleUIs.h"
#include "Engine.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

void TitleUIs::Init() {
	gameTitle_ = Engine::CreateSprite("title.png");
	gameTitle_->SetTranslate(Vector2(640.0f, 360.0f));
}

void TitleUIs::Update() {
	gameTitle_->Update();
}

void TitleUIs::Draw() const {
	Pipeline* pso = Engine::GetLastUsedPipeline();
	gameTitle_->Draw(pso);
}