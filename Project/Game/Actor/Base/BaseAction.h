#pragma once
// c++
#include <string>
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
// engine
#include "Engine/Module/Components/Attribute/AttributeGui.h"

template<typename OwnerType>
class ActionManager;

template<typename OwnerType>
class BaseAction 
	: public AttributeGui {
protected:
	// Observer以外から呼び出し関数などにアクセスできないように
	friend class ActionManager<OwnerType>;
	// actionの名前
	std::string actionName_;
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

#ifdef _DEBUG
	void Debug_Gui() override {};
#endif // _DEBUG

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
	
protected:	// observerクラス以外からは呼び出さない

	void SetObserver(ActionManager<OwnerType>* pObserver) { pManager_ = pObserver; }
	void SetOwner(OwnerType* owner) { pOwner_ = owner; }

	void CallStart(size_t actionTypeIndex) {
		actionIndex_ = actionTypeIndex;
		OnStart();
	}
	void CallUpdate() { OnUpdate(); }
	void CallEnd() { OnEnd(); }

	const std::string& GetActionName() const { return actionName_; }

protected:
	ActionManager<OwnerType>* pManager_ = nullptr;
	OwnerType* pOwner_ = nullptr;
};


