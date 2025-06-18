#pragma once
#include <any>
#include <unordered_map>
#include <string>

template<typename T>
struct ActionValue {
	T value;
};

struct ActionContext {
	std::unordered_map<std::string, std::any> values;

	template<typename T>
	void Set(const std::string& key, const T& v) {
		values[key] = ActionValue<T>{ v };
	}

	template<typename T>
	T Get(const std::string& key) const {
		const auto& val = std::any_cast<ActionValue<T>>(values.at(key));
		return val.value;
	}
};