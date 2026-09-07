#include "InputTextureButtonComponent.h"

#include "Engine/Module/Components/2d/Sprite.h"

using namespace AOENGINE;

void InputTextureButtonComponent::Update(Sprite& owner) {
	if (!enabled_ || pressedTexture_.empty()) {
		Reset(owner);
		return;
	}

	const bool pressed = inputType_ == ButtonInputType::Keyboard
		? Input::IsPressKey(keyboardKey_)
		: (Input::IsTriggerButton(gamepadButton_) || Input::IsPressButton(gamepadButton_));

	if (pressed) {
		if (!wasPressed_) {
			restoreTexture_ = owner.GetTextureName();
			wasPressed_ = true;
		}
		if (owner.GetTextureName() != pressedTexture_) {
			owner.ReSetTexture(pressedTexture_);
		}
		return;
	}

	Reset(owner);
}

void InputTextureButtonComponent::Reset(Sprite& owner) {
	if (!wasPressed_) {
		return;
	}

	// 押下中に別処理がTextureを変更した場合は、その変更を上書きしない。
	if (!restoreTexture_.empty() && owner.GetTextureName() == pressedTexture_) {
		owner.ReSetTexture(restoreTexture_);
	}

	restoreTexture_.clear();
	wasPressed_ = false;
}
