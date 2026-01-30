#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Enemy/BaseEnemy.h"

/// <summary>
/// 遠距離型の敵
/// </summary>
class LongRangeEnemy :
	public BaseEnemy {
public: // データ構造体

	struct Parameter : public AOENGINE::IJsonConverter {
		float colliderRadius = 1.f;						 // コライダーの大きさ
		Math::Vector3 colliderLocalPos = CVector3::ZERO; // colliderのローカル座標

		Parameter() {
			SetGroupName("Enemy");
			SetName("LongRangeEnemy");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("colliderRadius", colliderRadius)
				.Add("colliderLocalPos", colliderLocalPos)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "colliderRadius", colliderRadius);
			Convert::fromJson(jsonData, "colliderLocalPos", colliderLocalPos);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	LongRangeEnemy() = default;
	~LongRangeEnemy() = default;

public: // method

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 編集
	/// </summary>
	void Debug_Gui() override;

private:

	Parameter param_;

};

