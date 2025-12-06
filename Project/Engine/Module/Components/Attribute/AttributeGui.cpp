#include "AttributeGui.h"

using namespace AOENGINE;

int AOENGINE::AttributeGui::nextId_ = 0;

AOENGINE::AttributeGui::AttributeGui() {
	id_ = nextId_;
	nextId_++;
}

void AOENGINE::AttributeGui::EditName() {
	char buffer[256];
	std::snprintf(buffer, sizeof(buffer), "%s", name_.c_str());

	if (ImGui::InputText("Name :", buffer, sizeof(buffer))) {
		name_ = buffer;
	}
}
