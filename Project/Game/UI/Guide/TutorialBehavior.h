#pragma once
#include <memory>
#include "Game/Scene/TutorialScene.h"

/// <summary>
/// Tutorialの内容のBehavior
/// </summary>
class ITutorialBehavior {
public:

	ITutorialBehavior(TutorialScene* _host) : host_(_host) {}
	virtual ~ITutorialBehavior() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

protected:
	TutorialScene* host_;
	bool isNext_ = false;
};

/// <summary>
/// 移動のTutorial
/// </summary>
class TutorialMoveBehavior :
	public ITutorialBehavior {
public:
	TutorialMoveBehavior(TutorialScene* _host) :ITutorialBehavior(_host) {}
	~TutorialMoveBehavior() override = default;
	void Init() override;
	void Update() override;
private:
	float totalMoveTime_;	// 移動した時間の合計
	float targetValue_ = 5.0f;		// 目標時間
};

/// <summary>
/// JumpのTutorial
/// </summary>
class TutorialJumpBehavior :
	public ITutorialBehavior {
public:
	TutorialJumpBehavior(TutorialScene* _host) :ITutorialBehavior(_host) {}
	~TutorialJumpBehavior() override = default;
	void Init() override;
	void Update() override;
private:
	float totalJumpTime_;	// 移動した時間の合計
	float targetValue_ = 5.0f;		// 目標時間
};

/// <summary>
/// BoostのTutorial
/// </summary>
class TutorialBoostBehavior :
	public ITutorialBehavior {
public:
	TutorialBoostBehavior(TutorialScene* _host) :ITutorialBehavior(_host) {}
	~TutorialBoostBehavior() override = default;
	void Init() override;
	void Update() override;
private:
	int totalBoostCount;	// 移動した時間の合計
	int targetValue_ = 5;		// 目標時間
	bool isCount_;
};

/// <summary>
/// LockOnのTutorialを実行する
/// </summary>
class TutorialLockOnBehavior :
	public ITutorialBehavior {
public:
	TutorialLockOnBehavior(TutorialScene* _host) :ITutorialBehavior(_host) {}
	~TutorialLockOnBehavior() override = default;
	void Init() override;
	void Update() override;
private:
	float totalLockOnTime_;	// 移動した時間の合計
	float targetValue_ = 5.0f;		// 目標時間
};

/// <summary>
/// AttackのTutorialを実行する
/// </summary>
class TutorialAttackBehavior :
	public ITutorialBehavior {
public:
	TutorialAttackBehavior(TutorialScene* _host) :ITutorialBehavior(_host) {}
	~TutorialAttackBehavior() override = default;
	void Init() override;
	void Update() override;
private:
	int totalAttackCout_;
	int targetValue_ = 10;
};

/// <summary>
/// Freeモードのチュートリアル
/// </summary>
class TutorialFreeModeBehavior :
	public ITutorialBehavior {
public:
	TutorialFreeModeBehavior(TutorialScene* _host) :ITutorialBehavior(_host) {}
	~TutorialFreeModeBehavior() override = default;
	void Init() override;
	void Update() override;
};