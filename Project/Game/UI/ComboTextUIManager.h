#pragma once

/// stl
#include <memory>
#include <vector>

/// engine
#include "Engine/Lib/Math/Vector3.h"

/// game
#include "Game/UI/ComboTextUI.h"
#include "Game/UI/ComboTextUIParameter.h"

/// <summary>
/// ComboTextUI の生成から破棄までを管理するクラス。
///
/// 表示は「どのブロックグループに対して出したものか」を groupId で覚えておく。
/// 出すのはプレイヤーがグループを接続した瞬間(Player::TryConnectBlockGroup)、
/// 消し始めるのはそのグループが集合に向けて動き出した瞬間(BlockGroupLauncher)で、
/// この groupId がブロック側と表示側を結ぶ唯一の手掛かりになる。
/// ComboTextUI の実体はこのクラスが所有し、消え終わったものは Update() で破棄する
/// </summary>
class ComboTextUIManager{
public:
	ComboTextUIManager() = default;
	~ComboTextUIManager() = default;

	// 表示の実体と調整値を所有するためコピーはしない
	ComboTextUIManager(const ComboTextUIManager&) = delete;
	ComboTextUIManager& operator=(const ComboTextUIManager&) = delete;

	/// <summary>
	/// 保存済みの調整値を読み込む
	/// </summary>
	void Init();

	/// <summary>
	/// ブロックグループが接続された時に呼ぶ。そのグループの位置にコンボ数を出す。
	/// 同じグループの表示が既に出ている場合は何もしない
	/// </summary>
	/// <param name="groupId">接続されたグループID</param>
	/// <param name="comboCount">今接続されているグループの数</param>
	/// <param name="position">接続したグループの中心</param>
	void Spawn(int groupId,int comboCount,const Math::Vector3& position);

	/// <summary>
	/// 指定したグループの表示を消し始める。そのグループの表示が無ければ何もしない
	/// </summary>
	/// <param name="groupId">消し始めるグループID</param>
	void StartFadeOut(int groupId);

	/// <summary>
	/// 出ている表示を全て消し始める。接続受付をキャンセルした時など、
	/// どのグループも動き出さないまま終わる場合に呼ぶ
	/// </summary>
	void StartFadeOutAll();

	/// <summary>
	/// 更新。消え終わった表示はここでGameObjectごと破棄する
	/// </summary>
	void Update();

	/// <summary>
	/// 出ている表示を消える途中でも即座に破棄する。ステージを作り直す時などに呼ぶ
	/// </summary>
	void Clear();

private:

	/// <summary>出ている表示1つ分</summary>
	struct Entry{
		int groupId = -1;					// どのブロックグループに対して出したものか
		std::unique_ptr<ComboTextUI> ui;	// 表示の実体(所有)
	};

	/// <summary>groupId に対応する表示を探す。見つからなければ nullptr</summary>
	ComboTextUI* Find(int groupId) const;

private:

	// ComboTextUI は自分自身を捕まえたコールバックを持つため、vector の再確保で
	// 実体が動くと捕まえた先が壊れる。実体の位置を固定するため unique_ptr で持つ
	std::vector<Entry> entries_;

	// 調整値。表示は次々に作られては消えるため、実体はここに1つだけ持ち、
	// 各 ComboTextUI へは参照を渡す(エディタで編集した値がその場で効く)
	ComboTextUIParameter parameter_;

public: // accessor

	// 出ている表示の数
	int GetActiveCount() const{ return static_cast<int>(entries_.size()); }

	ComboTextUIParameter& GetParameter(){ return parameter_; }
	const ComboTextUIParameter& GetParameter() const{ return parameter_; }

};
