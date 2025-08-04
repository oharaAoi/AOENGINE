#pragma once

class Boss;

class BossEvaluationFormula {
public:

	BossEvaluationFormula() = default;
	~BossEvaluationFormula() = default;

	void Init(Boss* _pBoss);

	void Update();

	float HeightEvaluation(float min, float max);

	float ApproachEvaluation(float min, float max);

	float LeaveEvaluation(float min, float max);

	float StrafeEvaluation(float min, float max);

private:

	Boss* pBoss_;

	float nearTimer_;

	float preDamage_;
	float damageRate_;

};

