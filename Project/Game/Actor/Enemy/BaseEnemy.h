#pragma once
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"

enum class EnemyType {
	Short,		// 近距離
	Mid,		// 中距離
	Long,		// 遠距離
	Boss
};

enum class EnemyState {
	Normal,	// 通常状態
	Stan,	// スタン状態
	Armor	// アーマー状態
};

/// <summary>
/// Enemyの基底となるクラス
/// </summary>
class BaseEnemy :
	public AOENGINE::BaseEntity {
public: // データ構造体

	/// <summary>
	/// Enemyの基本のパラメータ
	/// </summary>
	struct BaseParameter : public AOENGINE::IJsonConverter {
		float health = 100;
		float postureStability = 100.0f;
		float postureStabilityScrapeRaito = 0.3f;	// 耐久度を削る割合
		
		BaseParameter() { SetName("EnemyBaseParameter"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("health", health)
				.Add("postureStability", postureStability)
				.Add("postureStabilityScrapeRaito", postureStabilityScrapeRaito)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "health", health);
			Convert::fromJson(jsonData, "postureStability", postureStability);
			Convert::fromJson(jsonData, "postureStabilityScrapeRaito", postureStabilityScrapeRaito);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	BaseEnemy() = default;
	~BaseEnemy() = default;

public: // method

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;
	
	/// <summary>
	/// 編集処理
	/// </summary>
	virtual void Debug_Gui() override;

public: // accessor

	void SetBaseParameter(const BaseParameter param) { baseParam_ = param; }
	const BaseParameter& GetBaseParameter() const { return baseParam_; }

	const BaseParameter& GetInitBaseParameter() const { return initBaseParam_; }

	EnemyType GetEnemyType() const { return type_; }

	void SetEnemyState(EnemyState state) { enemyState_ = state; }
	EnemyState GetEnemyState() const { return enemyState_; }

	void SetIsWithinSight(bool in) { isWithinSight_ = in; }
	bool GetIsWithinSight() const { return isWithinSight_; }

protected:

	BaseParameter baseParam_;
	BaseParameter initBaseParam_;

	EnemyState enemyState_;

	EnemyType type_;

private:

	bool isWithinSight_; // カメラ内に収まっているかを判定する

};

