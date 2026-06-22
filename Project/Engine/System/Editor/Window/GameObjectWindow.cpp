#include "GameObjectWindow.h"
#include <algorithm>
#include <sstream>
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Editor/Inspector/InspectorRegistry.h"
#include "Engine/System/Editor/Inspector/Entity/SpriteInspector.h"
#include "Engine/System/Editor/Inspector/Entity/TextInspector.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

using namespace AOENGINE;

AttributeGuiSceneObject::AttributeGuiSceneObject(AOENGINE::AttributeGui* attribute)
	: attribute_(attribute) {}

void AttributeGuiSceneObject::Init() {
	isDestroy_ = false;
	SyncFromAttribute();
}

void AttributeGuiSceneObject::Update() {
	SyncFromAttribute();
}

void AttributeGuiSceneObject::PostUpdate() {}

void AttributeGuiSceneObject::PreDraw() const {}

void AttributeGuiSceneObject::Draw() const {}

void AttributeGuiSceneObject::Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) {}

void AttributeGuiSceneObject::DrawInspector() {
	if (!attribute_) {
		ImGui::TextUnformatted("No attribute registered");
		return;
	}

	if (AOENGINE::Text* text = dynamic_cast<AOENGINE::Text*>(attribute_)) {
		AOENGINE::TextInspector::Draw(*text);
		return;
	}

	if (AOENGINE::Sprite* sprite = dynamic_cast<AOENGINE::Sprite*>(attribute_)) {
		AOENGINE::SpriteInspector::Draw(*sprite);
		return;
	}

	attribute_->Debug_Gui();
}

void AttributeGuiSceneObject::SyncFromAttribute() {
	if (!attribute_) {
		return;
	}

	SetName(attribute_->GetName());
	SetActive(attribute_->GetIsActive());
}

void AttributeGuiSceneObject::SetAttributeName(const std::string& name) {
	SetName(name);
	if (attribute_) {
		attribute_->SetName(name);
	}
}

void AttributeGuiSceneObject::SetAttributeActive(bool isActive) {
	SetActive(isActive);
	if (attribute_) {
		attribute_->SetIsActive(isActive);
	}
}

GameObjectWindow::GameObjectWindow() {}
GameObjectWindow::~GameObjectWindow() {}

void GameObjectWindow::Init() {
	selectedObjectHandle_ = ObjectHandle{};
	registeredAttributes_.clear();
	attributeObjectHandles_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectWindow::AddAttributeGui(AOENGINE::AttributeGui* attribute, const std::string& label) {
	if (!attribute) {
		return;
	}

	attribute->SetName(label);
	if (!HasRegisteredAttribute(attribute)) {
		registeredAttributes_.push_back(attribute);
	}

	EnsureAttributeGuiObjects();
}

void GameObjectWindow::SetSceneRenderer(AOENGINE::SceneRenderer* _renderer) {
	if (sceneRenderer_ != _renderer) {
		attributeObjectHandles_.clear();
	}

	sceneRenderer_ = _renderer;
	EnsureAttributeGuiObjects();
}

void GameObjectWindow::EnsureAttributeGuiObjects() {
	if (!sceneRenderer_) {
		return;
	}

	for (AOENGINE::AttributeGui* attribute : registeredAttributes_) {
		EnsureAttributeGuiObjectRecursive(attribute, AOENGINE::ObjectHandle{});
	}
}

AOENGINE::ObjectHandle GameObjectWindow::EnsureAttributeGuiObjectRecursive(AOENGINE::AttributeGui* attribute, const AOENGINE::ObjectHandle& parentHandle) {
	if (!attribute || !sceneRenderer_) {
		return AOENGINE::ObjectHandle{};
	}

	AttributeGuiSceneObject* attributeObject = nullptr;
	AOENGINE::ObjectHandle handle{};

	auto handleIt = attributeObjectHandles_.find(attribute);
	if (handleIt != attributeObjectHandles_.end()) {
		attributeObject = dynamic_cast<AttributeGuiSceneObject*>(sceneRenderer_->FindObject(handleIt->second));
		if (attributeObject && attributeObject->GetAttribute() == attribute) {
			handle = handleIt->second;
			attributeObject->SyncFromAttribute();
		}
	}

	if (!attributeObject) {
		attributeObject = sceneRenderer_->GetSceneWorld().CreateObject<AttributeGuiSceneObject>(attribute->GetName(), attribute);
		if (!attributeObject) {
			attributeObjectHandles_.erase(attribute);
			return AOENGINE::ObjectHandle{};
		}

		handle = attributeObject->GetHandle();
		attributeObjectHandles_[attribute] = handle;
	}

	if (parentHandle.IsValid()) {
		sceneRenderer_->SetParent(handle, parentHandle);
	} else {
		sceneRenderer_->MoveToRoot(handle);
	}

	for (AOENGINE::AttributeGui* child : attribute->GetChildren()) {
		EnsureAttributeGuiObjectRecursive(child, handle);
	}

	return handle;
}

bool GameObjectWindow::HasRegisteredAttribute(AOENGINE::AttributeGui* attribute) const {
	return std::find(registeredAttributes_.begin(), registeredAttributes_.end(), attribute) != registeredAttributes_.end();
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
		AttributeGuiSceneObject* attributeObject = dynamic_cast<AttributeGuiSceneObject*>(selectedObject);

		bool isActive = attributeObject && attributeObject->GetAttribute()
			? attributeObject->GetAttribute()->GetIsActive()
			: selectedObject->IsActive();
		if (ImGui::Checkbox(" ", &isActive)) {
			if (attributeObject) {
				attributeObject->SetAttributeActive(isActive);
			} else {
				selectedObject->SetActive(isActive);
			}
		}
		ImGui::SameLine();
		std::string selectName = attributeObject && attributeObject->GetAttribute()
			? attributeObject->GetAttribute()->GetName()
			: selectedObject->GetName();
		if (InputTextWithString("Name :", "##selectName", selectName)) {
			const std::string uniqueName = MakeUniqueName(selectName, selectedObject);
			if (attributeObject) {
				attributeObject->SetAttributeName(uniqueName);
			} else {
				selectedObject->SetName(uniqueName);
			}
		}

		ImGui::Separator();
		if (attributeObject) {
			attributeObject->DrawInspector();
		} else if (!InspectorRegistry::GetInstance().DrawObject(*selectedObject)) {
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
		EnsureAttributeGuiObjects();

		if (sceneRenderer_) {
			for (const ObjectHandle& handle : sceneRenderer_->GetRootObjectHandles()) {
				SceneObject* object = sceneRenderer_->FindObject(handle);
				if (object) {
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

bool AOENGINE::GameObjectWindow::IsSelected(const ObjectHandle& handle) const {
	return selectedObjectHandle_ == handle;
}
