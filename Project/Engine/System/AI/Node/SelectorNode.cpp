#include "SelectorNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コンストラクタ
///////////////////////////////////////////////////////////////////////////////////////////////

SelectorNode::SelectorNode() {
	color_ = ImColor(144, 238, 144);
	baseColor_ = color_;
	type_ = NodeType::Selector;
	SetNodeName("Selector");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonに変換
///////////////////////////////////////////////////////////////////////////////////////////////

json SelectorNode::ToJson() {
	json item;
	item["name"] = node_.name;
	item["nodeType"] = static_cast<int>(type_);
	item["nodePos"] = json{ {"x", pos_.x}, {"y", pos_.y} };
	item["children"] = json::array();

	for (const auto& child : children_) {
		item["children"].push_back(child->ToJson());
	}
	return item;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

BehaviorStatus SelectorNode::Execute() {
	while (currentIndex_ < static_cast<uint32_t>(children_.size())) {
		BehaviorStatus status = children_[currentIndex_]->Execute();

		if (status == BehaviorStatus::Running) {
			return BehaviorStatus::Running;
		}

		if (status == BehaviorStatus::Failure) {
			++currentIndex_;
			return BehaviorStatus::Failure;
		}

		if (status == BehaviorStatus::Success) {
			currentIndex_ = 0;
			return BehaviorStatus::Success;
		}

		// 上記に当てはまらなかったら次のnodeへ
		++currentIndex_;
	}

	// 全て成功
	currentIndex_ = 0;
	return BehaviorStatus::Inactive;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float SelectorNode::EvaluateWeight() {
	if (children_.empty()) { return 0; }
	// セレクタの場合は先頭の子どもの重さを計算する
	return children_[0]->EvaluateWeight();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void SelectorNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	InputTextWithString("ReName:","##selector", node_.name);
}
