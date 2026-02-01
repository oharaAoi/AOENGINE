#pragma once
// c++
#include <string>
// engine
#include "Engine/Lib/Json/IJsonConverter.h"
// game
#include "Game/Actor/Enemy/BaseEnemy.h"

/// <summary>
/// 中距離の敵クラス
/// </summary>
class MidRangeEnemy :
	public BaseEnemy {
public:

	/// <summary>
	/// 基礎的なパラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float searchRange = 1.0f;						 // 索敵の範囲
		float colliderRadius = 1.f;						 // コライダーの大きさ
		Math::Vector3 colliderLocalPos = CVector3::ZERO; // colliderのローカル座標
		Math::Vector3 weaponOffset = CVector3::ZERO;	 // 武器のoffset
		Math::Vector3 weaponRotate;						 // 武器のRotate
		Math::Vector3 translateOffset;					 // 座標のオフセット

		std::string worldStatePath = "";				 // worldStateのパス

		Parameter() {
			SetGroupName("Enemy");
			SetName("MidRangeEnemy");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("searchRange", searchRange)
				.Add("colliderRadius", colliderRadius)
				.Add("colliderLocalPos", colliderLocalPos)
				.Add("weaponOffset", weaponOffset)
				.Add("weaponRotate", weaponRotate)
				.Add("translateOffset", translateOffset)
				.Add("worldStatePath", worldStatePath)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "searchRange", searchRange);
			Convert::fromJson(jsonData, "colliderRadius", colliderRadius);
			Convert::fromJson(jsonData, "colliderLocalPos", colliderLocalPos);
			Convert::fromJson(jsonData, "weaponOffset", weaponOffset);
			Convert::fromJson(jsonData, "weaponRotate", weaponRotate);
			Convert::fromJson(jsonData, "translateOffset", translateOffset);
			Convert::fromJson(jsonData, "worldStatePath", worldStatePath);
		}

		void Debug_Gui() override;
	};
public: // コンストラクタ

	MidRangeEnemy() = default;
	~MidRangeEnemy() = default;

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

