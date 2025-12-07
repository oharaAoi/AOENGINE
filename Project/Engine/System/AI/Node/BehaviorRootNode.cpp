#include "BehaviorRootNode.h"
#include <utility>

using namespace AI;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コンストラクタ
///////////////////////////////////////////////////////////////////////////////////////////////

BehaviorRootNode::BehaviorRootNode() {
	color_ = ImColor(0, 191, 255);
	baseColor_ = color_;
	type_ = NodeType::Root;
	SetNodeName("Root");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonに変換
///////////////////////////////////////////////////////////////////////////////////////////////

json BehaviorRootNode::ToJson() {
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

BehaviorStatus BehaviorRootNode::Execute() {
	// 要素が1以上にならないようにする
	if (children_.size() > 1) {
		children_.erase(children_.begin());
	}

	currentRunNodeName_ = RunNodeName();

	// nodeを実行する
	while (currentIndex_ < static_cast<uint32_t>(children_.size())) {
		BehaviorStatus status = children_[currentIndex_]->Execute();

		// 走っているなら
		if (status == BehaviorStatus::Running)
			return BehaviorStatus::Running;

		// 成功したら
		if (status == BehaviorStatus::Success) {
			currentIndex_ = 0;
			return BehaviorStatus::Success;
		}

		// 失敗したなら
		if (status == BehaviorStatus::Failure) {
			++currentIndex_;
			continue;
		}
	}

	// 全て成功
	currentIndex_ = 0;
	return BehaviorStatus::Success;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorRootNode::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
}

std::string BehaviorRootNode::RunNodeName() {
	if (children_.empty()) {
		return this->GetName();
	} else {
		return children_[currentIndex_]->RunNodeName();
	}
}