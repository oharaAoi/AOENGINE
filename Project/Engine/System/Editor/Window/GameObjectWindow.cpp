#include "GameObjectWindow.h"
#include <algorithm>
#include <sstream>
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Editor/Inspector/InspectorRegistry.h"
#include "Engine/System/Editor/Inspector/Entity/SpriteInspector.h"
#include "Engine/System/Editor/Inspector/Entity/TextInspector.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/System/ParticleSystem/Field/GpuParticleField.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/WinApp/WinApp.h"

using namespace AOENGINE;

PostProcessSceneObject::PostProcessSceneObject(AOENGINE::PostProcess* postProcess)
	: postProcess_(postProcess) {}

void PostProcessSceneObject::Init() { isDestroy_ = false; }
void PostProcessSceneObject::Update() {}
void PostProcessSceneObject::PostUpdate() {}
void PostProcessSceneObject::PreDraw() const {}
void PostProcessSceneObject::Draw() const {}
void PostProcessSceneObject::Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) {}

PostEffectSceneObject::PostEffectSceneObject(AOENGINE::PostProcess* owner, PostEffectType type)
	: owner_(owner), type_(type) {}

void PostEffectSceneObject::Init() { isDestroy_ = false; }
void PostEffectSceneObject::Update() {}
void PostEffectSceneObject::PostUpdate() {}
void PostEffectSceneObject::PreDraw() const {}
void PostEffectSceneObject::Draw() const {}
void PostEffectSceneObject::Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) {}

std::shared_ptr<PostEffect::IPostEffect> PostEffectSceneObject::ResolveEffect() const {
	return owner_ ? owner_->GetEffect(type_) : nullptr;
}

GameObjectWindow::GameObjectWindow() {}
GameObjectWindow::~GameObjectWindow() {}

void GameObjectWindow::Init() {
	ResetInteractionState();
	postProcessObjectHandle_ = ObjectHandle{};
	postEffectObjectHandles_.clear();
}

