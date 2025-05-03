#pragma once
#include <memory>
#include <functional>
#include <Game/State/ICharacterState.h>

/// <summary>
/// 状態を管理するクラス
/// </summary>
/// <typeparam name="OwnerType">:対象の型</typeparam>
template<typename OwnerType>
class StateMachine {
public:

	StateMachine() : functionChangeState_([]() {}) {}

	// 開始関数
	void Init(OwnerType* pOwner) {
		pOwner_ = pOwner;
		functionChangeState_ = []() {};
	}

	/// <summary>
	/// 変更する関数
	/// </summary>
	/// <typeparam name="StateType">: コンストラクタに渡す値</typeparam>
	/// <typeparam name="...ArgType">: 可変長引数</typeparam>
	/// <param name="...args"></param>
	template<typename StateType, typename...ArgType>
	void ChangeState(ArgType...args) {
		// ステートの変更命令を格納する
		functionChangeState_ = [&]() {
			if (pOwner_ == nullptr) { return; }

			// stateがセットされていたら終了処理をする
			if (state_ != nullptr) {
				state_->CallExit(pOwner_);
			}

			// 新しいステートを作成
			state_ = std::make_unique<StateType>(args...);
			if (state_ == nullptr) { return; }

			// 新しいステートにマシーンをセット
			state_->SetMachine(this);
			// 開始処理
			state_->CallStart(pOwner_);

			functionChangeState_ = []() {};
			};
	}

	void Update() {
		// ステートの変更命令があれば処理する
		functionChangeState_();

		if (state_ != nullptr) {
			state_->CallUpdate(pOwner_);
		}
	}

private:
	// 持ち主のポインタ
	OwnerType* pOwner_ = nullptr;
	// 今のステート
	std::unique_ptr<ICharacterState<OwnerType>> state_ = nullptr;
	// ステートの変更命令保存しておく関数オブジェクト
	std::function<void()> functionChangeState_;
};
