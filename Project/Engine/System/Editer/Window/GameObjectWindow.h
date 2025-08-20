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

#ifdef _DEBUG
	void Edit();

	AttributeGui* GetSelectObject() const { return selectAttribute_; }

#endif // _DEBUG
private:

	using Pair = std::pair<std::string, AttributeGui*>;

	std::vector<Pair> attributeArray_;
	AttributeGui* selectAttribute_ = nullptr;

};

