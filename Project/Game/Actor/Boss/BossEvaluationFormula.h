#pragma once

/// <summary>
/// ボスの行動の評価値の計算式
/// </summary>
namespace BossEvaluationFormula{
	
	/// <summary>
	/// 適正距離評価値
	/// </summary>
	/// <param name="_distance">: 現在の距離</param>
	/// <param name="_optimal">: 理想距離</param>
	/// <param name="_range">: 有効範囲</param>
	/// <returns></returns>
	float AppropriateDistance(float _distance, float _optimal, float _range);


	
}
