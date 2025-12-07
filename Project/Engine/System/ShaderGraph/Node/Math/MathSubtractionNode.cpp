#include "MathSubtractionNode.h"

using namespace AOENGINE;

void MathSubtractionNode::Init() {
	addIN<float>("A", a_, ImFlow::ConnectionFilter::SameType());
	addIN<float>("B", b_, ImFlow::ConnectionFilter::SameType());

	auto texOut = addOUT<float>("output", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return value_; });
}

void MathSubtractionNode::customUpdate() {
	value_ = a_ - b_;
}

void MathSubtractionNode::draw() {
	a_ = getInVal<float>("A");
	b_ = getInVal<float>("B");
}

nlohmann::json MathSubtractionNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	return result;
}

void MathSubtractionNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
}
