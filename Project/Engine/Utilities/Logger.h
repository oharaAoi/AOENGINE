#pragma once
#include <string>
#include "Engine/Utilities/Convert.h"

namespace AOENGINE {

/// <summary>
/// Logを出すクラス
/// </summary>
class Logger {
public:

	Logger() = default;
	~Logger();

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// Logを出す
	/// </summary>
	/// <param name="message">: メッセージ</param>
	static void Log(const std::string& message);

	/// <summary>
	/// 警告も含めたlogを出す
	/// </summary>
	/// <param name="message">: メッセージ</param>
	static void AssertLog(const std::string& message);

	/// <summary>
	/// コメントアウトで囲んだlogを出す
	/// </summary>
	/// <param name="message">: メッセージ</param>
	static void CommentLog(const std::string& message);

	/// <summary>
	/// 古いlogファイルを消す
	/// </summary>
	/// <param name="max"></param>
	void DeleteOldLogFile(size_t max);

private:

	static std::string filePath_;

};

}