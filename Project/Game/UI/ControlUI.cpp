#include "ControlUI.h"

// engine
#include <Engine/Utilities/SceneObjectFinder.h>
#include <Engine/System/Input/Input.h>

void ControlUI::Init() {
	keyboardUI_ = FindSceneObject<AOENGINE::Sprite>("KeyboardUI");
	gamePadUI_ = FindSceneObject<AOENGINE::Sprite>("GamePadUI");
	gamePadUI_->SetActive(false);
}

void ControlUI::Update() {
	AOENGINE::Input* input = AOENGINE::Input::GetInstance();
	if (input->GetInputDevice() == InputDevice::Gamepad) {
		gamePadUI_->SetActive(true);
		keyboardUI_->SetActive(false);
	} else {
		gamePadUI_->SetActive(false);
		keyboardUI_->SetActive(true);
	}
}
