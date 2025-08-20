#include "AttributeGui.h"

int AttributeGui::nextId_ = 0;

AttributeGui::AttributeGui() {
	id_ = nextId_;
	nextId_++;
}
