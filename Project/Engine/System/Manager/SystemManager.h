#pragma once
#include "Engine/System/AI/BehaviorTreeSystem.h"

class SystemManager {
public: // コンストラクタ

	SystemManager() = default;
	~SystemManager() = default;

public:

	void Init();
	
	void Update();

private:

	BehaviorTreeSystem* behaviorTree_;

};

