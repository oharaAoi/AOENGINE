#pragma once

/// stl
#include <memory>
#include <vector>

/// engine
#include "Engine/Lib/Math/Vector3.h"

/// game
#include "Game/UI/DamageTextUI.h"
#include "Game/UI/DamageTextUIParameter.h"

/// <summary>
/// DamageTextUI の生成から破棄までを管理するクラス。
///
/// 表示は集合地点に1つだけ出て、グループが動き出すたびに集めた数へ書き換わり、
/// 全部集まったら総ダメージへ切り替わって、打ち上げで消える。
/// 接続受付は集合が終わるまで再開されない(BlockGroupConnectState)ため、
/// 書き換える対象になる表示は常に1つだけになる。それを current_ が指す
/// </summary>
class DamageTextUIManager{
public:
	DamageTextUIManager() = default;
	~DamageTextUIManager() = default;

	// 表示の実体と調整値を所有するためコピーはしない
	DamageTextUIManager(const DamageTextUIManager&) = delete;
	DamageTextUIManager& operator=(const DamageTextUIManager&) = delete;

	/// <summary>
	/// 保存済みの調整値を読み込む
	/// </summary>
	void Init();

	/// <summary>
	/// グループが動き出すたびに呼ぶ。そこまでに集めたブロック数とコンボを出す。
	/// 既に出ている場合は数を書き換えて、その場でもう一度跳ねさせる
	/// </summary>
	/// <param name="blockCount">そこまでに集めたブロック数</param>
	/// <param name="comboCount">そこまでに集めたグループ数(コンボ)</param>
	/// <param name="position">集合地点</param>
	void ShowGatheredCount(int blockCount,int comboCount,const Math::Vector3& position);

	/// <summary>
	/// 全部集まった時に呼ぶ。総ダメージへ切り替えて、出るところからやり直す
	/// </summary>
	/// <param name="damage">この塊がボスに当たった時のダメージ量</param>
	/// <param name="position">集合地点</param>
	void ShowDamage(float damage,const Math::Vector3& position);

	/// <summary>
	/// 出ている表示を全て消し始める。打ち上げた時に呼ぶ
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

	/// <summary>
	/// 書き換える対象が居ればそれを使い、居なければ新しく出す
	/// </summary>
	/// <param name="text">表示する文字</param>
	/// <param name="position">出す位置(新しく出す時だけ使う)</param>
	/// <param name="isReplay">trueなら出るところからやり直す。falseならその場で跳ね直すだけ</param>
	void Show(const std::string& text,const Math::Vector3& position,bool isReplay);

private:

	// DamageTextUI は自分自身を捕まえたコールバックを持つため、vector の再確保で
	// 実体が動くと捕まえた先が壊れる。実体の位置を固定するため unique_ptr で持つ
	std::vector<std::unique_ptr<DamageTextUI>> entries_;

	// 今書き換えている表示(entries_ の中の1つを指す非所有ポインタ)。
	// 消し始めたものはもう書き換えないため、その時点で手放す
	DamageTextUI* current_ = nullptr;

	// 調整値。表示は作られては消えるため、実体はここに1つだけ持ち、
	// 各 DamageTextUI へは参照を渡す(エディタで編集した値がその場で効く)
	DamageTextUIParameter parameter_;

public: // accessor

	// 出ている表示の数
	int GetActiveCount() const{ return static_cast<int>(entries_.size()); }

	DamageTextUIParameter& GetParameter(){ return parameter_; }
	const DamageTextUIParameter& GetParameter() const{ return parameter_; }

};
