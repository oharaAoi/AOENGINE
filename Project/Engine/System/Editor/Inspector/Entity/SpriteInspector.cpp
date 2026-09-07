#include "SpriteInspector.h"

#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Module/Components/2d/InputTextureButtonComponent.h"

#include <string>

using namespace AOENGINE;

namespace {

const char* kBlendItems[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
const char* kFillMethodItems[] = { "Vertical", "Horizontal", "Radial", "BothEnds" };

struct KeyboardKeyItem {
	const char* name;
	uint8_t value;
};

const KeyboardKeyItem kKeyboardKeys[] = {
	{ "Space", DIK_SPACE }, { "Enter", DIK_RETURN }, { "Escape", DIK_ESCAPE }, { "Tab", DIK_TAB },
	{ "Up", DIK_UP }, { "Down", DIK_DOWN }, { "Left", DIK_LEFT }, { "Right", DIK_RIGHT },
	{ "Left Shift", DIK_LSHIFT }, { "Right Shift", DIK_RSHIFT },
	{ "Left Control", DIK_LCONTROL }, { "Right Control", DIK_RCONTROL },
	{ "Left Alt", DIK_LALT }, { "Right Alt", DIK_RALT },
	{ "A", DIK_A }, { "B", DIK_B }, { "C", DIK_C }, { "D", DIK_D }, { "E", DIK_E },
	{ "F", DIK_F }, { "G", DIK_G }, { "H", DIK_H }, { "I", DIK_I }, { "J", DIK_J },
	{ "K", DIK_K }, { "L", DIK_L }, { "M", DIK_M }, { "N", DIK_N }, { "O", DIK_O },
	{ "P", DIK_P }, { "Q", DIK_Q }, { "R", DIK_R }, { "S", DIK_S }, { "T", DIK_T },
	{ "U", DIK_U }, { "V", DIK_V }, { "W", DIK_W }, { "X", DIK_X }, { "Y", DIK_Y }, { "Z", DIK_Z },
	{ "0", DIK_0 }, { "1", DIK_1 }, { "2", DIK_2 }, { "3", DIK_3 }, { "4", DIK_4 },
	{ "5", DIK_5 }, { "6", DIK_6 }, { "7", DIK_7 }, { "8", DIK_8 }, { "9", DIK_9 },
	{ "F1", DIK_F1 }, { "F2", DIK_F2 }, { "F3", DIK_F3 }, { "F4", DIK_F4 },
	{ "F5", DIK_F5 }, { "F6", DIK_F6 }, { "F7", DIK_F7 }, { "F8", DIK_F8 },
	{ "F9", DIK_F9 }, { "F10", DIK_F10 }, { "F11", DIK_F11 }, { "F12", DIK_F12 }
};

struct GamepadButtonItem {
	const char* name;
	XInputButtons value;
};

const GamepadButtonItem kGamepadButtons[] = {
	{ "D-pad Up", DpadUp }, { "D-pad Down", DpadDown },
	{ "D-pad Left", DpadLeft }, { "D-pad Right", DpadRight },
	{ "Start", Start }, { "Back", Back },
	{ "Left Stick", LStickThumb }, { "Right Stick", RStickThumb },
	{ "Left Shoulder", LShoulder }, { "Right Shoulder", RShoulder },
	{ "A", ButtonA }, { "B", ButtonB }, { "X", ButtonX }, { "Y", ButtonY }
};

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Sprite全体のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpriteInspector::Draw(Sprite& sprite) {
	bool isBackground = sprite.GetIsBackGround();
	if (ImGui::Checkbox("Background (Behind 3D)", &isBackground)) {
		sprite.SetIsBackGround(isBackground);
	}

	int renderQueue = sprite.GetRenderQueue();
	if (ImGui::DragInt("renderQueue", &renderQueue, 1)) {
		sprite.SetRenderQueue(renderQueue);
	}

	int currentBlendMode = static_cast<int>(sprite.GetBlendMode());
	if (ImGui::Combo("BlendMode", &currentBlendMode, kBlendItems, IM_ARRAYSIZE(kBlendItems))) {
		sprite.SetBlendMode(static_cast<Blend::BlendMode>(currentBlendMode));
	}

	if (ImGui::Button("ResetSize")) {
		sprite.ResetTextureSize();
	}

	if (ScreenTransform* transform = sprite.GetTransform()) {
		DrawTransform(*transform);
	}

	DrawUv(sprite);

	Math::Vector2 anchorPoint = sprite.GetAnchorPoint();
	if (ImGui::DragFloat2("anchorPoint", &anchorPoint.x, 0.01f, 0.0f, 1.0f)) {
		sprite.SetAnchorPoint(anchorPoint);
	}

	Math::Vector2 spriteSize = sprite.GetSpriteSize();
	if (ImGui::DragFloat2("textureSize", &spriteSize.x, 1.0f)) {
		sprite.ReSetTextureSize(spriteSize);
	}

	Math::Vector2 drawRange = sprite.GetDrawRange();
	if (ImGui::DragFloat2("drawRange", &drawRange.x, 1.0f)) {
		sprite.SetDrawRange(drawRange);
	}

	Math::Vector2 leftTop = sprite.GetLeftTop();
	if (ImGui::DragFloat2("leftTop", &leftTop.x, 1.0f)) {
		sprite.SetLeftTop(leftTop);
	}

	Math::Vector2 uvMin = sprite.GetUvMinSize();
	if (ImGui::DragFloat2("uvMin", &uvMin.x, 0.01f)) {
		sprite.SetUvMinSize(uvMin);
	}

	Math::Vector2 uvMax = sprite.GetUvMaxSize();
	if (ImGui::DragFloat2("uvMax", &uvMax.x, 0.01f)) {
		sprite.SetUvMaxSize(uvMax);
	}

	AOENGINE::Color color = sprite.GetColor();
	if (ImGui::ColorEdit4("color", &color.r)) {
		sprite.SetColor(color);
	}

	DrawTexture(sprite);
	DrawFill(sprite);
	DrawButton(sprite);
	DrawSaveLoad(sprite);
}

void SpriteInspector::DrawButton(Sprite& sprite) {
	if (!sprite.HasButtonComponent()) {
		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("SpriteAddComponentPopup");
		}
		if (ImGui::BeginPopup("SpriteAddComponentPopup")) {
			if (ImGui::MenuItem("Button")) {
				sprite.AddButtonComponent();
			}
			ImGui::EndPopup();
		}
		return;
	}

	InputTextureButtonComponent* button = sprite.GetButtonComponent();
	if (!button) { return; }

	bool remove = false;
	const bool open = ImGui::CollapsingHeader("Button");
	if (ImGui::BeginPopupContextItem("ButtonComponentContext")) {
		remove = ImGui::MenuItem("Remove Component");
		ImGui::EndPopup();
	}

	if (open) {
		bool enabled = button->IsEnabled();
		if (ImGui::Checkbox("Enabled##Button", &enabled)) {
			button->Reset(sprite);
			button->SetEnabled(enabled);
		}

		const char* inputTypes[] = { "Keyboard", "Gamepad" };
		int inputType = static_cast<int>(button->GetInputType());
		if (ImGui::Combo("Input Type", &inputType, inputTypes, IM_ARRAYSIZE(inputTypes))) {
			button->Reset(sprite);
			button->SetInputType(static_cast<ButtonInputType>(inputType));
		}

		if (button->GetInputType() == ButtonInputType::Keyboard) {
			int selected = 0;
			for (int i = 0; i < IM_ARRAYSIZE(kKeyboardKeys); ++i) {
				if (kKeyboardKeys[i].value == button->GetKeyboardKey()) { selected = i; break; }
			}
			if (ImGui::BeginCombo("Key", kKeyboardKeys[selected].name)) {
				for (int i = 0; i < IM_ARRAYSIZE(kKeyboardKeys); ++i) {
					if (ImGui::Selectable(kKeyboardKeys[i].name, i == selected)) {
						button->Reset(sprite);
						button->SetKeyboardKey(kKeyboardKeys[i].value);
					}
				}
				ImGui::EndCombo();
			}
		} else {
			int selected = 0;
			for (int i = 0; i < IM_ARRAYSIZE(kGamepadButtons); ++i) {
				if (kGamepadButtons[i].value == button->GetGamepadButton()) { selected = i; break; }
			}
			if (ImGui::BeginCombo("Gamepad Button", kGamepadButtons[selected].name)) {
				for (int i = 0; i < IM_ARRAYSIZE(kGamepadButtons); ++i) {
					if (ImGui::Selectable(kGamepadButtons[i].name, i == selected)) {
						button->Reset(sprite);
						button->SetGamepadButton(kGamepadButtons[i].value);
					}
				}
				ImGui::EndCombo();
			}
		}

		const std::string& pressedTexture = button->GetPressedTexture();
		if (!pressedTexture.empty()) {
			const auto handle = TextureManager::GetInstance()->GetDxHeapHandles(pressedTexture).handleGPU;
			ImGui::Image(reinterpret_cast<ImTextureID>(handle.ptr), ImVec2(64, 64));
		} else {
			ImGui::Button("Drop Texture Here", ImVec2(128, 64));
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_HANDLE")) {
				const AssetHandle& asset = *static_cast<const AssetHandle*>(payload->Data);
				if (asset.type == AssetType::Texture) {
					const std::string texture = TextureManager::GetInstance()->SearchSprite(asset.id);
					if (!texture.empty()) {
						button->Reset(sprite);
						button->SetPressedTexture(texture);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		ImGui::TextUnformatted("Pressed Texture");
	}

	if (remove) {
		sprite.RemoveButtonComponent();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 2D TransformのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpriteInspector::DrawTransform(ScreenTransform& transform) {
	if (!ImGui::TreeNode("transform")) {
		return;
	}

	Math::SRT srt = transform.GetTransform();
	bool changed = false;
	changed |= ImGui::DragFloat3("translation", &srt.translate.x, 0.1f);
	changed |= ImGui::DragFloat2("scale", &srt.scale.x, 0.01f);
	changed |= ImGui::SliderAngle("rotation", &srt.rotate.z);
	if (changed) {
		transform.SetTransform(srt);
	}

	ImGui::TreePop();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// UV TransformのInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpriteInspector::DrawUv(Sprite& sprite) {
	if (!ImGui::TreeNode("uv")) {
		return;
	}

	Math::SRT uvTransform = sprite.GetUvTransform();
	bool changed = false;
	changed |= ImGui::DragFloat2("uvTranslation", &uvTransform.translate.x, 0.01f);
	changed |= ImGui::DragFloat2("uvScale", &uvTransform.scale.x, 0.01f);
	changed |= ImGui::SliderAngle("uvRotation", &uvTransform.rotate.z);
	if (changed) {
		sprite.SetUvTransform(uvTransform);
	}

	ImGui::TreePop();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// TextureプレビューとDragDropを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpriteInspector::DrawTexture(Sprite& sprite) {
	D3D12_GPU_DESCRIPTOR_HANDLE texHandle = AOENGINE::TextureManager::GetInstance()->GetDxHeapHandles(sprite.GetTextureName()).handleGPU;
	ImTextureID texId = reinterpret_cast<ImTextureID>(texHandle.ptr);
	ImGui::Image(texId, ImVec2(64, 64));

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_HANDLE")) {
			const AssetHandle& handle = *static_cast<const AssetHandle*>(payload->Data);
			if (handle.type == AssetType::Texture) {
				const std::string dropTexture = TextureManager::GetInstance()->SearchSprite(handle.id);
				if (!dropTexture.empty()) {
					sprite.ReSetTexture(dropTexture);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Fill設定のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpriteInspector::DrawFill(Sprite& sprite) {
	int currentFillMethod = static_cast<int>(sprite.GetFillMethod());
	if (ImGui::Combo("Fill Method", &currentFillMethod, kFillMethodItems, IM_ARRAYSIZE(kFillMethodItems))) {
		sprite.SetFillMethod(static_cast<FillMethod>(currentFillMethod));
	}

	switch (sprite.GetFillMethod()) {
	case FillMethod::Vertical:
		{
			const char* pointItems[] = { "Top", "Bottom" };
			int currentPoint = sprite.GetFillStartingPoint() == FillStartingPoint::Bottom ? 1 : 0;
			if (ImGui::Combo("Fill Starting Point", &currentPoint, pointItems, IM_ARRAYSIZE(pointItems))) {
				sprite.SetFillStartingPoint(currentPoint == 0 ? FillStartingPoint::Top : FillStartingPoint::Bottom);
			}
		}
		break;
	case FillMethod::Horizontal:
		{
			const char* pointItems[] = { "Left", "Right" };
			int currentPoint = sprite.GetFillStartingPoint() == FillStartingPoint::Right ? 1 : 0;
			if (ImGui::Combo("Fill Starting Point", &currentPoint, pointItems, IM_ARRAYSIZE(pointItems))) {
				sprite.SetFillStartingPoint(currentPoint == 0 ? FillStartingPoint::Left : FillStartingPoint::Right);
			}
		}
		break;
	case FillMethod::Radial:
		{
			AOENGINE::ArcGaugeParam arcParam = sprite.GetArcGaugeParam();
			bool changed = false;

			changed |= ImGui::SliderFloat("FillAmount", &arcParam.fillAmount, 0.0f, 1.0f);
			ImGui::SameLine();
			changed |= ImGui::DragFloat(":", &arcParam.fillAmount, 0.01f);
			changed |= ImGui::DragFloat2("center", &arcParam.center.x, 0.01f);
			changed |= ImGui::DragFloat("innerRadius", &arcParam.innerRadius, 0.01f);
			changed |= ImGui::DragFloat("outerRadius", &arcParam.outerRadius, 0.01f);

			float startAngle = arcParam.startAngle * kToDegree;
			if (ImGui::DragFloat("startAngle", &startAngle, 1.0f, 0.0f, 360.0f)) {
				arcParam.startAngle = startAngle * kToRadian;
				changed = true;
			}

			float arcRange = arcParam.arcRange * kToDegree;
			if (ImGui::DragFloat("arcRange", &arcRange, 1.0f, 0.0f, 360.0f)) {
				arcParam.arcRange = arcRange * kToRadian;
				changed = true;
			}

			bool clockwise = arcParam.clockwise != 0;
			if (ImGui::Checkbox("clockwise", &clockwise)) {
				arcParam.clockwise = clockwise ? 1 : 0;
				changed = true;
			}

			if (changed) {
				sprite.SetArcGaugeParam(arcParam);
			}
		}
		break;
	case FillMethod::BothEnds:
		{
			const char* pointItems[] = { "TopBottom", "LeftRight" };
			int currentPoint = sprite.GetFillStartingPoint() == FillStartingPoint::LeftRight ? 1 : 0;
			if (ImGui::Combo("Fill Starting Point", &currentPoint, pointItems, IM_ARRAYSIZE(pointItems))) {
				sprite.SetFillStartingPoint(currentPoint == 0 ? FillStartingPoint::TopBottom : FillStartingPoint::LeftRight);
			}
		}
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Sprite単体のSave/Load UIを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpriteInspector::DrawSaveLoad(Sprite& sprite) {
	if (!ImGui::CollapsingHeader("Save & Load")) {
		return;
	}

	std::string groupName = sprite.GetSaveGroupName();
	if (InputTextWithString("GroupName", "##group", groupName)) {
		sprite.SetSaveGroupName(groupName);
	}

	std::string keyName = sprite.GetSaveKeyName();
	if (InputTextWithString("KeyName", "##key", keyName)) {
		sprite.SetSaveKeyName(keyName);
	}

	if (ImGui::Button("Save")) {
		sprite.Save(sprite.GetSaveGroupName(), sprite.GetSaveKeyName());
	}
	if (ImGui::Button("Load")) {
		sprite.Load(sprite.GetSaveGroupName(), sprite.GetSaveKeyName());
	}
}
