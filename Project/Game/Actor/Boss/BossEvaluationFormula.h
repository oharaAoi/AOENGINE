#pragma once

class Boss;

/// <summary>
/// bossの行動の評価関数をまとめたクラス
/// </summary>
class BossEvaluationFormula {
public:

	BossEvaluationFormula() = default;
	~BossEvaluationFormula() = default;

public:

	// 初期化
	void Init(Boss* _pBoss);
	// 更新
	void Update();
	// 高さ基準の評価
	float HeightEvaluation(float min, float max);
	// 近さ基準の評価
	float ApproachEvaluation(float min, float max);
	// 離れる基準の評価
	float LeaveEvaluation(float min, float max);
	// ストレイフをする基準の評価
	float StrafeEvaluation(float min, float max);

private:

	Boss* pBoss_;

	float nearTimer_;

	float preDamage_;
	float damageRate_;

};

