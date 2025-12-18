#include "CommentBox.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Input/Input.h"

uint32_t CommentBox::nextId_ = 1;

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
		if (AOENGINE::Input::GetInstance()->GetKey(DIK_DELETE)) {
			isDelete_ = true;
		}
	}
}

void CommentBox::Draw() {
	using namespace ax::NodeEditor;

	constexpr float alpha = 0.75f;

	isSelect_ = ax::NodeEditor::IsNodeSelected(id_);

	// -----------------------------
	// ノード本体（当たり判定）
	// -----------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
	PushStyleColor(StyleColor_NodeBg, ImColor(60, 60, 60, 100));
	PushStyleColor(StyleColor_NodeBorder, ImColor(120, 120, 120, 120));

	BeginNode(id_);


	{
		ImGui::BeginGroup();

		float textWidth = ImGui::CalcTextSize(text_.c_str()).x;
		float avail = ImGui::GetContentRegionAvail().x;

		if (avail > textWidth)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - textWidth) * 0.5f);

		std::string guiId = "##comment" + std::to_string(id_);
		InputTextWithString(" ", guiId.c_str(), text_);
		ImGui::EndGroup();
	}


	Group(size_);

	EndNode();

	PopStyleColor(2);
	ImGui::PopStyleVar();

	ImVec2 newSize = GetNodeSize(id_);
	if (newSize.x != size_.x || newSize.y != size_.y) {
		size_ = newSize;
		min_ = GetNodePosition(id_);
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