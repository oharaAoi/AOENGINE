#pragma once

// engine
#include <Engine/Utilities/Timer.h>

/// <summary>
/// タイトルの選択を管理しているクラス
/// </summary>
class TitleUI {
public: // data

	enum class TitleItem {
		Pause, // 待機中
		Start, // リトライ
		Exit   // タイトルへ
	};

public: // constructor

	TitleUI() = default;
	~TitleUI() = default;

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// Playerが存命していれば
	/// </summary>
	/// <param name="isPlayerAlive"></param>
	TitleItem Update();

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
	TitleItem CurrentSelect();

private: // private variable

	int selectIndex_ = 0;

	AOENGINE::Timer coolTimer_;

};

