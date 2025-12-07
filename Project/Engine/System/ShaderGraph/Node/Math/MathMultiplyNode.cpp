#include "MathMultiplyNode.h"

using namespace AOENGINE;

void MathMultiplyNode::Init() {
	addIN<float>("A", a_, ImFlow::ConnectionFilter::SameType());
	addIN<float>("B", b_, ImFlow::ConnectionFilter::SameType());

	auto texOut = addOUT<float>("output", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return value_; });
}

void MathMultiplyNode::customUpdate() {
	value_ = a_ * b_;
}

void MathMultiplyNode::draw() {
	a_ = getInVal<float>("A");
	b_ = getInVal<float>("B");
}

nlohmann::json MathMultiplyNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	return result;
}

void MathMultiplyNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
}
