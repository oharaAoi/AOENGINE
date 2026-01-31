#pragma once
// c++
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
// game
#include "Game/Commands/LockOn/LockOnCommands.h"

/// <summary>
/// 登録してあるコマンドを呼び出す処理を行えるクラス
/// </summary>
class LockOnInvoker {
public:

	using CommandFactory = std::function<std::unique_ptr<ILockOnCommand>()>;

public: // コンストラクタ

	LockOnInvoker() = default;
	~LockOnInvoker() = default;

public: // method

	/// <summary>
	/// コマンド生成登録
	/// </summary>
	/// <param name="name"></param>
	/// <param name="factory"></param>
	void Register(const std::string& name, CommandFactory factory);

	/// <summary>
	/// 実行処理
	/// </summary>
	/// <param name="commandName"></param>
	void Invoke(const std::string& commandName);

	template<class T, class... Args>
	CommandFactory MakeFactory(Args&&... args) {
		return [=]() {
			return std::make_unique<T>(args...);
			};
	}

private:

	std::unordered_map<std::string, std::function<std::unique_ptr<ILockOnCommand>()>> commandMap_;

};