void GameObjectWindow::ResetInteractionState() {
	selectedObjectHandle_ = ObjectHandle{};
	pendingDeleteHandle_ = ObjectHandle{};
	pendingDuplicateHandle_ = ObjectHandle{};
	pendingParentChildHandle_ = ObjectHandle{};
	pendingParentHandle_ = ObjectHandle{};
	pendingMoveToRoot_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void GameObjectWindow::AddPostProcess(AOENGINE::PostProcess* postProcess, const std::string& label) {
	if (!postProcess) {
		return;
	}

	if (registeredPostProcess_ != postProcess) {
		postProcessObjectHandle_ = ObjectHandle{};
		postEffectObjectHandles_.clear();
	}
	registeredPostProcess_ = postProcess;
	postProcessLabel_ = label.empty() ? "PostProcess" : label;
	EnsurePostProcessObjects();
}

void GameObjectWindow::SetSceneRenderer(AOENGINE::SceneRenderer* _renderer) {
	sceneRenderer_ = _renderer;
	EnsureCanvasObject();
	EnsureParticleObjects();
	EnsurePostProcessObjects();
}

void GameObjectWindow::EnsureParticleObjects() {
	if (!sceneRenderer_) { return; }
	SceneWorld& world = sceneRenderer_->GetSceneWorld();
	ParticleManager* particleManager = ParticleManager::GetInstance();
	if (!world.IsValid(particleManager->GetHandle()) || world.FindObject(particleManager->GetHandle()) != particleManager) {
		world.AddExternalObject(*particleManager, "ParticleManager");
	}
	GpuParticleManager::GetInstance()->RegisterSceneObjects(world);
}

void GameObjectWindow::SetCanvas2d(AOENGINE::Canvas2d* canvas) {
	canvas2d_ = canvas;
	EnsureCanvasObject();
}

void GameObjectWindow::EnsureCanvasObject() {
	if (!sceneRenderer_ || !canvas2d_) { return; }
	SceneWorld& world = sceneRenderer_->GetSceneWorld();
	canvas2d_->SetSceneWorld(&world);
	if (!world.IsValid(canvas2d_->GetHandle()) || world.FindObject(canvas2d_->GetHandle()) != canvas2d_) {
		world.AddExternalObject(*canvas2d_, "Canvas2d");
	}
	canvas2d_->AttachItemsToCanvas();
}

void GameObjectWindow::EnsurePostProcessObjects() {
	if (!sceneRenderer_ || !registeredPostProcess_) {
		return;
	}

	PostProcessSceneObject* root = dynamic_cast<PostProcessSceneObject*>(sceneRenderer_->FindObject(postProcessObjectHandle_));
	if (!root || root->GetPostProcess() != registeredPostProcess_) {
		root = sceneRenderer_->GetSceneWorld().CreateObject<PostProcessSceneObject>(postProcessLabel_, registeredPostProcess_);
		if (!root) {
			postProcessObjectHandle_ = ObjectHandle{};
			return;
		}
		postProcessObjectHandle_ = root->GetHandle();
	}
	root->SetName(postProcessLabel_);
	sceneRenderer_->MoveToRoot(postProcessObjectHandle_);

	for (PostEffectType type : registeredPostProcess_->GetEffectOrder()) {
		if (!registeredPostProcess_->GetEffect(type)) {
			continue;
		}

		PostEffectSceneObject* effectObject = nullptr;
		auto handleIt = postEffectObjectHandles_.find(type);
		if (handleIt != postEffectObjectHandles_.end()) {
			effectObject = dynamic_cast<PostEffectSceneObject*>(sceneRenderer_->FindObject(handleIt->second));
			if (effectObject && (effectObject->GetOwner() != registeredPostProcess_ || effectObject->GetEffectType() != type)) {
				effectObject = nullptr;
			}
		}

		if (!effectObject) {
			effectObject = sceneRenderer_->GetSceneWorld().CreateObject<PostEffectSceneObject>(
				PostProcess::GetEffectName(type), registeredPostProcess_, type);
			if (!effectObject) {
				continue;
			}
			postEffectObjectHandles_[type] = effectObject->GetHandle();
		}

		effectObject->SetName(PostProcess::GetEffectName(type));
		sceneRenderer_->SetParent(effectObject->GetHandle(), postProcessObjectHandle_);
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
		Sprite* canvasItem = dynamic_cast<Sprite*>(selectedObject);
		ParticleManager* particleManager = dynamic_cast<ParticleManager*>(selectedObject);
		GpuParticleManager* gpuParticleManager = dynamic_cast<GpuParticleManager*>(selectedObject);
		GpuParticleField* gpuParticleField = dynamic_cast<GpuParticleField*>(selectedObject);
		PostProcessSceneObject* postProcessObject = dynamic_cast<PostProcessSceneObject*>(selectedObject);
		PostEffectSceneObject* postEffectObject = dynamic_cast<PostEffectSceneObject*>(selectedObject);
		std::shared_ptr<PostEffect::IPostEffect> selectedEffect = postEffectObject ? postEffectObject->ResolveEffect() : nullptr;

		bool isActive = selectedEffect
			? selectedEffect->GetIsEnable()
			: selectedObject->IsSelfActive();
		if (!postProcessObject && ImGui::Checkbox(" ", &isActive)) {
			if (selectedEffect) {
				selectedEffect->SetIsEnable(isActive);
			} else {
				selectedObject->SetActive(isActive);
			}
		}
		if (!postProcessObject) {
			ImGui::SameLine();
		}
		std::string selectName = selectedObject->GetName();
		if (postProcessObject || postEffectObject) {
			ImGui::TextUnformatted(selectName.c_str());
		} else if (InputTextWithString("Name :", "##selectName", selectName)) {
			const std::string uniqueName = MakeUniqueName(selectName, selectedObject);
			selectedObject->SetName(uniqueName);
		}

		DrawPrefabOverride(*selectedObject);
		ImGui::Separator();
		if (canvasItem) {
			canvasItem->Debug_Gui();
		} else if (particleManager) {
			particleManager->Debug_Gui();
		} else if (gpuParticleManager) {
			gpuParticleManager->Debug_Gui();
		} else if (gpuParticleField) {
			gpuParticleField->Debug_Gui();
		} else if (!InspectorRegistry::GetInstance().DrawObject(*selectedObject)) {
			ImGui::TextUnformatted("No inspector registered");
		}
	} else {
		selectedObjectHandle_ = ObjectHandle{};
	}
	ImGui::End();
}

SceneObject* GameObjectWindow::FindPrefabInstanceRoot(SceneObject& object) const {
	if (!sceneRenderer_) { return nullptr; }

	SceneWorld& world = sceneRenderer_->GetSceneWorld();
	SceneObject* current = &object;
	while (current) {
		if (current->IsPrefabInstanceRoot()) {
			return current;
		}

		const ObjectHandle parentHandle = world.GetParentHandle(current->GetHandle());
		if (!parentHandle.IsValid()) {
			break;
		}
		current = world.FindObject(parentHandle);
	}
	return nullptr;
}

void GameObjectWindow::DrawPrefabOverride(SceneObject& object) {
	SceneObject* prefabRoot = FindPrefabInstanceRoot(object);
	if (!prefabRoot) { return; }

	const std::string prefabName = prefabRoot->GetPrefabSource();
	if (prefabName.empty()) { return; }

	ImGui::Spacing();
	ImGui::Text("Prefab: %s", prefabName.c_str());
	if (ImGui::Button("Apply Prefab Override")) {
		PrefabManager::GetInstance()->SavePrefab(
			prefabName, prefabRoot->GetHandle(), *sceneRenderer_);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Overwrite %s.prefab.json with this instance hierarchy", prefabName.c_str());
	}
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
		EnsureCanvasObject();
		EnsureParticleObjects();
		EnsurePostProcessObjects();

		if (sceneRenderer_) {
			for (const ObjectHandle& handle : sceneRenderer_->GetRootObjectHandles()) {
				SceneObject* object = sceneRenderer_->FindObject(handle);
				if (object) {
					DrawHierarchyObject(*object);
				}
			}

			const ImVec2 dropSize(ImGui::GetContentRegionAvail().x, (std::max)(24.0f, ImGui::GetContentRegionAvail().y));
			ImGui::InvisibleButton("##HierarchyRootDropTarget", dropSize);
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT_HANDLE")) {
					pendingParentChildHandle_ = *static_cast<const ObjectHandle*>(payload->Data);
					pendingParentHandle_ = ObjectHandle{};
					pendingMoveToRoot_ = true;
				}
				ImGui::EndDragDropTarget();
			}

			ApplyPendingHierarchyAction();
			ApplyPendingParentChange();
		}
		ImGui::End();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　実行画面を表示
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::GameObjectWindow::ExecutionWindow() {
	if (ImGui::Begin("Game View", nullptr)) {
		if (ImGui::IsWindowFocused()) {
			EditorWindows::GetInstance()->SetSelectWindow(this);
		}

		if (processedSceneFrame_) {
			processedSceneFrame_->DrawScene();
		}
	}
	ImGui::End();

	if (ImGui::Begin("Scene View", nullptr)) {
		if (ImGui::IsWindowFocused()) {
			EditorWindows::GetInstance()->SetSelectWindow(this);
		}

		bool sceneViewClicked = false;
		bool gizmoCapturedMouse = false;
		ImVec2 clickedPixel{};
		if (editorSceneFrame_) {
			editorSceneFrame_->DrawScene();
			sceneViewClicked = ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			if (sceneViewClicked) {
				const ImVec2 mousePos = ImGui::GetMousePos();
				const ImVec2 imagePos = editorSceneFrame_->GetImagePos();
				clickedPixel = ImVec2(mousePos.x - imagePos.x, mousePos.y - imagePos.y);
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB_ASSET")) {
					if (payload->Data && payload->DataSize > 1) {
						const ImVec2 mousePos = ImGui::GetMousePos();
						const ImVec2 imagePos = editorSceneFrame_->GetImagePos();
						InstantiatePrefabAtScenePosition(
							static_cast<const char*>(payload->Data),
							{ mousePos.x - imagePos.x, mousePos.y - imagePos.y },
							editorSceneFrame_->GetAvailSize());
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		// Scene Viewで選択中のオブジェクトだけにギズモを表示する。
		if (editorSceneFrame_) {
			Engine::ActivateSceneView(SceneViewType::Editor);

			ISceneObject* selectedObject = dynamic_cast<ISceneObject*>(GetSelectObject());
			if (selectedObject && selectedObject->IsActive()) {
				selectedObject->Manipulate(editorSceneFrame_->GetAvailSize(), editorSceneFrame_->GetImagePos());

				// Manipulate()内でImGuizmo::PopID()された後はIsUsing()が
				// falseになり得る。IDに依存しないIsUsingAny()で操作を保護する。
				gizmoCapturedMouse = ImGuizmo::IsOver() || ImGuizmo::IsUsingAny();
			}

			Engine::ActivateSceneView(SceneViewType::Game);
		}

		if (sceneViewClicked && sceneRenderer_ && !gizmoCapturedMouse) {
			Engine::ActivateSceneView(SceneViewType::Editor);
			selectedObjectHandle_ = sceneRenderer_->PixelPick(clickedPixel, editorSceneFrame_->GetAvailSize());
			Engine::ActivateSceneView(SceneViewType::Game);
		}
	}
	ImGui::End();
}

void AOENGINE::GameObjectWindow::InstantiatePrefabAtScenePosition(
	const std::string& prefabName, const ImVec2& pixelPos, const ImVec2& imageSize) {
	if (!sceneRenderer_ || !canvas2d_ || prefabName.empty() ||
		imageSize.x <= 0.0f || imageSize.y <= 0.0f) {
		return;
	}

	SceneObject* root = PrefabManager::GetInstance()->Instantiate(prefabName, *sceneRenderer_, *canvas2d_);
	if (!root) { return; }

	if (auto* gameObject = dynamic_cast<BaseGameObject*>(root); gameObject && gameObject->GetTransform()) {
		Engine::ActivateSceneView(SceneViewType::Editor);
		const float ndcX = pixelPos.x / imageSize.x * 2.0f - 1.0f;
		const float ndcY = 1.0f - pixelPos.y / imageSize.y * 2.0f;
		const Math::Matrix4x4 inverseViewProjection = Render::GetViewProjectionMat().Inverse();
		const Math::Vector3 nearPoint = TransformCoord({ ndcX, ndcY, 0.0f }, inverseViewProjection);
		const Math::Vector3 farPoint = TransformCoord({ ndcX, ndcY, 1.0f }, inverseViewProjection);
		const Math::Vector3 direction = (farPoint - nearPoint).Normalize();

		Math::Vector3 position = nearPoint + direction * 10.0f;
		if (std::abs(direction.y) > 0.0001f) {
			const float distance = -nearPoint.y / direction.y;
			if (distance >= 0.0f) {
				position = nearPoint + direction * distance;
			}
		}
		gameObject->GetTransform()->SetTranslate(position);
		Engine::ActivateSceneView(SceneViewType::Game);
	} else if (auto* sprite = dynamic_cast<Sprite*>(root)) {
		const Math::Vector2 canvasPosition = {
			pixelPos.x / imageSize.x * static_cast<float>(WinApp::sClientWidth),
			pixelPos.y / imageSize.y * static_cast<float>(WinApp::sClientHeight)
		};
		sprite->SetTranslate(canvasPosition);
	}

	selectedObjectHandle_ = root->GetHandle();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Objectの削除
//////////////////////////////////////////////////////////////////////////////////////////////////

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
		DrawHierarchyDragDrop(object);
		DrawHierarchyContextMenu(object);
		return;
	}

	const bool opened = ImGui::TreeNodeEx(label.c_str(), flags);
	if (ImGui::IsItemClicked()) {
		selectedObjectHandle_ = handle;
	}
	DrawHierarchyDragDrop(object);
	DrawHierarchyContextMenu(object);

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

void AOENGINE::GameObjectWindow::DrawHierarchyDragDrop(SceneObject& object) {
	const ObjectHandle handle = object.GetHandle();
	if (object.GetScenePersistence() == ScenePersistence::SceneData && ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("SCENE_OBJECT_HANDLE", &handle, sizeof(handle));
		ImGui::TextUnformatted(object.GetName().c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT_HANDLE")) {
			pendingParentChildHandle_ = *static_cast<const ObjectHandle*>(payload->Data);
			pendingParentHandle_ = handle;
			pendingMoveToRoot_ = false;
		}
		ImGui::EndDragDropTarget();
	}
}

void AOENGINE::GameObjectWindow::DrawHierarchyContextMenu(SceneObject& object) {
	if (!ImGui::BeginPopupContextItem()) {
		return;
	}

	const bool canEdit = dynamic_cast<BaseGameObject*>(&object) != nullptr || dynamic_cast<Sprite*>(&object) != nullptr;
	if (ImGui::MenuItem("Duplicate", nullptr, false, canEdit)) {
		pendingDuplicateHandle_ = object.GetHandle();
	}
	if (ImGui::MenuItem("Delete", nullptr, false, canEdit)) {
		pendingDeleteHandle_ = object.GetHandle();
	}

	ImGui::EndPopup();
}

void AOENGINE::GameObjectWindow::ApplyPendingHierarchyAction() {
	if (!sceneRenderer_) {
		return;
	}

	if (pendingDuplicateHandle_.IsValid()) {
		BaseGameObject* source = dynamic_cast<BaseGameObject*>(sceneRenderer_->FindObject(pendingDuplicateHandle_));
		if (source) {
			const std::string duplicateName = MakeUniqueName(source->GetName());
			if (BaseGameObject* duplicate = sceneRenderer_->DuplicateObject(*source, duplicateName)) {
				selectedObjectHandle_ = duplicate->GetHandle();
			}
		} else if (Sprite* sourceSprite = dynamic_cast<Sprite*>(sceneRenderer_->FindObject(pendingDuplicateHandle_))) {
			const std::string duplicateName = MakeUniqueName(sourceSprite->GetName());
			Sprite* duplicate = nullptr;
			if (Text* sourceText = dynamic_cast<Text*>(sourceSprite)) {
				Text* text = canvas2d_->AddText(duplicateName, sourceText->GetText(), sourceText->GetRenderQueue());
				if (text) {
					text->SetFontPath(sourceText->GetFontPath());
					text->SetFontSize(sourceText->GetFontSize());
					text->SetTextColor(sourceText->GetTextColor());
					text->SetTextAnchorPoint(sourceText->GetTextAnchorPoint());
					duplicate = text;
				}
			} else {
				duplicate = canvas2d_->AddSprite(sourceSprite->GetTextureName(), duplicateName, sourceSprite->GetRenderQueue());
				if (duplicate) {
					duplicate->SetColor(sourceSprite->GetColor());
					duplicate->SetDrawRange(sourceSprite->GetDrawRange());
					duplicate->SetLeftTop(sourceSprite->GetLeftTop());
					duplicate->SetAnchorPoint(sourceSprite->GetAnchorPoint());
					duplicate->SetIsFlipX(sourceSprite->GetIsFlipX());
					duplicate->SetIsFlipY(sourceSprite->GetIsFlipY());
				}
			}
			if (duplicate) {
				duplicate->GetTransform()->SetSRT(sourceSprite->GetTransform()->GetTransform());
				duplicate->SetActive(sourceSprite->IsSelfActive());
				selectedObjectHandle_ = duplicate->GetHandle();
			}
		}
		pendingDuplicateHandle_ = ObjectHandle{};
	}

	if (pendingDeleteHandle_.IsValid()) {
		if (selectedObjectHandle_ == pendingDeleteHandle_) {
			selectedObjectHandle_ = ObjectHandle{};
		}
		sceneRenderer_->DestroyObject(pendingDeleteHandle_);
		pendingDeleteHandle_ = ObjectHandle{};
	}
}

void AOENGINE::GameObjectWindow::ApplyPendingParentChange() {
	if (!sceneRenderer_ || !pendingParentChildHandle_.IsValid()) { return; }

	bool changed = false;
	if (pendingMoveToRoot_) {
		changed = sceneRenderer_->MoveToRoot(pendingParentChildHandle_);
	} else if (pendingParentHandle_.IsValid()) {
		changed = sceneRenderer_->SetParent(pendingParentChildHandle_, pendingParentHandle_);
	}

	if (changed) {
		SyncTransformParent(pendingParentChildHandle_, pendingMoveToRoot_ ? ObjectHandle{} : pendingParentHandle_);
	}

	pendingParentChildHandle_ = ObjectHandle{};
	pendingParentHandle_ = ObjectHandle{};
	pendingMoveToRoot_ = false;
}

void AOENGINE::GameObjectWindow::SyncTransformParent(const ObjectHandle& childHandle, const ObjectHandle& parentHandle) {
	if (!sceneRenderer_) { return; }
	SceneObject* child = sceneRenderer_->FindObject(childHandle);
	SceneObject* parent = sceneRenderer_->FindObject(parentHandle);

	if (BaseGameObject* childObject = dynamic_cast<BaseGameObject*>(child)) {
		BaseGameObject* parentObject = dynamic_cast<BaseGameObject*>(parent);
		if (parentObject && parentObject->GetTransform()) {
			childObject->GetTransform()->SetParent(parentObject->GetTransform()->GetWorldMatrix());
		} else {
			childObject->GetTransform()->ClearParent();
		}
		return;
	}

	if (Sprite* childSprite = dynamic_cast<Sprite*>(child)) {
		Sprite* parentSprite = dynamic_cast<Sprite*>(parent);
		if (parentSprite && parentSprite->GetTransform()) {
			childSprite->GetTransform()->SetParent(parentSprite->GetTransform()->GetMatrix());
		} else {
			childSprite->GetTransform()->ClearParent();
		}
	}
}

bool AOENGINE::GameObjectWindow::IsSelected(const ObjectHandle& handle) const {
	return selectedObjectHandle_ == handle;
}
