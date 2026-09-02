#include "CustomParameterManager.h"

#include <algorithm>

using namespace AOENGINE;

CustomParameterManager* CustomParameterManager::GetInstance() {
	static CustomParameterManager instance;
	return &instance;
}

void CustomParameterManager::Register(CustomParameterSet* parameters) {
	if (!parameters || std::find(parameterSets_.begin(), parameterSets_.end(), parameters) != parameterSets_.end()) {
		return;
	}
	parameterSets_.push_back(parameters);
}

void CustomParameterManager::Unregister(CustomParameterSet* parameters) {
	std::erase(parameterSets_, parameters);
}
