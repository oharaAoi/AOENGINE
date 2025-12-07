#include "MathAddNode.h"

using namespace AOENGINE;

void MathAddNode::Init() {
	addIN<float>("A", a_, ImFlow::ConnectionFilter::SameType());
	addIN<float>("B", b_, ImFlow::ConnectionFilter::SameType());

	auto texOut = addOUT<float>("output", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return value_; });
}

void MathAddNode::customUpdate() {
	value_ = a_ + b_;
}

void MathAddNode::draw() {
	a_ = getInVal<float>("A");
	b_ = getInVal<float>("B");
}

nlohmann::json MathAddNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	return result;
}

void MathAddNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
}
