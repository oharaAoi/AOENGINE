#include "UVTransformNode.h"

void UVTransformNode::Init() {
	// inputの設定
	addIN<Vector2>("inputUV", inputUV_, ImFlow::ConnectionFilter::SameType());
	addIN<Vector2>("scale", scale_, ImFlow::ConnectionFilter::SameType());
	addIN<Vector2>("offset", offset_, ImFlow::ConnectionFilter::SameType());
	addIN<float>("rotate", rotate_, ImFlow::ConnectionFilter::SameType());

	// outputの設定
	auto texOut = addOUT<NodeUVTransform>("uv", ImFlow::PinStyle::cyan());
	texOut->behaviour([this]() { return outputUV_; });
}

void UVTransformNode::customUpdate() {
	inputUV_ = getInVal<Vector2>("inputUV");
	scale_ = getInVal<Vector2>("scale");
	offset_ = getInVal<Vector2>("offset");
	rotate_ = getInVal<float>("rotate");

	outputUV_.scale = scale_;
	outputUV_.rotate = rotate_;
	outputUV_.translate = offset_;
}

void UVTransformNode::draw() {
}

nlohmann::json UVTransformNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	return result;
}

void UVTransformNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
}
