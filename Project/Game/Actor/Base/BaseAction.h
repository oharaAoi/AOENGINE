#pragma once
// c++
#include <string>
#include <memory>
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
// engine
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Game/Information/ActionContext.h"

template<typename OwnerType>
class ActionManager;

template<typename OwnerType>
class BaseAction 
	: public AttributeGui {
protected:
	// Observer以外から呼び出し関数などにアクセスできないように
	friend class ActionManager<OwnerType>;
	size_t actionIndex_;

public:	// base

	BaseAction() = default;
	virtual ~BaseAction() override = default;

	/// <summary>
	/// 設定ジのみ行う処理
	/// </summary>
	virtual void Build() = 0;

	virtual void OnStart() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnEnd() = 0;
	virtual void CheckNextAction() = 0;
	virtual bool IsInput() = 0;

	void Debug_Gui() override {};

	/// <summary>
	/// 自身のアクションを削除する
	/// </summary>
	void DeleteSelf() {
		if (!pManager_) {
			return;
		}
		pManager_->DeleteAction(actionIndex_);
	}

	void NextAction(size_t hash);
	void AddAction(size_t hash);

	/// <summary>
	/// 次のアクションに遷移する関数
	/// </summary>
	/// <typeparam name="ActionT">: 次のアクションのクラス</typeparam>
	template<typename ActionT>
	void NextAction() {
		if (!pManager_) {
			return;
		}
		size_t hash = typeid(ActionT).hash_code();
		pManager_->AddAction(hash);
		pManager_->DeleteAction(actionIndex_);
	}

	/// <summary>
	/// 行うアクションを追加する関数
	/// </summary>
	/// <typeparam name="ActionT">: 追加するアクションのクラス</typeparam>
	template<typename ActionT>
	void AddAction() {
		if (!pManager_) {
			return;
		}
		size_t hash = typeid(ActionT).hash_code();
		pManager_->AddAction(hash);
	}

	/// <summary>
	/// 入力処理を確認する
	/// </summary>
	/// <typeparam name="ActionT"></typeparam>
	/// <returns></returns>
	template<typename ActionT>
	bool CheckInput() {
		if (!pManager_) {
			return 0;
		}
		size_t hash = typeid(ActionT).hash_code();
		return pManager_->CheckInput(hash);
	}

	void SetContext(const std::shared_ptr<ActionContext>& context) {
		context_ = context;
	}
	
protected:	// observerクラス以外からは呼び出さない

	void SetObserver(ActionManager<OwnerType>* pObserver) { pManager_ = pObserver; }
	void SetOwner(OwnerType* owner) { pOwner_ = owner; }

	void CallStart(size_t actionTypeIndex) {
		actionIndex_ = actionTypeIndex;
		OnStart();
	}
	void CallUpdate() { OnUpdate(); }
	void CallEnd() { OnEnd(); }

protected:
	ActionManager<OwnerType>* pManager_ = nullptr;
	OwnerType* pOwner_ = nullptr;

	// 変数の値を共有したいときに使用するmap
	std::shared_ptr<ActionContext> context_;

	float actionTimer_;
};

template<typename OwnerType>
inline void BaseAction<OwnerType>::NextAction(size_t hash) {
	if (!pManager_) {
		return;
	}
	pManager_->AddAction(hash);
	pManager_->DeleteAction(actionIndex_);
}

template<typename OwnerType>
inline void BaseAction<OwnerType>::AddAction(size_t hash) {
	if (!pManager_) {
		return;
	}
	pManager_->AddAction(hash);
}