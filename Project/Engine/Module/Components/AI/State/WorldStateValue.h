#pragma once
#include <variant>
#include <string>
#include <stdexcept>

/// <summary>
/// WorldState内で使用する値
/// </summary>
struct WorldStateValue{
public:

	using Value = std::variant<
		int32_t, int32_t*,
		float, float*,
		bool, bool*,
		std::string, std::string*
	>;
	Value value_;

public:

	WorldStateValue() = default;
	WorldStateValue(Value v) : value_(v) {}
	~WorldStateValue() = default;

	const Value& Get() const { return value_; }

	template<typename T>
	T As() const {
		return std::visit([](auto&& arg) -> T {
			using U = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<U, T>) return arg;
			if constexpr (std::is_same_v<U, T*>) return *arg;

			return T();}, value_);
	}

};

