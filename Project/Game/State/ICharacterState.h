#pragma once
#include <string>
#include "Engine/Module/Components/Attribute/AttributeGui.h"

template<typename OwnerType>
class StateMachine;

/// <summary>
/// StateのBaseClass
/// </summary>
template<typename OwnerType> // テンプレートで管理対象の型を指定
class ICharacterState :
	public AttributeGui {
protected:
	// ステートマシン以外から呼び出し関数などにアクセスできないように
	friend class StateMachine<OwnerType>;
public:
	ICharacterState() = default;
	virtual ~ICharacterState() = default;

public:

	virtual void OnStart() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnExit() = 0;

	virtual void Debug_Gui() override = 0;

	void SetOwner(OwnerType* owner) { pOwner_ = owner; }

	std::string GetStateName() { return GetName(); }

protected:

	// この状態を管理しているステートマシーンをセット
	void SetMachine(StateMachine<OwnerType>* pMachine) { stateMachine_ = pMachine; }

	void CallStart(OwnerType* pOwner) {
		if (stateMachine_ == nullptr || pOwner == nullptr) {
			return;
		}
		SetOwner(pOwner);
		OnStart();
	}
	void CallUpdate(OwnerType* pOwner) {
		if (stateMachine_ == nullptr || pOwner == nullptr) {
			return;
		}
		OnUpdate();
	}
	void CallExit(OwnerType* pOwner) {
		if (stateMachine_ == nullptr || pOwner == nullptr) {
			return;
		}
		OnExit();
	}

protected:

	StateMachine<OwnerType>* stateMachine_ = nullptr;
	OwnerType* pOwner_ = nullptr;
};


