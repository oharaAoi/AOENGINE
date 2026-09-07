#pragma once

// engine
#include "Engine/Module/Components/2d/Sprite.h"

/// <summary>
/// 操作UIを管理するクラス
/// </summary>
class ControlUI {
public: // constructor

	ControlUI() = default;
	~ControlUI() = default;

public: // public method

	void Init();

	void Update();

private: // private variables

	AOENGINE::Sprite* keyboardUI_;
	AOENGINE::Sprite* gamePadUI_;

};

