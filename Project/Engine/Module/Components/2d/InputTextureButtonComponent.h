#pragma once

#include <cstdint>
#include <string>

#include "Engine/System/Input/Input.h"

namespace AOENGINE {

class Sprite;

enum class ButtonInputType {
	Keyboard,
	Gamepad
};

/// <summary>
/// 指定した入力を押している間、所有SpriteのTextureを差し替えるコンポーネント。
/// </summary>
class InputTextureButtonComponent final {
public:
	void Update(Sprite& owner);
	void Reset(Sprite& owner);

	void SetEnabled(bool enabled) { enabled_ = enabled; }
	bool IsEnabled() const { return enabled_; }

	void SetInputType(ButtonInputType inputType) { inputType_ = inputType; }
	ButtonInputType GetInputType() const { return inputType_; }

	void SetKeyboardKey(uint8_t key) { keyboardKey_ = key; }
	uint8_t GetKeyboardKey() const { return keyboardKey_; }

	void SetGamepadButton(XInputButtons button) { gamepadButton_ = button; }
	XInputButtons GetGamepadButton() const { return gamepadButton_; }

	void SetPressedTexture(const std::string& texture) { pressedTexture_ = texture; }
	const std::string& GetPressedTexture() const { return pressedTexture_; }

private:
	bool enabled_ = true;
	ButtonInputType inputType_ = ButtonInputType::Keyboard;
	uint8_t keyboardKey_ = DIK_SPACE;
	XInputButtons gamepadButton_ = XInputButtons::ButtonA;
	std::string pressedTexture_;

	bool wasPressed_ = false;
	std::string restoreTexture_;
};

}
