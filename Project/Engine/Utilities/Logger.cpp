#include "Logger.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cassert>

std::string Logger::filePath_;

Logger::~Logger() {
	std::ofstream logStream(filePath_, std::ios::app);
	logStream << "FINISHED LOG" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Logger::Init() {
	std::filesystem::create_directory("Logs");
	// Logの数を制限する
	DeleteOldLogFile(10);

	// 現在時刻を取得
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	// ログ・ファイルの名前にコンマ秒はいらないので削る
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> nowSeconds 
		= std::chrono::time_point_cast<std::chrono::seconds>(now);
	// 日本時間に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSeconds };
	// 年月日の文字列に変更
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	// 時刻を使ってファイル名を決定
	filePath_ = std::string("Logs/") + dateString + ".log";
	// ファイルを作って書き込み準備
	std::ofstream logStream(filePath_);

	logStream << "SUCCESS CREATE LOG" << std::endl;

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Log出力
///////////////////////////////////////////////////////////////////////////////////////////////

void Logger::Log(const std::string& message) {
	std::ofstream logStream(filePath_, std::ios::app);
	logStream << message << std::endl;
	OutputDebugStringA(message.c_str());
}

void Logger::AssertLog(const std::string& message) {
	std::ofstream logStream(filePath_, std::ios::app);
	logStream << message << std::endl;
	OutputDebugStringA(message.c_str());
	assert(message.c_str());
}

void Logger::CommentLog(const std::string& message) {
	Log("++++++++++++++++++++++++++++\n");
	Log(message + "\n");
	Log("++++++++++++++++++++++++++++\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Logがたくさん残らないように数を制限する
///////////////////////////////////////////////////////////////////////////////////////////////

void Logger::DeleteOldLogFile(size_t max) {
    std::vector<std::filesystem::directory_entry> files;

    // フォルダ内の通常ファイルを収集
    for (const auto& entry : std::filesystem::directory_iterator("Logs/")) {
        if (std::filesystem::is_regular_file(entry)) {
            files.push_back(entry);
        }
    }

    // ファイル数が上限を超えていなければ何もしない
    if (files.size() <= max)
        return;

    // 更新日時でソート（古い順）
    std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
        return std::filesystem::last_write_time(a) < std::filesystem::last_write_time(b);
              });

    // 古いファイルから削除
    for (size_t i = 0; i < files.size() - max; ++i) {
        std::error_code ec;
        std::filesystem::remove(files[i], ec);
    }
}
