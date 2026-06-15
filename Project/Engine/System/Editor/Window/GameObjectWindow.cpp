#include "GameObjectWindow.h"
#include <sstream>
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Editor/Inspector/InspectorRegistry.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

using namespace AOENGINE;

GameObjectWindow::GameObjectWindow() {}
GameObjectWindow::~GameObjectWindow() {}

void GameObjectWindow::Init() {
	selectedObjectHandle_ = ObjectHandle{};
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectWindow::AddAttributeGui(AOENGINE::AttributeGui* attribute, const std::string& label) {
	if (attribute) {
		attribute->SetName(label);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　同じ名前の場合の処理
//////////////////////////////////////////////////////////////////////////////////////////////////

std::string GameObjectWindow::MakeUniqueName(const std::string& baseName, const AOENGINE::SceneObject* ignoreObject) const {
	int count = 0;
	const std::string base = baseName.empty() ? "new object" : baseName;
	std::string newName = base;

	auto isDuplicate = [&](const std::string& name) {
		if (!sceneRenderer_) {
			return false;
		}

		for (const ObjectHandle& handle : sceneRenderer_->GetObjectHandles()) {
			SceneObject* object = sceneRenderer_->FindObject(handle);
			if (object && object != ignoreObject && object->GetName() == name) {
				return true;
			}
		}
		return false;
		};

	while (isDuplicate(newName)) {
		++count;
		newName = base + "(" + std::to_string(count) + ")";
	}

	return newName;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加Menuを表示する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::GameObjectWindow::CreateNewObjectWindow() {
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu(" + ")) {
			if (ImGui::MenuItem("3dObject")) {
				if (sceneRenderer_) {
					const std::string objectName = MakeUniqueName("new object");
					AOENGINE::BaseGameObject* newObject = sceneRenderer_->AddObject<AOENGINE::BaseGameObject>(objectName, "Object_Normal.json");
					if (newObject) {
						selectedObjectHandle_ = newObject->GetHandle();
					}
				}
			}

			if (ImGui::MenuItem("Sprite")) {
				canvas2d_->AddSprite("white.png", "new sprite");
			}

			if (ImGui::MenuItem("Text")) {
				canvas2d_->AddText("new text", "New Text");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集画面を表示する
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectWindow::InspectorWindow() {
	// -------------------------------------------------
	// ↓ ItemごとのImGuiを編集する
	// -------------------------------------------------
	ImGui::Begin("Inspector");
	SceneObject* selectedObject = GetSelectObject();
	if (selectedObject != nullptr) {
		bool isActive = selectedObject->IsActive();
		if (ImGui::Checkbox(" ", &isActive)) {
			selectedObject->SetActive(isActive);
		}
		ImGui::SameLine();
		std::string selectName = selectedObject->GetName();
		if (InputTextWithString("Name :", "##selectName", selectName)) {
			selectedObject->SetName(MakeUniqueName(selectName, selectedObject));
		}

		ImGui::Separator();
		if (!InspectorRegistry::GetInstance().DrawObject(*selectedObject)) {
			ImGui::TextUnformatted("No inspector registered");
		}
	} else {
		selectedObjectHandle_ = ObjectHandle{};
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ヒエラルキーを表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::GameObjectWindow::HierarchyWindow() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
	// -------------------------------------------------
	// ↓ Itemの選択
	// -------------------------------------------------
	if (ImGui::Begin("Hierarchy", nullptr, window_flags)) {
		// objectの追加表示
		CreateNewObjectWindow();

		if (sceneRenderer_) {
			for (const ObjectHandle& handle : sceneRenderer_->GetObjectHandles()) {
				SceneObject* object = sceneRenderer_->FindObject(handle);
				if (object && !IsChildObject(object)) {
					DrawHierarchyObject(*object);
				}
			}
		}
		ImGui::End();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　実行画面を表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::GameObjectWindow::ExecutionWindow() {
	if (ImGui::Begin("Game Window", nullptr)) {
		if (ImGui::IsWindowFocused()) {
			EditorWindows::GetInstance()->SetSelectWindow(this);
		}

		// sceneの表示
		processedSceneFrame_->DrawScene();

		// manipulateの表示
		if (ManipulateTool::isActive_) {
			if (ManipulateTool::is3dManipulate_) {
				if (sceneRenderer_ != nullptr) {
					sceneRenderer_->EditObject(processedSceneFrame_->GetAvailSize(), processedSceneFrame_->GetImagePos());
				}
			} else {
				if (canvas2d_ != nullptr) {
					canvas2d_->EditObject(processedSceneFrame_->GetAvailSize(), processedSceneFrame_->GetImagePos());
				}
			}
		}
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの削除
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::GameObjectWindow::DeleteObject(AOENGINE::AttributeGui* attribute) {
	(void)attribute;
}

SceneObject* AOENGINE::GameObjectWindow::GetSelectObject() const {
	if (!sceneRenderer_) {
		return nullptr;
	}

	return sceneRenderer_->FindObject(selectedObjectHandle_);
}

void AOENGINE::GameObjectWindow::DrawHierarchyObject(SceneObject& object) {
	const ObjectHandle handle = object.GetHandle();
	std::string label = object.GetName();
	label += "##object_";
	label += std::to_string(handle.index);
	label += "_";
	label += std::to_string(handle.generation);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	if (IsSelected(handle)) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (!object.HasChild()) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(label.c_str(), flags);
		if (ImGui::IsItemClicked()) {
			selectedObjectHandle_ = handle;
		}
		return;
	}

	const bool opened = ImGui::TreeNodeEx(label.c_str(), flags);
	if (ImGui::IsItemClicked()) {
		selectedObjectHandle_ = handle;
	}

	if (!opened) {
		return;
	}

	for (SceneObject* child : object.GetChildren()) {
		if (child) {
			DrawHierarchyObject(*child);
		}
	}

	ImGui::TreePop();
}

bool AOENGINE::GameObjectWindow::IsChildObject(const SceneObject* object) const {
	if (!sceneRenderer_ || !object) {
		return false;
	}

	for (const ObjectHandle& handle : sceneRenderer_->GetObjectHandles()) {
		SceneObject* parent = sceneRenderer_->FindObject(handle);
		if (!parent) {
			continue;
		}

		for (const SceneObject* child : parent->GetChildren()) {
			if (child == object) {
				return true;
			}
		}
	}

	return false;
}

bool AOENGINE::GameObjectWindow::IsSelected(const ObjectHandle& handle) const {
	return selectedObjectHandle_ == handle;
}
