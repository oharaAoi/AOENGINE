#include "MathAddNode.h"

void MathAddNode::Init() {
	addIN<float>("A", a_, ImFlow::ConnectionFilter::SameType());
	addIN<float>("B", b_, ImFlow::ConnectionFilter::SameType());
}

void MathAddNode::customUpdate() {
	value_ = a_ + b_;
}

void MathAddNode::draw() {
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
