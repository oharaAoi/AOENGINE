#pragma once
#include <string>
#include <vector>
#include "Engine/System/Manager/ImGuiManager.h"

namespace AOENGINE {

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

	// 名前の編集
	void EditName();

	// children 
	void AddChild(AOENGINE::AttributeGui* child) { children_.emplace_back(child); }
	void DeleteChild(AOENGINE::AttributeGui* child) {
		children_.erase(
			std::remove_if(children_.begin(), children_.end(),
						   [child](AOENGINE::AttributeGui* c) { return c == child; }),
			children_.end()
		);
	}
	void ClearChild() { children_.clear(); }

	const std::vector<AOENGINE::AttributeGui*>& GetChildren() const { return children_; }

	bool HasChild() const { return !children_.empty() ? true : false; }

	void SetIsActive(bool isActive) { isActive_ = isActive; }
	bool GetIsActive() const { return isActive_; }

protected:

	bool isActive_ = true;

	std::string name_ = "new Attribute";

private:

	std::vector<AOENGINE::AttributeGui*> children_;

	int id_;
	static int nextId_;
};

}