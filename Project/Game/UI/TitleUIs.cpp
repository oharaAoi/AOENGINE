#include "TitleUIs.h"
#include "Engine.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

void TitleUIs::Init() {
	SetName("TitleUIs");
	gameTitle_ = Engine::GetCanvas2d()->AddSprite("title.png", "title");
	gameTitle_->SetTranslate(Vector2(640.0f, 360.0f));
	
	AddChild(gameTitle_);
	EditorWindows::AddObjectWindow(this, GetName());
}

void TitleUIs::Update() {
	
}

void TitleUIs::Draw() const {
	
}

void TitleUIs::Debug_Gui() {
}
