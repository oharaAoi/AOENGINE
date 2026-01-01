#pragma once
#include <list>
#include <memory>
#include "Engine/System/AI/BehaviorTree.h"

namespace AI {

/// <summary>
/// Treeを管理するクラス
/// </summary>
class BehaviorTreeSystem {
public: // コンストラクタ

	BehaviorTreeSystem() = default;
	~BehaviorTreeSystem() = default;
	BehaviorTreeSystem(const BehaviorTreeSystem&) = delete;
	const BehaviorTreeSystem& operator=(const BehaviorTreeSystem&) = delete;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static BehaviorTreeSystem* GetInstance();

public:

	void Finalize();

	void Init();

	void Update();

	BehaviorTree* Create();

	void SetSelectNode(BaseBehaviorNode* _node) { selectNode_ = _node; }

private:

	std::list<std::unique_ptr<BehaviorTree>> trees_;

	BaseBehaviorNode* selectNode_;

};

}