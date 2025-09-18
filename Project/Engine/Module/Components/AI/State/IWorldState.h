#pragma once
#include <any>
#include <unordered_map>
#include <string>

class IWorldState {
public:
	IWorldState() = default;
	virtual ~IWorldState() = default;

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
