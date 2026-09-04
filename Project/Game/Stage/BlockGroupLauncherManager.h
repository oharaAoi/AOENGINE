#pragma once

/// stl
#include <vector>

/// game
#include "Game/Stage/BlockGroupLauncher.h"

class StageBlockField;

namespace AOENGINE{
class Color;
class BaseCollider;
}

/// <summary>
/// 複数の BlockGroupLauncher を束ねて管理するクラス。
/// BlockGroupLauncher を1個しか持たないと、打ち上げ中に次の集合が始まった時に
/// BeginGather() の Clear() で前の打ち上げが制御を失ってしまう。
/// そのため、空いているランチャーを使い回しつつ、足りなければ増やして複数の集合・打ち上げを同時に扱えるようにする。
/// </summary>
class BlockGroupLauncherManager{
public:
	BlockGroupLauncherManager() = default;
	~BlockGroupLauncherManager() = default;

	/// <summary>
	/// 新しい集合を開始する。Idle なランチャーを使い回し、無ければ1つ増やす
	/// </summary>
	/// <param name="request">集合の依頼内容</param>
	/// <param name="params">調整値</param>
	void BeginGather(const BlockGroupLauncher::GatherRequest& request,const BlockGroupLauncher::Params& params);

	/// <summary>
	/// 集合中のランチャーを打ち上げる
	/// </summary>
	void Launch();

	/// <summary>
	/// 全ランチャーを更新する
	/// </summary>
	void Update(float deltaTime);

	/// <summary>
	/// 全ランチャーの制御を手放す
	/// </summary>
	void Clear();

	/// <summary>
	/// 動いている全ランチャーの接続線を描画する
	/// </summary>
	/// <param name="color">線の色</param>
	/// <param name="thickness">太さ</param>
	void DrawConnectLine(const AOENGINE::Color& color,float thickness) const;

private:

	std::vector<BlockGroupLauncher> launchers_;	// 使い回すランチャーのプール

	StageBlockField* pField_ = nullptr;	// 非所有

public: // accessor

	void SetField(StageBlockField* field);

	// どれか1つでも動いているか
	bool IsActive() const;
	// 動いているランチャーの数
	int GetActiveCount() const;
	// 全ランチャーのグループ数の合計
	int GetGroupCount() const;
	// 全ランチャーのブロック数の合計
	int GetBlockCount() const;
	/// <summary>指定したColliderを持つブロックが属するランチャーのブロック数を返す。見つからなければ0</summary>
	int GetBlockCountByCollider(const AOENGINE::BaseCollider* collider) const;

};
