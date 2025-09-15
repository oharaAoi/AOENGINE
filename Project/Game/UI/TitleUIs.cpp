#include "TitleUIs.h"
#include "Engine.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

void TitleUIs::Init() {
	SetName("TitleUIs");
	gameTitle_ = Engine::CreateSprite("title.png");
	gameTitle_->SetTranslate(Vector2(640.0f, 360.0f));
	Engine::GetCanvas2d()->AddSprite(gameTitle_.get());

	AddChild(gameTitle_.get());
	EditorWindows::AddObjectWindow(this, GetName());
}

void TitleUIs::Update() {
	
}

void TitleUIs::Draw() const {
	
}

void TitleUIs::Debug_Gui() {
}
