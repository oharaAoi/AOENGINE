#include "LoopValueNode.h"
#include "Engine/Lib/GameTimer.h"

AOENGINE::LoopValueNode::LoopValueNode() {
	output_ = 0;
	start_ = 0;
	end_ = 0;
	duration_ = 0;
	easeIndex_ = 0;
	loopType_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::LoopValueNode::Init() {
	timer_.Init(start_, end_, duration_, easeIndex_, static_cast<LoopType>(loopType_));

	auto texOut = addOUT<float>("output", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return output_; });
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新関数
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::LoopValueNode::customUpdate() {
	timer_.Update(GameTimer::DeltaTime());

	output_ = timer_.GetValue();
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集項目
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::LoopValueNode::updateGui() {
	ImGui::DragFloat("start", &start_, 0.01f);
	ImGui::DragFloat("end", &end_, 0.01f);
	ImGui::DragFloat("duration", &duration_, 0.01f);
	Math::SelectEasing(easeIndex_);
	ImGui::Combo("emitOrigin##emitOrigin", &loopType_, "Loop\0Stop\0Return\0RoundTrip");
	if (ImGui::Button("Reset")) {
		timer_.Init(start_, end_, duration_, easeIndex_, static_cast<LoopType>(loopType_));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ node描画
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::LoopValueNode::draw() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ json変換
///////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json AOENGINE::LoopValueNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	result["props"]["start"] = Convert::toJson<float>(start_);
	result["props"]["end"] = Convert::toJson<float>(end_);
	result["props"]["duration"] = Convert::toJson<float>(duration_);
	result["props"]["easeIndex"] = Convert::toJson<int>(easeIndex_);
	result["props"]["loopType"] = Convert::toJson<int>(loopType_);
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonから変換
///////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::LoopValueNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
	start_ = _json["props"]["start"].get<float>();
	end_ = _json["props"]["end"].get<float>();
	duration_ = _json["props"]["duration"].get<float>();
	easeIndex_ = _json["props"]["easeIndex"].get<int>();
	loopType_ = _json["props"]["loopType"].get<int>();

	// 初期化もしておく
	timer_.Init(start_, end_, duration_, easeIndex_, static_cast<LoopType>(loopType_));
}
