#pragma once

#include <vector>

namespace AOENGINE {

class CustomParameterSet;

class CustomParameterManager {
public:
	static CustomParameterManager* GetInstance();

	void Register(CustomParameterSet* parameters);
	void Unregister(CustomParameterSet* parameters);
	const std::vector<CustomParameterSet*>& GetParameterSets() const { return parameterSets_; }

private:
	std::vector<CustomParameterSet*> parameterSets_;
};

}
