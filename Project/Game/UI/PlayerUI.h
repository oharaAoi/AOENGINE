#pragma once

// engine
#include "Engine/Module/Components/2d/Sprite.h"

// game
#include "Game/Actor/Player/Player.h"

/// <summary>
/// PlayerのUIを管理するクラス
/// </summary>
class PlayerUI {
public: // constructor

	PlayerUI() = default;
	~PlayerUI() = default;

public: // public method

	void Init();

	void Update(Player* player);

private:
	void UpdateHP(Player* player);
	void UpdateConnectableTime(Player* player);

private: // private variable

	AOENGINE::Sprite* hp_ = nullptr;
	AOENGINE::Sprite* connectableTime_ = nullptr;
};

