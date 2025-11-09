#include "MathMultiplyNode.h"

void MathMultiplyNode::Init() {
	addIN<float>("A", a_, ImFlow::ConnectionFilter::SameType());
	addIN<float>("B", b_, ImFlow::ConnectionFilter::SameType());
}

void MathMultiplyNode::customUpdate() {
	value_ = a_ * b_;
}

void MathMultiplyNode::draw() {
	a_ = getInVal<float>("A");
	b_ = getInVal<float>("B");

	showOUT<float>(
		"output",
		[=]() -> float {
			return value_;
		},
		ImFlow::PinStyle::green()
	);
}
