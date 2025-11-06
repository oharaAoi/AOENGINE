#include "ShaderGraphEditor.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/Node/PropertyNode.h"
#include "Engine/System/ShaderGraph/Node/SampleTexture2dNode.h"

static float zoom = 1.0f;           // 現在のズーム倍率
static ImVec2 pan = ImVec2(0, 0);   // 平行移動（パン）量

ShaderGraphEditor::~ShaderGraphEditor() {
	ax::NodeEditor::DestroyEditor(context_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Init() {
	context_ = ax::NodeEditor::CreateEditor();
	ax::NodeEditor::SetCurrentEditor(context_);
	auto& style = ax::NodeEditor::GetStyle();
	style.LinkStrength = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Update() {
	Edit();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理dw
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Edit() {
	editor.update();

	// ----------------------
	// ↓ 作成処理
	// ----------------------
	CreateNode();

	CreateProperty();

	// ----------------------
	// ↓ nodeの表示
	// ----------------------

	for (const auto& node : nodes_) {
		node->draw();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ nodeの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::CreateNode() {
	editor.rightClickPopUpContent([this](ImFlow::BaseNode*) {
		ImGui::TextUnformatted(" NodeContextMenu ");
		ImGui::Separator();

		CreteTexture();
		CreateProperty();
								  });
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ propertyの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::CreateProperty() {
	if (ImGui::BeginMenu(" Property ")) {
		if (ImGui::MenuItem(" Float ")) {
			auto node = editor.addNode<PropertyNode<float>>(ImVec2(200, 100), "Float");
			node->setTitle("Float");
		}

		if (ImGui::MenuItem(" Vector2 ")) {
			auto node = editor.addNode<PropertyNode<Vector2>>(ImVec2(200, 100), "Vector2");
			node->setTitle("Vector2");
		}

		if (ImGui::MenuItem(" Vector3 ")) {
			auto node = editor.addNode<PropertyNode<Vector3>>(ImVec2(200, 100), "Vector3");
			node->setTitle("Vector3");
		}

		if (ImGui::MenuItem(" Vector4 ")) {
			auto node = editor.addNode<PropertyNode<Vector4>>(ImVec2(200, 100), "Vector4");
			node->setTitle("Vector4");
		}

		if (ImGui::MenuItem(" Color ")) {
			auto node = editor.addNode<PropertyNode<Color>>(ImVec2(200, 100), "Color");
			node->setTitle("Color");
		}
		ImGui::EndMenu();
	}
	//for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
	//	if (ImGui::Selectable(items[i])) {
	//		// 通常クリック処理（必要なら）
	//	}
	//	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
	//		ImGui::SetDragDropPayload("PROPERTY_ITEM", items[i], strlen(items[i]) + 1);
	//		ImGui::Text("Create: %s", items[i]);
	//		ImGui::EndDragDropSource();
	//	}
	//}
	//ImGui::End();

	//ImVec2 size = ImGui::GetContentRegionAvail();
	//ImGui::Dummy(size);
	//if (ImGui::BeginDragDropTarget()) {
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PROPERTY_ITEM")) {
	//		const char* droppedType = (const char*)payload->Data;
	//		printf("Create node: %s\n", droppedType);
	//	}
	//	ImGui::EndDragDropTarget();
	//}
}

void ShaderGraphEditor::CreteTexture() {
	if (ImGui::BeginMenu(" Texture ")) {
		if (ImGui::MenuItem(" Texture2d")) {}
		if (ImGui::MenuItem(" SampleTexture2d")) {
			auto node = editor.addNode<SampleTexture2dNode>(ImVec2(200, 100));
			node->setTitle("SampleTexture2D");
		}
		ImGui::EndMenu();
	}
}
