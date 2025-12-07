#pragma once
#include <vector>
#include <functional>
#include <string>
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// GameObjectをまとめたwindow
/// </summary>
class GameObjectWindow {
public:

	GameObjectWindow();
	~GameObjectWindow();

	void Init();

	void AddAttributeGui(AOENGINE::AttributeGui* attribute, const std::string& label);

	void InspectorWindow();

	AOENGINE::AttributeGui* GetSelectObject() const { return selectAttribute_; }

private:

	std::string MakeUniqueName(const std::string& baseName);

private:

	std::vector<AOENGINE::AttributeGui*> attributeArray_;
	AOENGINE::AttributeGui* selectAttribute_ = nullptr;

};

}