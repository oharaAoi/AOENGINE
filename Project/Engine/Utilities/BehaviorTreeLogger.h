#pragma once
#include <string>

/// <summary>
/// BehaviorTreeのLogを作るクラス
/// </summary>
class BehaviorTreeLogger {
public:

	BehaviorTreeLogger() = default;
	~BehaviorTreeLogger();

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_fileName">: ファイル名</param>
	void Init(const std::string& _fileName);

	/// <summary>
	/// Logを出力する
	/// </summary>
	/// <param name="message"></param>
	void Log(const std::string& _message);

	/// <summary>
	/// 警告も含めたlogを出す
	/// </summary>
	/// <param name="message">: メッセージ</param>
	void AssertLog(const std::string& _message);

	/// <summary>
	/// 古いlogファイルを消す
	/// </summary>
	/// <param name="max"></param>
	void DeleteOldLogFile(size_t max);

private:

	std::string filePath_;

};

