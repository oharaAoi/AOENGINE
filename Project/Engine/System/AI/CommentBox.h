#pragma once
#include "Engine/Lib/Math/MyMath.h"

#include <string>
#include <vector>
#include <json.hpp>
#include <Engine/System/Manager/ImGuiManager.h>

class CommentBox {
public:

	CommentBox() = default;
	~CommentBox() = default;

public:

	void Init(const ImVec2& _min, const ImVec2& _max, const std::string& _text = "Comment Box Text");

	void Update();

	void Draw();

	nlohmann::json ToJson();

	void FromJson(const nlohmann::json& _json);

	bool GetIsSelect() const { return isSelect_; }
	bool GetIsDelete() const { return isDelete_; }

private:

	ImRect ExpandRect(const ImRect& r, float x, float y);

private:

	uint32_t id_;
	static uint32_t nextId_;

	ImVec2 min_;
	ImVec2 max_;
	ImVec2 size_;

	std::vector<char> commentBuffer_;
	std::string text_ = "Comment Box Text";
	ImU32 bgColor_ = IM_COL32(255, 255, 128, 128);

	bool dragging_ = false;
	bool resizing_ = false;

	bool isSelect_ = false;
	bool isDelete_ = false;

	ImVec2 prevPos_;
	ImVec2 screenPos;
};

