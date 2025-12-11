#include "BehaviorTreeNodeFactory.h"
#include "Engine/System/AI/Node/BehaviorRootNode.h"
#include "Engine/System/AI/Node/SequenceNode.h"
#include "Engine/System/AI/Node/SelectorNode.h"
#include "Engine/System/AI/Node/WeightSelectorNode.h"
#include "Engine/System/AI/Node/PlannerNode.h"
#include "Engine/System/AI/Node/PlannerSelectorNode.h"
#include "Engine/System/AI/Node/ConditionNode.h"
#include "Engine/System/AI/Node/ParallelNode.h"
#include "Engine/System/AI/BehaviorTreeSerializer.h"

using namespace AI;

void BehaviorTreeNodeFactory::CreateNode(int nodeType, const std::string& crateTaskName, std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList,
										 Blackboard* _worldState, std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap, 
										 const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray, const ImVec2& _mousePos) {
	if (nodeType == (int)NodeType::Sequencer) {
		auto& node = _nodeList.emplace_back(std::make_shared<SequenceNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Selector) {
		auto& node = _nodeList.emplace_back(std::make_shared<SelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::WeightSelector) {
		auto& node = _nodeList.emplace_back(std::make_shared<WeightSelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Planner) {
		auto& node = _nodeList.emplace_back(std::make_shared<PlannerNode>(_canTaskMap, _worldState, _goalArray));
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::PlannerSelector) {
		auto& node = _nodeList.emplace_back(std::make_shared<PlannerSelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::PlannerSelector) {
		auto& node = _nodeList.emplace_back(std::make_shared<PlannerSelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Parallel) {
		auto& node = _nodeList.emplace_back(std::make_shared<ParallelNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Condition) {
		auto& node = _nodeList.emplace_back(std::make_shared<ConditionNode>());
		node->SetBlackboard(_worldState);
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Task) {
		auto& node = _nodeList.emplace_back(_canTaskMap[crateTaskName]->Clone());
		node->Init();
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));
		node->SetBlackboard(_worldState);
	}
}

std::shared_ptr<BaseBehaviorNode> BehaviorTreeNodeFactory::CreateNodeFromJson(const json& _json, std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList,
																		   std::vector<Link>& _link, Blackboard* _worldState, 
																		   std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap,
																		   const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray) {
	// nodeを作成
	std::shared_ptr<BaseBehaviorNode> node;
	NodeType type = static_cast<NodeType>(_json["nodeType"]);
	std::string name = _json["name"];

	// 種類によってインスタンスを変える
	switch (type) {
	case NodeType::Root: node = std::make_shared<BehaviorRootNode>(); break;
	case NodeType::Sequencer: node = std::make_shared<SequenceNode>(); break;
	case NodeType::Selector: node = std::make_shared<SelectorNode>(); break;
	case NodeType::WeightSelector: node = std::make_shared<WeightSelectorNode>(); break;
	case NodeType::Condition: node = std::make_shared<ConditionNode>(); break;
	case NodeType::Parallel: node = std::make_shared<ParallelNode>(); break;
	case NodeType::Planner:
		node = std::make_shared<PlannerNode>(_canTaskMap, _worldState, _goalArray);
		{
			PlannerNode* plannerNode = dynamic_cast<PlannerNode*>(node.get());
			plannerNode->SetGOBT(_json["orientedName"], _json["treeFileName"]);
		}
		break;
	case NodeType::PlannerSelector:
		node = std::make_shared<PlannerSelectorNode>();
		break;
	case NodeType::Task:
		node = _canTaskMap[name]->Clone();
		node->Init();
		break;
	}

	// jsonからnodeの情報を取得
	node->SetBlackboard(_worldState);
	node->FromJson(_json);
	_nodeList.push_back(node);

	// 子どもがいたら再帰的に処理
	for (const auto& childJson : _json["children"]) {
		std::shared_ptr<BaseBehaviorNode> child = CreateNodeFromJson(childJson, _nodeList, _link, _worldState, _canTaskMap, _goalArray);
		node->AddChild(child.get());

		// nodeと子どもをリンクでつなぐ
		Link link;
		link.id = BaseBehaviorNode::GetNextId();
		link.from = node->GetOutput().id;
		link.to = child->GetInput().id;
		_link.push_back(link);
	}

	return node;
}

void BehaviorTreeNodeFactory::CreateTree(const std::string& nodeName, std::list<std::shared_ptr<BaseBehaviorNode>>& _nodeList,
										 std::vector<Link>& _link, BaseBehaviorNode* _root, Blackboard* _worldState, 
										 std::unordered_map<std::string, std::shared_ptr<BaseBehaviorNode>>& _canTaskMap,
										 const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray) {
	AOENGINE::Logger::Log("[Create][BehaviorTree] : " + nodeName);
	_nodeList.clear();
	if (_root != nullptr) {
		_root->ClearChild();
	}

	// jsonからtreeの情報を読み取る
	json nodeTree = BehaviorTreeSerializer::LoadToJson(nodeName);
	_root = _nodeList.emplace_back(BehaviorTreeNodeFactory::CreateNodeFromJson(nodeTree, _nodeList, _link, _worldState, _canTaskMap, _goalArray)).get();

	AOENGINE::Logger::Log("--- success!");
}
