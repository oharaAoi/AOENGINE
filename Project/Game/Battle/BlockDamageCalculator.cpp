#include "BlockDamageCalculator.h"

/// stl
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////////////
//  ダメージの算出
///////////////////////////////////////////////////////////////////////////////////////////////

float BlockDamageCalculator::Calculate(const HitContext& context) const{
	// ダメージ計算式
	//! TODO 調整
	float damage = (parameter_.perBlock * static_cast<float>(context.blockCount));
	return (std::min)(damage,parameter_.maxDamage);
}
