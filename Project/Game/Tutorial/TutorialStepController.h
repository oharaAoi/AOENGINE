#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "Game/Tutorial/Step/BaseTutorialStep.h"
#include "Game/Tutorial/TutorialContext.h"
#include "Game/Tutorial/TutorialStepParameter.h"

/// <summary>
/// チュートリアルのページを順に進める管理クラス
/// </summary>
class TutorialStepController {
public:

	// ページの並び。増やしたら kStepCount も合わせる
	enum class StepKind {
		Move,			// 移動とジャンプ
		DamageFloor,	// ダメージ床
		Connect,		// ブロックをつなぐ
		Launch,			// 打ち上げ。ボスに当てたら次へ
		Ending,			// タイトルに戻る
	};
	static constexpr std::size_t kStepCount = 5;

	TutorialStepController() = default;
	~TutorialStepController() = default;

	/// <summary>最初のページをセットする</summary>
	void Init(TutorialContext& context);

	/// <summary>今のページを進める。ページの切り替えはここでは行わない</summary>
	void Update(TutorialContext& context, float deltaTime);

	/// <summary>
	/// 今のページが次へ進める状態か。進めはしない
	/// </summary>
	bool WantsAdvance(const TutorialContext& context) const;

	/// <summary>次のページへ進める。最後のページだったら全部終わりにする</summary>
	void Advance(TutorialContext& context);

	/// <summary>今のページから前へ戻れる状態か。戻りはしない</summary>
	bool WantsBack(const TutorialContext& context) const;

	/// <summary>前のページへ戻る</summary>
	void Back(TutorialContext& context);

	/// <summary>指定したページへ切り替える。デバッグからの飛ばしにも使う</summary>
	void ChangeStep(TutorialContext& context, StepKind kind);

	/// <summary>今のページの表示と、確認用の強制切り替え</summary>
	void Debug_Gui(TutorialContext& context);

private:

	static std::size_t ToIndex(StepKind kind) { return static_cast<std::size_t>(kind); }

	/// <summary>種類からページの実体を作る</summary>
	std::unique_ptr<BaseTutorialStep> CreateStep(StepKind kind) const;

	/// <summary>今のページの次にあたる種類を返す</summary>
	StepKind GetNextKind(StepKind kind) const;

	/// <summary>今のページの前にあたる種類を返す</summary>
	StepKind GetPrevKind(StepKind kind) const;

	/// <summary>ページを差し替える</summary>
	void SetStep(TutorialContext& context, std::unique_ptr<BaseTutorialStep> next);

private:

	// 今のページ
	std::unique_ptr<BaseTutorialStep> currentStep_;
	StepKind currentKind_ = StepKind::Move;

	// 最後のページまで終わったか
	bool isAllFinished_ = false;

	// ページを送る間合いの調整値。各ページへは context 経由で渡す
	TutorialStepParameter parameter_;

	// ページが無い時に返す名前
	const std::string kNoneName_ = "None";

public: // accessor

	/// <summary>最後のページまで終わったか。シーンを移す合図に使う</summary>
	bool IsAllFinished() const { return isAllFinished_; }

	StepKind GetCurrentKind() const { return currentKind_; }
	const std::string& GetCurrentName() const;

	/// <summary>今のページが入力で送れるか。ボタン表示の出し分けに使う</summary>
	bool CanSkipByInput() const;

	/// <summary>前のページがあるか。戻るの案内を出すかの判断に使う</summary>
	bool CanBack() const { return currentKind_ != StepKind::Move; }

	/// <summary>今のページの説明文を引くためのキー</summary>
	const std::string& GetCurrentTextKey() const;

	/// <summary>今のページが持つチェックの数</summary>
	std::size_t GetCurrentCheckCount() const;

	/// <summary>今のページの、番号で指定した行動ができたか。チェックの表示に使う</summary>
	bool IsCurrentCleared(std::size_t index) const;

	/// <summary>今のページの番号。チェックの置き場所を引くのに使う</summary>
	std::size_t GetCurrentIndex() const { return ToIndex(currentKind_); }
};
