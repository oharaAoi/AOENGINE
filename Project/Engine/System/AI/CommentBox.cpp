#include "CommentBox.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Input/Input.h"
#include <cstring>

uint32_t CommentBox::nextId_ = 10000;
constexpr size_t kCommentCapacity = 512;

void CommentBox::Init(const ImVec2& _min, const ImVec2& _max, const std::string& _text) {
	id_ = nextId_;
	nextId_++;

	min_ = _min;
	max_ = _max;
	size_ = max_ - min_;

	text_ = _text;
	
	ax::NodeEditor::SetNodePosition(id_, min_);
}

void CommentBox::Update() {
	if (isSelect_) {
		if (AOENGINE::Input::GetInstance()->IsTriggerKey(DIK_DELETE)) {
			isDelete_ = true;
		}
	}
}

void CommentBox::Draw() {
	constexpr float alpha = 0.75f;

	isSelect_ = ax::NodeEditor::IsNodeSelected(id_);

	// -----------------------------
	// ノード本体（当たり判定）
	// -----------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
	ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(60, 60, 60, 100));
	ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(120, 120, 120, 120));

	ax::NodeEditor::BeginNode(id_);

	std::string guiId = "##comment" + std::to_string(id_);
	InputTextWithString("input", guiId.c_str(), text_);
	ax::NodeEditor::Group(size_);

	ax::NodeEditor::EndNode();

	if (ax::NodeEditor::BeginGroupHint(id_)) {
		auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

		auto min = ax::NodeEditor::GetGroupMin();
		
		ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
		ImGui::BeginGroup();
		ImGui::TextUnformatted(text_.c_str());
		ImGui::EndGroup();

		auto drawList = ax::NodeEditor::GetHintBackgroundDrawList();

		ImRect hintBounds(
			ImGui::GetItemRectMin(),
			ImGui::GetItemRectMax()
		);

		ImRect hintFrameBounds(
			hintBounds.Min - ImVec2(8, 4),
			hintBounds.Max + ImVec2(8, 4)
		);

		drawList->AddRectFilled(
			hintFrameBounds.Min,
			hintFrameBounds.Max,
			IM_COL32(255, 255, 255, 64 * bgAlpha / 255),
			4.0f
		);

		drawList->AddRect(
			hintFrameBounds.Min,
			hintFrameBounds.Max,
			IM_COL32(255, 255, 255, 128 * bgAlpha / 255),
			4.0f
		);
	}
	ax::NodeEditor::EndGroupHint();
	
	ax::NodeEditor::PopStyleColor(2);
	ImGui::PopStyleVar();

	ImVec2 newSize = ax::NodeEditor::GetNodeSize(id_);
	if (newSize.x != size_.x || newSize.y != size_.y) {
		size_ = newSize;
		min_ = ax::NodeEditor::GetNodePosition(id_);
		max_ = min_ + size_;
	}
}

nlohmann::json CommentBox::ToJson() {
	nlohmann::json result;
	result["id"] = id_;
	result["min"] = nlohmann::json{ {"x", min_.x}, {"y", min_.y} };
	result["max"] = nlohmann::json{ {"x", max_.x}, {"y", max_.y} };
	result["text"] = text_;
	return result;
}

void CommentBox::FromJson(const nlohmann::json& _json) {
	id_ = _json["id"];
	min_ = ImVec2(_json["min"]["x"], _json["min"]["y"]);
	max_ = ImVec2(_json["max"]["x"], _json["max"]["y"]);
	text_ = _json["text"];
	size_ = max_ - min_;
	ax::NodeEditor::SetNodePosition(id_, min_);
}

ImRect CommentBox::ExpandRect(const ImRect& r, float x, float y) {
	return ImRect(
		ImVec2(r.Min.x - x, r.Min.y - y),
		ImVec2(r.Max.x + x, r.Max.y + y)
	);
}
