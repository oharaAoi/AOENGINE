#pragma once
// c++
#include <vector>
#include <string>
// engine
#include "Engine/System/Editor/Window/Item/IWindowItem.h"

namespace AOENGINE {

/// <summary>
/// Colliderに設定するWindowの設定を行う
/// </summary>
class ColliderCategorySettingWindow :
	public IWindowItem {
public: // constructor

	ColliderCategorySettingWindow();
	~ColliderCategorySettingWindow() override = default;

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init() override;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Edit() override;

	/// <summary>
	/// 保存処理
	/// </summary>
	/// <param name="path"></param>
	void Save(const std::string& path);

private: // private method

	std::vector<std::string> categoties_;

};

}
