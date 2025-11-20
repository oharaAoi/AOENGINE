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
	Vector3 pos;
	Vector3 velocity;
};