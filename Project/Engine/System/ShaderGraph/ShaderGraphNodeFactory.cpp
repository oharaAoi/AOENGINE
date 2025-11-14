#include "ShaderGraphNodeFactory.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/System/ShaderGraph/Node/PropertyNode.h"
#include "Engine/System/ShaderGraph/Node/InOutPriorityNode.h"
#include "Engine/System/ShaderGraph/Node/SampleTexture2dNode.h"
#include "Engine/System/ShaderGraph/Node/TextureNode.h"
#include "Engine/System/ShaderGraph/Node/Blend/BlendNode.h"
#include "Engine/System/ShaderGraph/Node/Blend/MaskBlendNode.h"
#include "Engine/System/ShaderGraph/Node/TimeNode.h"
#include "Engine/System/ShaderGraph/Node/Math/MathAddNode.h"
#include "Engine/System/ShaderGraph/Node/Math/MathMultiplyNode.h"
#include "Engine/System/ShaderGraph/Node/Math/MathSubtractionNode.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ShaderGraphResultNode> ShaderGraphNodeFactory::Init(ImFlow::ImNodeFlow* _editor) {
	nodeEntries_.clear();
	RegisterNode<PropertyNode<float>>("Property/Property_Float", _editor);
	RegisterNode<PropertyNode<Vector2>>("Property/Property_Vector2", _editor);
	RegisterNode<PropertyNode<Vector3>>("Property/Property_Vector3", _editor);
	RegisterNode<PropertyNode<Vector4>>("Property/Property_Vector4", _editor);
	RegisterNode<PropertyNode<Color>>("Property/Property_Color", _editor);

	RegisterNode<InOutPriorityNode<float>>("InOutPriority/InOut_Float", _editor);
	RegisterNode<InOutPriorityNode<Vector2>>("InOutPriority/InOut_Vector2", _editor);
	RegisterNode<InOutPriorityNode<Vector3>>("InOutPriority/InOut_Vector3", _editor);
	RegisterNode<InOutPriorityNode<Vector4>>("InOutPriority/InOut_Vector4", _editor);
	RegisterNode<InOutPriorityNode<Color>>("InOutPriority/InOut_Color", _editor);

	RegisterNode<TextureNode>("Texture/Texture", _editor);
	RegisterNode<SampleTexture2dNode>("Texture/SampleTexture2D", _editor);

	RegisterNode<BlendNode>("Merge/Blend", _editor);
	RegisterNode<MaskBlendNode>("Merge/MaskBlend", _editor);

	RegisterNode<MathAddNode>("Math/Add", _editor);
	RegisterNode<MathSubtractionNode>("Math/Subtraction", _editor);
	RegisterNode<MathMultiplyNode>("Math/Multiply", _editor);

	RegisterNode<TimeNode>("Other/TimeNode", _editor);

	RegisterNode<ShaderGraphResultNode>("Result/ResultNode", _editor);

	std::shared_ptr<ShaderGraphResultNode> root = _editor->addNode<ShaderGraphResultNode>(ImVec2(200, 300));
	root->Init();
	root->setTitle("ResultNode");
	return root;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Graphを作成する
///////////////////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ShaderGraphResultNode> ShaderGraphNodeFactory::CreateGraph(const json& _json) {
	std::map<std::string, std::vector<NodeEntry>> tree;
	for (auto& e : nodeEntries_) {
		auto slash = e.path.find('/');
		std::string category = e.path.substr(0, slash);
		tree[category].push_back(e);
	}

	std::unordered_map<uintptr_t, std::shared_ptr<ImFlow::BaseNode>> nodeMap;
	// nodeを作成する
	for (auto& jsonNode : _json["nodes"]) {
		std::string dataName = jsonNode["name"];
		uintptr_t id = jsonNode["id"];

		for (auto& [category, entries] : tree) {
			bool isBreak = false;
			for (auto& enter : entries) {
				std::string name = enter.path.substr(enter.path.find('/') + 1);
				if (name == dataName) {
					std::shared_ptr<ImFlow::BaseNode> createdNode = enter.spawn(ImVec2(300, 200));

					if (createdNode) {
						createdNode->fromJson(jsonNode);
						nodeMap[id] = createdNode;
						isBreak = true;
						break;
					}
				}
			}

			if (isBreak) {
				break;
			}
		}
	}

	// リンクを結ぶ
	for (auto& jlink : _json["links"]) {
		uintptr_t fromNode = jlink["fromNode"].get<uintptr_t>();
		uintptr_t toNode = jlink["toNode"].get<uintptr_t>();

		// mapに登録されているNodeとidが一致するものを取得する
		auto outNode = nodeMap[fromNode];
		auto inNode = nodeMap[toNode];

		// jsonから各ピンの名前を取得する
		std::string fromPin = jlink["fromPin"].get<std::string>();
		std::string toPin = jlink["toPin"].get<std::string>();

		// 名前と一致するピンを作成する
		ImFlow::Pin* outPin = outNode->outPin(fromPin);
		ImFlow::Pin* inPin = inNode->inPin(toPin);

		if (outPin && inPin) {
			outPin->createLink(inPin);
		}
	}

	std::shared_ptr<ShaderGraphResultNode> result = nullptr;
	for (auto& [id, node] : nodeMap) {
		if (node->getName() == "ResultNode") {
			result = std::dynamic_pointer_cast<ShaderGraphResultNode>(node);
		
			if (result) {
				return result;
			} else {
				// 型が違っていた場合
				Logger::AssertLog("Don`t Find ResultNode");
			}
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ GraphをGui上から作成する
///////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGraphNodeFactory::CreateGui(const ImVec2& _pos) {
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
					e.spawn(_pos);
				}
			}
			ImGui::EndMenu();
			ImGui::SetActiveID(0, nullptr);
		}
	}
}
