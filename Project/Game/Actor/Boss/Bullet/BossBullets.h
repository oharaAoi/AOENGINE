#pragma once
#include "Engine/Lib/Math/Vector3.h"

enum class BossBulletType {
	MISSILE,
};

struct MissileInitParam {
	Vector3 pos;
	Vector3 velocity;
};