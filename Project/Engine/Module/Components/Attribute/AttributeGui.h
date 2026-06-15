#pragma once
#include <string>
#include <vector>
#include "Engine/System/Manager/ImGuiManager.h"

namespace AOENGINE {

/// <summary>
/// ImGui描画をサポートするためのクラス
/// </summary>
class AttributeGui {
public:	// constructor

	AttributeGui();
	virtual ~AttributeGui() { children_.clear(); };


public: // public method

	virtual void Debug_Gui() = 0;

	/// <summary>
	/// 名前の編集
	/// </summary>
	void EditName();

	/// <summary>
	/// 子供の追加
	/// </summary>
	/// <param name="child"></param>
	void AddChild(AOENGINE::AttributeGui* child);

	/// <summary>
	/// 子どもの削除
	/// </summary>
	/// <param name="child"></param>
	void DeleteChild(AOENGINE::AttributeGui* child);

	/// <summary>
	/// 子どもの一斉削除
	/// </summary>
	void ClearChild() { children_.clear(); }

	/// <summary>
	/// 子を所有しているか
	/// </summary>
	/// <returns></returns>
	bool HasChild() const { return !children_.empty() ? true : false; }

public: // accessor

	// name
	void SetName(const std::string& name) { name_ = name; }
	const std::string& GetName() const { return name_; }

	const std::vector<AOENGINE::AttributeGui*>& GetChildren() const { return children_; }


	void SetIsActive(bool isActive) { isActive_ = isActive; }
	bool GetIsActive() const { return isActive_; }

	int GetID() const { return id_; }

protected:

	bool isActive_ = true;

	std::string name_ = "new Attribute";

private:

	std::vector<AOENGINE::AttributeGui*> children_;

	int id_;
	static int nextId_;
};

}