#pragma once
#include <memory>
#include "Game/Scene/GameScene.h"
#include "Game/UI/Guide/GameMenuUI.h"

/// <summary>
/// Tutorialの内容のBehavior
/// </summary>
class IGameSceneBehavior {
public:

	IGameSceneBehavior(GameScene* _host) : host_(_host) {}
	virtual ~IGameSceneBehavior() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

protected:
	GameScene* host_;
};

/// <summary>
/// GamePlay中のBehavior
/// </summary>
class GamePlayBehavior :
	public IGameSceneBehavior {
public:
	GamePlayBehavior(GameScene* _host) :IGameSceneBehavior(_host) {}
	~GamePlayBehavior() override = default;
	void Init() override;
	void Update() override;
};

/// <summary>
/// GameMenuのBehavior
/// </summary>
class GameMenuBehavior :
	public IGameSceneBehavior {
public:
	GameMenuBehavior(GameScene* _host) :IGameSceneBehavior(_host) {}
	~GameMenuBehavior() override = default;
	void Init() override;
	void Update() override;
private:

	std::unique_ptr<GameMenuUI> gameMenuUI_;
};