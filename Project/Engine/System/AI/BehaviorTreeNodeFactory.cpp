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
#include "Engine/Utilities/Logger.h"

using namespace AI;

void BehaviorTreeNodeFactory::CreateNode(int nodeType, const std::string& crateTaskName, std::list<std::unique_ptr<BaseBehaviorNode>>& _nodeList,
										 Blackboard* _worldState, const std::unordered_map<std::string, ActionNode>& _creators,
										 const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray, const ImVec2& _mousePos) {
	if (nodeType == (int)NodeType::Sequencer) {
		auto& node = _nodeList.emplace_back(std::make_unique<SequenceNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Selector) {
		auto& node = _nodeList.emplace_back(std::make_unique<SelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::WeightSelector) {
		auto& node = _nodeList.emplace_back(std::make_unique<WeightSelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Planner) {
		auto& node = _nodeList.emplace_back(std::make_unique<PlannerNode>(_creators, _worldState, _goalArray));
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::PlannerSelector) {
		auto& node = _nodeList.emplace_back(std::make_unique<PlannerSelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::PlannerSelector) {
		auto& node = _nodeList.emplace_back(std::make_unique<PlannerSelectorNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Parallel) {
		auto& node = _nodeList.emplace_back(std::make_unique<ParallelNode>());
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Condition) {
		auto& node = _nodeList.emplace_back(std::make_unique<ConditionNode>());
		node->SetBlackboard(_worldState);
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));

	} else if (nodeType == (int)NodeType::Task) {
		auto& node = _nodeList.emplace_back(CreateActionNode(_creators, crateTaskName));
		node->Init();
		node->SetPos(Math::Vector2(_mousePos.x, _mousePos.y));
		node->SetBlackboard(_worldState);
	}
}

BaseBehaviorNode* BehaviorTreeNodeFactory::CreateNodeFromJson(const json& _json, std::list<std::unique_ptr<BaseBehaviorNode>>& _nodeList,
																		   std::vector<Link>& _link, Blackboard* _worldState, 
																		   const std::unordered_map<std::string, ActionNode>& _creators,
																		   const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray) {
	// 空なら
	if (_json.empty()) {
		std::unique_ptr<BaseBehaviorNode> node = std::make_unique<BehaviorRootNode>();
		node->SetBlackboard(_worldState);
		BaseBehaviorNode* rawNode = node.get();
		_nodeList.push_back(std::move(node));
		return rawNode;
	}

	// nodeを作成
	std::unique_ptr<BaseBehaviorNode> node;
	NodeType type = static_cast<NodeType>(_json["nodeType"]);
	std::string name = _json["name"];

	// 種類によってインスタンスを変える
	switch (type) {
	case NodeType::Root: node = std::make_unique<BehaviorRootNode>(); break;
	case NodeType::Sequencer: node = std::make_unique<SequenceNode>(); break;
	case NodeType::Selector: node = std::make_unique<SelectorNode>(); break;
	case NodeType::WeightSelector: node = std::make_unique<WeightSelectorNode>(); break;
	case NodeType::Condition: node = std::make_unique<ConditionNode>(); break;
	case NodeType::Parallel: node = std::make_unique<ParallelNode>(); break;
	case NodeType::Planner:
		node = std::make_unique<PlannerNode>(_creators, _worldState, _goalArray);
		{
			PlannerNode* plannerNode = dynamic_cast<PlannerNode*>(node.get());
			plannerNode->SetGOBT(_json["orientedName"], _json["treeFileName"]);
		}
		break;
	case NodeType::PlannerSelector:
		node = std::make_unique<PlannerSelectorNode>();
		break;
	case NodeType::Task:
		node = CreateActionNode(_creators, name);
		node->Init();
		break;
	}

	// jsonからnodeの情報を取得
	node->SetBlackboard(_worldState);
	node->FromJson(_json);
	BaseBehaviorNode* rawNode = node.get();
	_nodeList.push_back(std::move(node));

	// 子どもがいたら再帰的に処理
	for (const auto& childJson : _json["children"]) {
		BaseBehaviorNode* child = CreateNodeFromJson(childJson, _nodeList, _link, _worldState, _creators, _goalArray);
		rawNode->AddChild(child);

		// nodeと子どもをリンクでつなぐ
		Link link;
		link.id = BaseBehaviorNode::GetNextId();
		link.from = rawNode->GetOutput().id;
		link.to = child->GetInput().id;
		_link.push_back(link);
	}
	return rawNode;
}

void BehaviorTreeNodeFactory::CreateTree(const json& _json, std::list<std::unique_ptr<BaseBehaviorNode>>& _nodeList,
										 std::vector<Link>& _link, BaseBehaviorNode* _root, Blackboard* _worldState, 
										 const std::unordered_map<std::string, ActionNode>& _creators,
										 const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray) {
	_nodeList.clear();
	if (_root != nullptr) {
		_root->ClearChild();
	}

	// jsonからtreeの情報を読み取る
	_root = _nodeList.emplace_back(BehaviorTreeNodeFactory::CreateNodeFromJson(_json, _nodeList, _link, _worldState, _creators, _goalArray)).get();

	AOENGINE::Logger::Log("--- success!");
}

std::unique_ptr<BaseBehaviorNode> AI::BehaviorTreeNodeFactory::CreateActionNode(const std::unordered_map<std::string, ActionNode>& _creators,
																								   const std::string& _name) {
	auto it = _creators.find(_name);
	if (it == _creators.end()) {
		return nullptr;
	}
	return it->second();
}
