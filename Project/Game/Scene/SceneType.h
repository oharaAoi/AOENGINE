#pragma once

enum class SceneType {
	Title,
	Game,
	GameOver,
	Tutorial,
	Test,
};

static const char* kSceneTypeNames[] = { "TITLE", "GAME", "GAMEOVER","TUTORIAL", "TEST"};