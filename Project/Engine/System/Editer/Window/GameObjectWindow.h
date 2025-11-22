#pragma once
#include <vector>
#include <functional>
#include <string>
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// GameObjectをまとめたwindow
/// </summary>
class GameObjectWindow {
public:

	GameObjectWindow();
	~GameObjectWindow();

	void Init();

	void AddAttributeGui(AttributeGui* attribute, const std::string& label);

	void InspectorWindow();

	AttributeGui* GetSelectObject() const { return selectAttribute_; }

private:

	std::string MakeUniqueName(const std::string& baseName);

private:

	std::vector<AttributeGui*> attributeArray_;
	AttributeGui* selectAttribute_ = nullptr;

};

