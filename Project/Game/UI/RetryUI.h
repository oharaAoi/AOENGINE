#pragma once

// engine
#include <Engine/Module/Components/2d/Sprite.h>

/// <summary>
/// リトライを管理しているクラス
/// </summary>
class RetryUI {
public: // constructor

	RetryUI() = default;
	~RetryUI() = default;

public: // public method

	void Init();

	void Update(bool isPlayerAlive);

private: // private variable



};

