#pragma once
#include <cstddef>
#include <string>

class TutorialContext;

/// <summary>
/// チュートリアルの1ページぶんの基底クラス
/// </summary>
class BaseTutorialStep {
public:
	virtual ~BaseTutorialStep() = default;

	// ページの開始、終わり
	virtual void Enter(TutorialContext& context) {}
	virtual void Exit(TutorialContext& context) {}

	// 更新
	virtual void Update(TutorialContext& context, float deltaTime) = 0;

	// このページが終わったかどうか
	virtual bool IsFinished() const = 0;

	// ページ名。デバッグ表示に使う
	virtual const std::string& GetName() const = 0;

	/// <summary>
	/// 入力で次のページへ送れるページか
	/// </summary>
	virtual bool CanSkipByInput() const { return true; }

	/// <summary>このページで出す説明文のキー。テキストの差し替えに使う</summary>
	virtual const std::string& GetTextKey() const { return GetName(); }

	/// <summary>
	/// このページが持つチェックの数
	/// </summary>
	virtual std::size_t GetCheckCount() const { return 1; }

	/// <summary>
	/// 番号で指定したチェックの行動を、実際に成し遂げたか
	/// </summary>
	virtual bool IsCleared(std::size_t index) const { (void)index; return false; }
};
