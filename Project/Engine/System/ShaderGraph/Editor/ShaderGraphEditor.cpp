#include "ShaderGraphEditor.h"
#include "Engine/System/ShaderGraph/Node/PropertyNode.h"
#include "Engine/System/ShaderGraph/Node/InOutPriorityNode.h"
#include "Engine/System/ShaderGraph/Node/SampleTexture2dNode.h"
#include "Engine/System/ShaderGraph/Node/TextureNode.h"
#include "Engine/System/ShaderGraph/Node/BlendNode.h"
#include "Engine/System/ShaderGraph/Node/Blend/MaskBlendNode.h"
#include "Engine/System/ShaderGraph/Node/TimeNode.h"
#include "Engine/System/ShaderGraph/Node/Math/MathAddNode.h"
#include "Engine/System/ShaderGraph/Node/Math/MathMultiplyNode.h"
#include "Engine/System/ShaderGraph/Node/Math/MathSubtractionNode.h"
#include <map>

static float zoom = 1.0f;           // 現在のズーム倍率
static ImVec2 pan = ImVec2(0, 0);   // 平行移動（パン）量

ShaderGraphEditor::~ShaderGraphEditor() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::Init() {
	RegisterNode<PropertyNode<float>>("Property/Float");
	RegisterNode<PropertyNode<Vector2>>("Property/Vector2");
	RegisterNode<PropertyNode<Vector3>>("Property/Vector3");
	RegisterNode<PropertyNode<Vector4>>("Property/Vector4");
	RegisterNode<PropertyNode<Color>>("Property/Color");

	RegisterNode<InOutPriorityNode<float>>("InOutPriority/Float");
	RegisterNode<InOutPriorityNode<Vector2>>("InOutPriority/Vector2");
	RegisterNode<InOutPriorityNode<Vector3>>("InOutPriority/Vector3");
	RegisterNode<InOutPriorityNode<Vector4>>("InOutPriority/Vector4");
	RegisterNode<InOutPriorityNode<Color>>("InOutPriority/Color");

	RegisterNode<TextureNode>("Texture/Texture");
	RegisterNode<SampleTexture2dNode>("Texture/SampleTexture2D");

	RegisterNode<BlendNode>("Merge/Blend");
	RegisterNode<MaskBlendNode>("Merge/MaskBlend");

	RegisterNode<MathAddNode>("Math/Add");
	RegisterNode<MathSubtractionNode>("Math/Subtraction");
	RegisterNode<MathMultiplyNode>("Math/Multiply");

	RegisterNode<TimeNode>("Other/TimeNode");
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

		std::map<std::string, std::vector<NodeEntry>> tree;

		for (auto& e : nodeEntries_) {
			auto slash = e.path.find('/');
			std::string category = e.path.substr(0, slash);
			tree[category].push_back(e);
		}

		for (auto& [category, entries] : tree) {
			if (ImGui::BeginMenu(category.c_str())) {
				for (auto& e : entries) {
					auto name = e.path.substr(e.path.find('/') + 1);
					if (ImGui::MenuItem(name.c_str())) {

						ImGui::CloseCurrentPopup();
						ImGui::SetWindowFocus(nullptr);
						ImGui::SetActiveID(0, nullptr);
						e.spawn(ImVec2(300, 200));
					}
				}
				ImGui::EndMenu();
				ImGui::SetActiveID(0, nullptr);
			}
		}

		ImGui::End();
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
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ texture系のNodeの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::CreteTexture() {
	if (ImGui::BeginMenu(" Texture ")) {
		if (ImGui::MenuItem(" TextureNode ")) {
			auto node = editor.addNode<TextureNode>(ImVec2(200, 100));
			node->Init();
			node->setTitle(" TextureNode");
		}

		if (ImGui::MenuItem(" SampleTexture2d ")) {
			auto node = editor.addNode<SampleTexture2dNode>(ImVec2(200, 100));
			node->Init();
			node->setTitle("SampleTexture2D");
		}
		ImGui::EndMenu();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 合成系のNodeの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphEditor::CreateMerge() {
	if (ImGui::BeginMenu(" Merge ")) {
		if (ImGui::MenuItem(" Blend ")) {
			auto node = editor.addNode<BlendNode>(ImVec2(200, 100));
			node->Init();
			node->setTitle(" Blend");
		}
		ImGui::EndMenu();
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