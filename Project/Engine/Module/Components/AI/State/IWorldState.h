#pragma once
#include <any>
#include <unordered_map>
#include <string>
#include <variant>
#include <stdexcept>
#include <vector>
// engine
#include "Engine/Module/Components/AI/State/WorldStateValue.h"

/// <summary>
/// Nodeごとに情報を共有するためのクラス
/// </summary>
class IWorldState {
public: // コンストラクタ

	IWorldState() = default;
	virtual ~IWorldState() = default;

public:

	template<typename T>
	void SetRef(const std::string& key, T& ref) {
        stateMap_[key] = WorldStateValue(&ref);
	}

	template<typename T>
	void Set(const std::string& key, const T& value) {
		stateMap_[key] = WorldStateValue(value);
	}

	WorldStateValue Get(const std::string& key) const {
		return stateMap_.at(key).Get();
	}

	void Debug_Gui();

	void KeyCombo(std::string& _key, int32_t& index, const std::string& _label);

private:

	void CreateValue();

protected:
	std::unordered_map<std::string, WorldStateValue> stateMap_;
};
    