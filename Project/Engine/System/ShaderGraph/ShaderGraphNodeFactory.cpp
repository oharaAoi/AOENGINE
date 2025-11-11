#include "ShaderGraphNodeFactory.h"
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

void ShaderGraphNodeFactory::Init(ImFlow::ImNodeFlow& _editor) {
	RegisterNode<PropertyNode<float>>("Property/Float", _editor);
	RegisterNode<PropertyNode<Vector2>>("Property/Vector2", _editor);
	RegisterNode<PropertyNode<Vector3>>("Property/Vector3", _editor);
	RegisterNode<PropertyNode<Vector4>>("Property/Vector4", _editor);
	RegisterNode<PropertyNode<Color>>("Property/Color", _editor);

	RegisterNode<InOutPriorityNode<float>>("InOutPriority/Float", _editor);
	RegisterNode<InOutPriorityNode<Vector2>>("InOutPriority/Vector2", _editor);
	RegisterNode<InOutPriorityNode<Vector3>>("InOutPriority/Vector3", _editor);
	RegisterNode<InOutPriorityNode<Vector4>>("InOutPriority/Vector4", _editor);
	RegisterNode<InOutPriorityNode<Color>>("InOutPriority/Color", _editor);

	RegisterNode<TextureNode>("Texture/Texture", _editor);
	RegisterNode<SampleTexture2dNode>("Texture/SampleTexture2D", _editor);

	RegisterNode<BlendNode>("Merge/Blend", _editor);
	RegisterNode<MaskBlendNode>("Merge/MaskBlend", _editor);

	RegisterNode<MathAddNode>("Math/Add", _editor);
	RegisterNode<MathSubtractionNode>("Math/Subtraction", _editor);
	RegisterNode<MathMultiplyNode>("Math/Multiply", _editor);

	RegisterNode<TimeNode>("Other/TimeNode", _editor);
}

void ShaderGraphNodeFactory::CreateGraph(const json& _json) {
	std::map<std::string, std::vector<NodeEntry>> tree;
	for (auto& e : nodeEntries_) {
		auto slash = e.path.find('/');
		std::string category = e.path.substr(0, slash);
		tree[category].push_back(e);
	}

	std::unordered_map<uintptr_t, std::shared_ptr<ImFlow::BaseNode>> nodeMap;

	for (auto& jsonNode : _json["nodes"]) {
		std::string dataName = jsonNode["name"];
		uintptr_t id = jsonNode["id"];

		for (auto& [category, entries] : tree) {
			for (auto& enter : entries) {
				std::string name = enter.path.substr(enter.path.find('/') + 1);
				if (name == dataName) {
					std::shared_ptr<ImFlow::BaseNode> createdNode = enter.spawn(ImVec2(300, 200));

					if (createdNode) {
						createdNode->fromJson(jsonNode);
						nodeMap[id] = createdNode;
					}
				}
			}
		}
	}

	for (auto& jlink : _json["links"]) {
		uintptr_t fromNode = jlink["fromNode"].get<uintptr_t>();
		uintptr_t toNode = jlink["toNode"].get<uintptr_t>();

		auto outNode = nodeMap[fromNode];
		auto inNode = nodeMap[toNode];

		// JSON は string なので std::string で受け取る
		std::string fromPin = jlink["fromPin"].get<std::string>();
		std::string toPin = jlink["toPin"].get<std::string>();

		// ★ ここが大事：char* ではなく std::string 版を使う
		ImFlow::Pin* outPin = outNode->outPin(fromPin);
		ImFlow::Pin* inPin = inNode->inPin(toPin);

		if (outPin && inPin) {
			outPin->createLink(inPin);
		}
	}
}

void ShaderGraphNodeFactory::CreateGui() {
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
}
