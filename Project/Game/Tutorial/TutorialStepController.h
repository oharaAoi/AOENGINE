#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "Game/Tutorial/Step/BaseTutorialStep.h"
#include "Game/Tutorial/TutorialContext.h"

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

	/// <summary>今のページを進め、終わったら次のページへ切り替える</summary>
	void Update(TutorialContext& context, float deltaTime);

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

	/// <summary>ページを差し替える</summary>
	void SetStep(TutorialContext& context, std::unique_ptr<BaseTutorialStep> next);

private:

	// 今のページ
	std::unique_ptr<BaseTutorialStep> currentStep_;
	StepKind currentKind_ = StepKind::Move;

	// 最後のページまで終わったか
	bool isAllFinished_ = false;

	// ページが無い時に返す名前
	const std::string kNoneName_ = "None";

public: // accessor

	/// <summary>最後のページまで終わったか。シーンを移す合図に使う</summary>
	bool IsAllFinished() const { return isAllFinished_; }

	StepKind GetCurrentKind() const { return currentKind_; }
	const std::string& GetCurrentName() const;

	/// <summary>今のページが入力で送れるか。ボタン表示の出し分けに使う</summary>
	bool CanSkipByInput() const;
};
