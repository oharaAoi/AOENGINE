#pragma once
// c++
#include <memory>
#include <list>
#include <unordered_map>
#include <cassert>
#include <string>
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Information/ActionContext.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/System/Editer/Window/EditorWindows.h"

template<typename OwnerType>
class ActionManager :
	public AttributeGui {
public:

	ActionManager() {}
	~ActionManager() {
		Finalize();
	};

	void Finalize() {
		runActionMap_.clear();
		actionMap_.clear();
	}

	void Init(OwnerType* pOwner, const std::string& name) {
		pOwner_ = pOwner;

		context_ = std::make_shared<ActionContext>();
	
		name_ = name;
		EditorWindows::AddObjectWindow(this, name.c_str());

		isActionStop_ = false;
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

		// 実際に追加する
		for (const auto& addAction : addIndexList_) {
			auto it = runActionMap_.find(addAction);
			if (it == runActionMap_.end()) {
				AddRunAction(addAction);
			}
		}

		// 削除リストをクリア
		deleteIndexList_.clear();
		addIndexList_.clear();

		// 実行
		if (!isActionStop_) {
			for (auto& [size, action] : runActionMap_) {
				if (action) {
					action->OnUpdate();
				}
			}
		}

		// 追加するActionがあるかを判定する
		for (auto& action : runActionMap_) {
			if (action.second != nullptr) {
				action.second->CheckNextAction();
			}
		}
	}

	/// <summary>
	/// 実行中のAction名を表示
	/// </summary>
	void DisplayRunActions() {
		ImGui::BulletText("RunActions");
		for (auto& [size, action] : runActionMap_) {
			ImGui::Text(action->GetName().c_str());
		}
	}

public:

	void Debug_Gui() override {
		DisplayRunActions();
	}

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
		actionMap_[hash]->SetContext(context_);
		actionMap_[hash]->Build();
		AddChild(actionMap_[hash].get());
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

	void ChangeAction(size_t actionTypeIndex) {
		// actionMap_ に actionTypeIndex が存在するか確認
		assert(actionMap_.find(actionTypeIndex) != actionMap_.end() && "actionTypeIndex not found in actionMap_");

		if (runActionMap_.find(actionTypeIndex) == runActionMap_.end()) {
			runActionMap_.clear();
			runActionMap_[actionTypeIndex] = actionMap_[actionTypeIndex];
			runActionMap_[actionTypeIndex]->CallStart(actionTypeIndex);
		}
	}

	/// <summary>
	/// 指定したアクションを追加リストにいれる
	/// </summary>
	/// <param name="actionTypeIndex"></param>
	void AddAction(size_t actionTypeIndex) {
		auto it = runActionMap_.find(actionTypeIndex);
		if (it == runActionMap_.end()) {
			addIndexList_.emplace_back(actionTypeIndex);
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

	bool CheckInput(size_t actionTypeIndex) {
		return actionMap_[actionTypeIndex]->IsInput();
	}

	void SetIsActionStop(bool _stop) { isActionStop_ = _stop; }



private:
	// 持ち主のポインタ
	OwnerType* pOwner_ = nullptr;

	// 今のAction
	std::unordered_map<size_t, std::shared_ptr<BaseAction<OwnerType>>> runActionMap_;
	// ownerが行うActionのインスタンスをまとめたMap
	std::unordered_map<size_t, std::shared_ptr<BaseAction<OwnerType>>> actionMap_;

	// 追加するActionのリスト
	std::list<size_t> addIndexList_;
	// 削除するActionのリスト
	std::list<size_t> deleteIndexList_;

	// 変数の値を共有したいときに使用するmap
	std::shared_ptr<ActionContext> context_;

	bool isActionStop_;
};