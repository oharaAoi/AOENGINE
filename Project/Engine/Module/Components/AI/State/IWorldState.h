#pragma once
#include <any>
#include <unordered_map>
#include <string>

/// <summary>
/// Nodeごとに情報を共有するためのクラス
/// </summary>
class IWorldState {
public: // コンストラクタ

	IWorldState() = default;
	virtual ~IWorldState() = default;

public:

	template<typename T>
	void Set(const std::string& key, const T& value) {
		stateMap_[key] = static_cast<T>(value);
	}

	template<typename T>
	T Get(const std::string& key) const {
		return std::any_cast<T>(stateMap_.at(key));
	}

protected:
	std::unordered_map<std::string, std::any> stateMap_;
};
