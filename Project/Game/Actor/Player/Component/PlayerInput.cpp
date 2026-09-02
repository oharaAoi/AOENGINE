#include "PlayerInput.h"

#include <algorithm>

#include "Engine/System/Input/Input.h"

using namespace AOENGINE;

void PlayerInput::Update(float stickDeadZone) {
	// 左右移動 (A/D/←/→ ・ 左スティック左右)
	float horizontal = 0.0f;
	if (Input::IsPressKey(DIK_D) || Input::IsPressKey(DIK_RIGHT)) {
		horizontal += 1.0f;
	}
	if (Input::IsPressKey(DIK_A) || Input::IsPressKey(DIK_LEFT)) {
		horizontal -= 1.0f;
	}
	horizontal += Input::GetLeftJoyStick(stickDeadZone).x;
	horizontal_ = std::clamp(horizontal, -1.0f, 1.0f);

	// ジャンプ (Space ・ パッドA)
	jumpTriggered_ = Input::IsTriggerKey(DIK_SPACE) || Input::IsTriggerButton(ButtonA);

	// 集めたブロックの打ち上げ (E ・ パッドB)
	launchTriggered_ = Input::IsTriggerKey(DIK_E) || Input::IsTriggerButton(ButtonB);
}
