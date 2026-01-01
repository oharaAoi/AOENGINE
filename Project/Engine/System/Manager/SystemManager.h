#pragma once
#include "Engine/System/AI/BehaviorTreeSystem.h"

namespace AOENGINE {

class SystemManager {
public: // コンストラクタ

	SystemManager() = default;
	~SystemManager() = default;

public:

	void Finalize();

	void Init();

	void Update();

private:

	AI::BehaviorTreeSystem* behaviorTree_;

};

}