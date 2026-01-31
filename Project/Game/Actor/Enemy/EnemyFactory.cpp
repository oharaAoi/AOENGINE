#include "EnemyFactory.h"
#include "Game/Actor/Enemy/Short/ShortRangeEnemy.h"
#include "Game/Actor/Enemy/Mid/MidRangeEnemy.h"
#include "Game/Actor/Enemy/Long/LongRangeEnemy.h"
#include "Game/Actor/Boss/Boss.h"

std::unique_ptr<BaseEnemy> EnemyFactory::Create(EnemyType type) {
	switch (type) {
	case EnemyType::Short:
		return std::make_unique<ShortRangeEnemy>();
	case EnemyType::Mid:
		return std::make_unique<MidRangeEnemy>();
	case EnemyType::Long:
		return std::make_unique<LongRangeEnemy>();
	case EnemyType::Boss:
		return std::make_unique<Boss>();
	default:
		break;
	}
    return nullptr;
}
