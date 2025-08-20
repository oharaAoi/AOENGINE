#pragma once
#include <string>
#include <vector>
#include "Engine/System/Manager/ImGuiManager.h"

/// <summary>
/// ImGui描画をサポートするためのクラス
/// </summary>
class AttributeGui {
public:	// member method

	AttributeGui();
	virtual ~AttributeGui() { children_.clear(); };

	virtual void Debug_Gui() = 0;

public: 

	// name
	void SetName(const std::string& name) { name_ = name; }
	const std::string& GetName() const { return name_; }

	// children 
	void AddChild(AttributeGui* child) { children_.emplace_back(child); }
	void DeleteChild(AttributeGui* child) {
		children_.erase(
			std::remove_if(children_.begin(), children_.end(),
						   [child](AttributeGui* c) { return c == child; }),
			children_.end()
		);
	}

	const std::vector<AttributeGui*>& GetChildren() const { return children_; }

	bool HasChild() const { return !children_.empty() ? true : false; }

	void SetIsActive(bool isActive) { isActive_ = isActive; }
	bool GetIsActive() const { return isActive_; }

protected:

	bool isActive_ = true;

	std::string name_ = "new Attribute";

private:

	std::vector<AttributeGui*> children_;

	int id_;
	static int nextId_;
};

