#pragma once
#include "Engine/Lib/Math/Vector3.h"

enum class BossBulletType {
	Shot,
	Missile,
};

/// <summary>
/// bulletの基礎パラメータ
/// </summary>
struct MissileInitParam {
	Math::Vector3 pos;
	Math::Vector3 velocity;
};