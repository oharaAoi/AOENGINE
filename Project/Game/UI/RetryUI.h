#pragma once

// engine
#include <Engine/Utilities/Timer.h>

enum class RetryItem {
	Pause, // 待機中
	Retry, // リトライ
	Title  // タイトルへ
};

/// <summary>
/// リトライを管理しているクラス
/// </summary>
class RetryUI {
public: // constructor

	RetryUI() = default;
	~RetryUI() = default;

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// Playerが存命していれば
	/// </summary>
	/// <param name="isPlayerAlive"></param>
	RetryItem Update(bool isPlayerAlive);

private: // private variable

	/// <summary>
	/// 次のアクションを選択する
	/// </summary>
	void SelectItem();

	/// <summary>
	/// 次の項目の決定
	/// </summary>
	/// <returns>決定ボタンが押されたかどうか</returns>
	bool DecisionItem();

	/// <summary>
	/// 現在選択中の項目を返す
	/// </summary>
	/// <returns></returns>
	RetryItem CurrentSelect();

private: // private variable

	int selectIndex_ = 0;

	AOENGINE::Timer coolTimer_;

};

