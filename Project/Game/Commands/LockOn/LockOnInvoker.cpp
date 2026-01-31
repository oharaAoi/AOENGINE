#include "LockOnInvoker.h"

void LockOnInvoker::Register(const std::string& name, CommandFactory factory) {
	commandMap_[name] = std::move(factory);
}

void LockOnInvoker::Invoke(const std::string& commandName) {
	auto it = commandMap_.find(commandName);
	if (it == commandMap_.end()) {
		return;
	}

	// コマンドを生成する
	auto command = it->second(); 
	if (command) {
		command->Execute();
	}
}
