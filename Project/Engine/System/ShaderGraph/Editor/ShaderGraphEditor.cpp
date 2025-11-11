#include "ShaderGraphEditor.h"
#include "Engine/System/ShaderGraph/Editor/ShaderGraphSerializer.h"

static float zoom = 1.0f;           // 現在のズーム倍率
static ImVec2 pan = ImVec2(0, 0);   // 平行移動（パン）量

ShaderGraphEditor::~ShaderGraphEditor() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Init() {
	nodeFactory_.Init(editor);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Update() {
	Edit();
}

void ShaderGraphEditor::ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited) {
	if (!node || visited.contains(node)) return;
	visited.insert(node);

	// すべての入力ピンをチェック
	for (auto& in : node->getIns()) {
		// このInPinがリンクされているなら
		auto link = in->getLink().lock();
		if (link) {
			// 左側（出力ピン）の親ノードを取得
			ImFlow::BaseNode* srcNode = link->left()->getParent();
			ExecuteFrom(srcNode, visited); // 依存ノードを先に実行
		}

		node->customUpdate();
	}

	if (node->getIns().empty()) {
		node->customUpdate();
		return;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理dw
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Edit() {
	SaveGraph();
	ImGui::SameLine();
	LoadGraph();

	editor.update();

	// ----------------------
	// ↓ node更新処理
	// ----------------------
	std::unordered_set<ImFlow::BaseNode*> visited;
	for (auto& [id, nodePtr] : editor.getNodes()) {
		ImFlow::BaseNode* node = nodePtr.get();

		for (auto& out : node->getOuts()) {
			if (out->isConnected()) {
				continue;
			}
		}
		
		// たとえば PreviewNode が末端とする
		ExecuteFrom(node, visited);
	}

	// ----------------------
	// ↓ 編集処理
	// ----------------------
	ImGui::Begin("Inspector");
	for (auto& node : editor.getNodes()) {
		if (node.second.get()->isSelected()) {
			node.second.get()->updateGui();
		}
	}
	ImGui::End();

	// ---------------------- 
	// ↓ 作成処理
	// ----------------------
	CreateNode();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ nodeの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::CreateNode() {
	editor.rightClickPopUpContent([this](ImFlow::BaseNode*) {

		ImGui::Begin("PopUpMenu", nullptr);
		ImGui::TextUnformatted(" NodeContextMenu ");
		ImGui::Separator();

		nodeFactory_.CreateGui();

		ImGui::End();
								  });
}

void ShaderGraphEditor::LoadGraph() {
	if (ImGui::Button("Load")) {
		nodeFactory_.CreateGraph(ShaderGraphSerializer::Load("./Project/Packages/Game/Assets/GameData/ShaderGraph/", "test.json"));
	}
}

void ShaderGraphEditor::SaveGraph() {
	if (ImGui::Button("Save")) {
		ShaderGraphSerializer::Save("./Project/Packages/Game/Assets/GameData/ShaderGraph/", "test.json", editor);
	}
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