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

void AOENGINE::AttributeGui::AddChild(AOENGINE::AttributeGui* child) {
	int count = 0;
	std::string newName = child->GetName();

	auto isDuplicate = [&](const std::string& name) {
		for (auto* attr : children_) {
			if (attr->GetName() == name) return true;
		}
		return false;
		};

	while (isDuplicate(newName)) {
		++count;
		newName = newName + "(" + std::to_string(count) + ")";
	}

	child->SetName(newName);
	children_.push_back(child);
}

void AOENGINE::AttributeGui::DeleteChild(AOENGINE::AttributeGui* child) {
	children_.erase(
		std::remove_if(children_.begin(), children_.end(),
					   [child](AOENGINE::AttributeGui* c) { return c == child; }),
		children_.end()
	);
}
