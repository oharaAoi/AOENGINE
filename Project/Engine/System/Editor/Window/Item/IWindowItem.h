#pragma once
#include <string>

namespace AOENGINE {

/// <summary>
/// Windowの項目として表示する際に継承するクラス
/// </summary>
class IWindowItem {
public: // constructor

	IWindowItem() = default;
	virtual ~IWindowItem() = default;

public: // public method

	virtual void Init() = 0;

	virtual void Edit() = 0;

public: // accessor method

	const std::string& GetName() const { return name_; }
	bool& GetIsActive() { return isActive_; }

protected: // protected method

	std::string name_ = "";
	bool isActive_ = false;

};
};
