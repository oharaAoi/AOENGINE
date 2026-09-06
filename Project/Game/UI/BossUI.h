#pragma once
// engine
#include "Engine/Module/Components/2d/Sprite.h"

// game
#include "Game/Actor/Boss/Boss.h"

/// <summary>
/// BossのUIを管理するクラス
/// </summary>
class BossUI {
public: // constructor

	BossUI() = default;
	~BossUI() = default;

public: // public method

	void Init();

	void Update(Boss* boss);

private: // private variable

	AOENGINE::Sprite* hp_;
};

