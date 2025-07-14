#include "WeightSelectorNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

WeightSelectorNode::WeightSelectorNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::WeightSelector;
	SetNodeName("WeightSelector");
}

BehaviorStatus WeightSelectorNode::Execute() {
	return BehaviorStatus();
}

void WeightSelectorNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:", "##wightSelector", node_.name);
}
