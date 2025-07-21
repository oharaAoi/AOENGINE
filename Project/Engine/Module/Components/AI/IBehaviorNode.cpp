#include "IBehaviorNode.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/GameTimer.h"

uint32_t IBehaviorNode::nextSerialNumber_ = 1;

IBehaviorNode::IBehaviorNode() {
	Init();
}

void IBehaviorNode::Init() {
	node_.id = GetNextId();

	node_.inputId.id = GetNextId();
	node_.inputId.kind = ax::NodeEditor::PinKind::Input;

	node_.outputId.id = GetNextId();
	node_.outputId.kind = ax::NodeEditor::PinKind::Output;

	isDelete_ = false;
	currentIndex_ = 0;

	setNodePos_ = false;

	pos_ = CVector2::ZERO;

	state_ = BehaviorStatus::Inactive;
}

void IBehaviorNode::Update() {
	if (isSelect_) {
		if (Input::GetInstance()->GetKey(DIK_DELETE)) {
			if (type_ != NodeType::Root) {
				isDelete_ = true;
			}
		}
	}

	if (coolTime_ > 0.0f) {
		coolTime_ -= GameTimer::DeltaTime();
	}

	if (state_ == BehaviorStatus::Running) {
		color_ = ImColor(255, 215, 0);
	} else {
		color_ = baseColor_;
	}
}

void IBehaviorNode::DrawNode() {
	if (!setNodePos_) {
		ax::NodeEditor::SetNodePosition(node_.id, ImVec2{ pos_.x, pos_.y }); // 初期位置に配置
		setNodePos_ = true;
	}
	ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, color_);
	ax::NodeEditor::BeginNode(node_.id);
	isSelect_ = ax::NodeEditor::IsNodeSelected(node_.id);
	// 現在の描画位置
	ImVec2 textPos = ImGui::GetCursorScreenPos();
	ImGui::Text("id:%d", node_.id);
	ImGui::SameLine();
	ImGui::Text(" %s", node_.name.c_str());

	DrawImGuiLine(textPos);
	
	if (node_.inputId.id) {
		ax::NodeEditor::BeginPin(node_.inputId.id, ax::NodeEditor::PinKind::Input);
		ImGui::Text("InputId:%d", node_.inputId.id);
		ImGui::Text("In");
		ax::NodeEditor::EndPin();
	}

	if (!isLeafNode_) {
		if (node_.outputId.id) {
			ax::NodeEditor::BeginPin(node_.outputId.id, ax::NodeEditor::PinKind::Output);
			ImGui::Text("OutputId:%d", node_.outputId.id);
			ImGui::Text("Out");
			ax::NodeEditor::EndPin();
		}
	}

	ax::NodeEditor::EndNode();
	ax::NodeEditor::PopStyleColor(); // Pushした色の数だけPop

	for (auto& node : children_) {
		node->DrawNode();
	}

	ImVec2 nodePos = ax::NodeEditor::GetNodePosition(node_.id);
	pos_ = Vector2{ nodePos.x, nodePos.y };

	// 子の順番を左から順にする
	std::sort(children_.begin(), children_.end(),
			  [](IBehaviorNode* a, IBehaviorNode* b) {
				  ImVec2 posA = ax::NodeEditor::GetNodePosition(a->GetId());
				  ImVec2 posB = ax::NodeEditor::GetNodePosition(b->GetId());
				  return posA.x < posB.x; // Xが小さい方が左
			  }
	);
}

void IBehaviorNode::ResetIndex() {
	currentIndex_ = 0;
}

void IBehaviorNode::AddChild(IBehaviorNode* child) {
	children_.push_back(child);
}

void IBehaviorNode::DeleteChild(IBehaviorNode* _child) {
	for(size_t index = 0; index < children_.size(); ++index){
		if (children_[index] == _child) {
			children_.erase(children_.begin() + index);
		}
	}
}

void IBehaviorNode::ClearChild() {
	children_.clear();
}

json IBehaviorNode::ToJson() {
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

bool IBehaviorNode::IsSelectNode() {
	bool selected = ax::NodeEditor::IsNodeSelected(node_.id);
	return selected;
}

uint32_t IBehaviorNode::GetNextId() {
	uint32_t result = nextSerialNumber_;
	nextSerialNumber_++;
	return result;
}

void IBehaviorNode::DrawImGuiLine(const ImVec2& _texPos) {
	// 各テキストを描画
	std::string idText = "id : ";
	std::string nameText = " " + node_.name;

	// テキストの高さ（線のy位置調整用）
	float textHeight = ImGui::GetTextLineHeight();

	// テキスト幅を合計（線の長さに使う）
	ImVec2 size1 = ImGui::CalcTextSize(idText.c_str());
	ImVec2 size2 = ImGui::CalcTextSize(nameText.c_str());
	float lineWidth = size1.x + size2.x;

	// 線の位置と長さ
	ImVec2 lineStart = ImVec2(_texPos.x, _texPos.y + textHeight + 2.0f);
	ImVec2 lineEnd = ImVec2(_texPos.x + lineWidth, lineStart.y);

	// 線を描画
	ImGui::GetWindowDrawList()->AddLine(
		lineStart,
		lineEnd,
		IM_COL32(255, 255, 255, 255), // 色：白
		1.0f                          // 太さ
	);
}
