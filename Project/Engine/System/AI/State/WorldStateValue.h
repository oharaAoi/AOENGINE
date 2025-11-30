#pragma once
#include <variant>
#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp>

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

	void DebugValue(const std::string& name, WorldStateValue& wv);

	const Value& Get() const { return value_; }

	template<typename T>
	T As() const {
		T result{};

		std::visit([&](auto&& arg) {
			using U = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<U, T>) {
				result = arg;
			} else if constexpr (std::is_same_v<U, T*>) {
				result = *arg;
			}
				   }, value_);

		return result;
	}

	nlohmann::json to_json(const WorldStateValue& v) const;

	void from_json(const nlohmann::json& j, WorldStateValue& v);

};

