#pragma once
#include <array>
#include <cstddef>
#include <functional>
#include <string>

#include "Engine/Lib/Math/Vector2.h"
#include "Game/UI/IntroUIParameter.h"

namespace AOENGINE {
	class Sprite;
	class Text;
}

/// <summary>
/// ゲームが始まる前のカウントダウン演出
/// </summary>
class IntroUI {
public:

	IntroUI();
	~IntroUI() = default;

	// 初期化、更新
	void Init();
	void Update(float deltaTime);

	//　イントロ演出開始
	void Start();

	// パラメータ編集
	void Debug_Gui();

private:

	// 演出全体の流れ
	enum class Phase {
		ObjectiveWait,	// 目的を出すまでの待ち
		ObjectiveIn,	// 目的が出てくる
		ObjectiveHold,	// 目的を見せている
		ObjectiveOut,	// 目的を戻す
		Countdown,		// 3 / 2 / 1 / Go
		Done,			// 終わった
	};
	// 流れの数
	static constexpr std::size_t kPhaseCount = 6;

	// 1文字ぶんの進み方
	enum class ItemState {
		Waiting,	// まだ出ていない
		MoveIn,		// 右から中心へ
		MoveOut,	// 中心から左へ。Goは通らない
		Hold,		// 中心で止まっている。Goだけ通る
		Fade,		// 薄くなって消える。Goだけ通る
		Done,		// 終わった
	};
	// 状態の数
	static constexpr std::size_t kItemStateCount = 6;

	// 1文字ぶんの中身
	struct Item {
		AOENGINE::Text* text = nullptr;
		ItemState state = ItemState::Waiting;
		float timer = 0.0f;	// 今の状態になってからの経過時間
		// 出てきてからの経過時間。状態が変わっても数え続ける。
		// 振りは移動とは別の時間で動かすので、こちらを見る
		float lifeTime = 0.0f;
	};

	// テキストと、スプライトの作成
	AOENGINE::Sprite* ResolveSprite(const std::string& name, const std::string& textureName, int renderQueue);
	AOENGINE::Text* ResolveText(const std::string& name, int renderQueue);

	// フェーズとStateをIndexとして取得
	std::size_t ToIndex(ItemState state) const { return static_cast<std::size_t>(state); }
	std::size_t ToIndex(Phase phase) const { return static_cast<std::size_t>(phase); }

	// フェーズ、状態切り替え
	void ChangePhase(Phase next);
	void ChangeState(Item& item, ItemState next) const;

	/// <summary>出ていない状態へ戻す。数えている時間も全部消す</summary>
	void ResetItem(Item& item) const;

	/// <summary>目的の絵を、進み具合に合わせて置く</summary>
	void PlaceObjective(float ratio) const;

	// 各アイテムの開始、終わり、更新
	void FinishItem(Item& item) const;
	void StartItem(std::size_t index);
	void UpdateItem(Item& item, bool isGo, float deltaTime);

	// 文字ごとの進め方。stateUpdaters_ から呼ばれる
	void UpdateMoveIn(Item& item, bool isGo);	// 右から中心へ
	void UpdateMoveOut(Item& item);				// 中心から左へ
	void UpdateHold(Item& item);				// 中心で止まる
	void UpdateFade(Item& item);				// 薄くなって消える

	// 演出全体の流れ。phaseUpdaters_ から呼ばれる
	void UpdateObjectiveWait();					// 目的を出すまで待つ
	void UpdateObjectiveIn();					// 目的が出てくる
	void UpdateObjectiveHold();					// 目的を見せている
	void UpdateObjectiveOut();					// 目的を戻す
	void UpdateCountdown(float deltaTime);		// 3 / 2 / 1 / Go

	// 画面上の3つの置き場所
	Math::Vector2 CalcCenterPos() const;
	Math::Vector2 CalcRightPos() const;
	Math::Vector2 CalcLeftPos() const;

	// 次の文字スタート開始
	void UpdateNextStart(float deltaTime);

	/// <summary>進み具合から位置・回転・大きさ・色を入れる</summary>
	void PlaceItem(const Item& item, const Math::Vector2& position, float shapeRatio, float alpha) const;

	/// <summary>0〜1の進み具合を、loopCount回ぶん繰り返した0〜1へ直す</summary>
	float RepeatRatio(float ratio, int32_t loopCount) const;

	/// <summary>
	/// 進み具合から、左右に振る回転の角度を求める。
	/// </summary>
	float CalcSwingDegree(float ratio) const;

	/// <summary>経過時間と長さから0〜1の進み具合を出す</summary>
	float CalcRatio(float timer, float duration) const;

	/// <summary>背景と文字をまとめて表示/非表示にする</summary>
	void SetItemsActive(bool isActive);

	/// <summary>背景を置く</summary>
	void PlaceBack() const;

private:

	// 状態ごとの進め方。Waiting と Done は何もしない
	using ItemUpdater = std::function<void(Item&, bool)>;
	std::array<ItemUpdater, kItemStateCount> stateUpdaters_;

	// 流れごとの進め方
	using PhaseUpdater = std::function<void(float)>;
	std::array<PhaseUpdater, kPhaseCount> phaseUpdaters_;

	// 調整値
	IntroUIParameter parameter_;

	// 背景を暗くするスプライト
	AOENGINE::Sprite* back_ = nullptr;
	// 最初に出す目的の絵
	AOENGINE::Sprite* objective_ = nullptr;

	// 今の流れと、その経過時間
	Phase phase_ = Phase::Done;
	float phaseTimer_ = 0.0f;

	// 3 / 2 / 1 / Go
	std::array<Item, IntroUIParameter::kItemCount> items_;

	// 何文字目まで動き出したか
	std::size_t startedCount_ = 0;
	// 次の文字を出す間合いを数えている最中か
	bool isWaitingNext_ = false;
	float nextTimer_ = 0.0f;

	// 演出そのものが動いているか
	bool isPlaying_ = false;
	// 最後の文字が消えてからの余韻
	float endTimer_ = 0.0f;

	// シーン上での名前
	const std::string kBackName = "IntroBack";
	const std::string kObjectiveName = "IntroObjective";
	const std::string kItemName = "Text_IntroItem";

	// 出す文字。今は画像を使わず Text で書く
	const std::array<std::string, IntroUIParameter::kItemCount> kItemTexts{ "3", "2", "1", "Go!" };

	// 手前に出すための描画順
	const int kBackRenderQueue = 200;
	const int kTextRenderQueue = 201;

public: // accessor

	/// <summary>演出が動いている最中か</summary>
	bool IsPlaying() const { return isPlaying_; }

	/// <summary>Goまで終わったか。ゲームを始める合図に使う</summary>
	bool IsFinished() const { return !isPlaying_; }
};
