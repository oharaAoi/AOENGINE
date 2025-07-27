#pragma once
#include <string>
#include "Engine/Utilities/Convert.h"

class Logger {
public:

	Logger() = default;
	~Logger();

	void Init();

	static void Log(const std::string& message);

	static void AssertLog(const std::string& message);

	static void CommentLog(const std::string& message);

	void DeleteOldLogFile(size_t max);

private:

	static std::string filePath_;

};