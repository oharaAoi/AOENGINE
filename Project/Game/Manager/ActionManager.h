#pragma once
// c++
#include <memory>
#include <list>
#include <unordered_map>
#include <cassert>

#include "Game/Actor/Base/BaseAction.h"

template<typename OwnerType>
class ActionManager {
public:

	ActionManager() {}
	~ActionManager() = default;

	void Init(OwnerType* pOwner) {
		pOwner_ = pOwner;
	}

	void Update() {
		// 終了するActionの終了処理を行う
		for (const auto& deleteAction : deleteIndexList_) {
			auto it = runActionMap_.find(deleteAction);
			if (it != runActionMap_.end()) {
				it->second->CallEnd(); // アクション終了処理を呼ぶ
				runActionMap_.erase(it);  // アクションを削除
			}
		}

		// 削除リストをクリア
		deleteIndexList_.clear();

		// 実行
		for (auto& [size, action] : runActionMap_) {
			if (action) {
				action->OnUpdate();
			}
		}

		// 追加するActionがあるかを判定する
		for (auto& [size, action] : runActionMap_) {
			if (action) {
				action->CheckNextAction();
			}
		}
	}

	/// <summary>
	/// 実行中のAction名を表示
	/// </summary>
	void DisplayRunActions() {
		/*ImGui::BulletText("RunActions");
		for (auto& [size, action] : runActionMap_) {
			ImGui::Text(action->GetActionName().c_str());
		}*/
	}

public:

	/// <summary>
	/// Actionのインスタンスを構築する
	/// </summary>
	/// <typeparam name="ActionT"></typeparam>
	template<typename ActionT>
	void BuildAction() {
		size_t hash = typeid(ActionT).hash_code();
		actionMap_[hash] = std::make_shared<ActionT>();
		actionMap_[hash]->SetOwner(pOwner_);
		actionMap_[hash]->SetObserver(this);
		actionMap_[hash]->Build();
		actionMap_[hash]->CallStart(hash);
	}

	/// <summary>
	/// 実行するActionを追加する
	/// </summary>
	void AddRunAction(size_t actionTypeIndex) {
		// actionMap_ に actionTypeIndex が存在するか確認
		assert(actionMap_.find(actionTypeIndex) != actionMap_.end() && "actionTypeIndex not found in actionMap_");

		if (runActionMap_.find(actionTypeIndex) == runActionMap_.end()) {
			runActionMap_[actionTypeIndex] = actionMap_[actionTypeIndex];
			runActionMap_[actionTypeIndex]->CallStart(actionTypeIndex);
		}
	}

	/// <summary>
	/// 指定したActionを削除する
	/// </summary>
	/// <param name="actionTypeIndex"></param>
	void DeleteAction(size_t actionTypeIndex) {
		auto it = runActionMap_.find(actionTypeIndex);
		if (it != runActionMap_.end()) {
			deleteIndexList_.emplace_back(actionTypeIndex);
		}
	}

	template<typename ActionT>
	std::shared_ptr<BaseAction<OwnerType>> GetActionInstance() {
		size_t hash = typeid(ActionT).hash_code();
		return actionMap_[hash];
	}

private:
	// 持ち主のポインタ
	OwnerType* pOwner_ = nullptr;
	// 今のAction
	std::unordered_map<size_t, std::shared_ptr<BaseAction<OwnerType>>> runActionMap_;
	// 削除するActionのリスト
	std::list<size_t> deleteIndexList_;
	// ownerが行うActionのインスタンスをまとめたMap
	std::unordered_map<size_t, std::shared_ptr<BaseAction<OwnerType>>> actionMap_;
};

