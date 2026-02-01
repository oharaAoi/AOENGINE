#pragma once
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/System/AI/BehaviorTree.h"
// game
#include "Game/Actor/Enemy/EnemyBulletManager.h"
#include "Game/Actor/Weapon/BaseWeapon.h"

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
		float angularVelocity = 90.f; // 角速度
		float angularThreshold = 10.f; // 角速度

		BaseParameter() { SetName("EnemyBaseParameter"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("health", health)
				.Add("postureStability", postureStability)
				.Add("postureStabilityScrapeRaito", postureStabilityScrapeRaito)
				.Add("angularVelocity", angularVelocity)
				.Add("angularThreshold", angularThreshold)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "health", health);
			Convert::fromJson(jsonData, "postureStability", postureStability);
			Convert::fromJson(jsonData, "postureStabilityScrapeRaito", postureStabilityScrapeRaito);
			Convert::fromJson(jsonData, "angularVelocity", angularVelocity);
			Convert::fromJson(jsonData, "angularThreshold", angularThreshold);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	BaseEnemy() ;
	~BaseEnemy();

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

	/// <summary>
	/// 共通初期化処理
	/// </summary>
	void InitCommon();

	/// <summary>
	/// Targetの方向を向く処理
	/// </summary>
	bool TargetLook();

	/// <summary>
	/// ダメージを食らう処理
	/// </summary>
	/// <param name="takeDamage"></param>
	void Damage(float takeDamage);

public: // accessor

	void SetBaseParameter(const BaseParameter param) { baseParam_ = param; }
	const BaseParameter& GetBaseParameter() const { return baseParam_; }

	const BaseParameter& GetInitBaseParameter() const { return initBaseParam_; }

	EnemyType GetEnemyType() const { return type_; }

	void SetEnemyState(EnemyState state) { enemyState_ = state; }
	EnemyState GetEnemyState() const { return enemyState_; }

	void SetIsWithinSight(bool in) { isWithinSight_ = in; }
	bool GetIsWithinSight() const { return isWithinSight_; }

	void SetIsDead(bool isDead) { isDead_ = isDead; }
	bool GetIsDead() const { return isDead_; }

	void SetEnemyBulletManager(EnemyBulletManager* enemyBulletManager) { pEnemyBulletManager_ = enemyBulletManager; }
	EnemyBulletManager* GetEnemyBulletManager() const { return pEnemyBulletManager_; }

	void SetIsTargetDiscovery(bool isTargetDiscovery) { isTargetDiscovery_ = isTargetDiscovery; }
	bool GetIsTargetDiscovery() const { return isTargetDiscovery_; }

	void SetTargetTransform(AOENGINE::WorldTransform* worldTransform) { targetTransform_ = worldTransform; }
	AOENGINE::WorldTransform* GetTargetTransform() const { return targetTransform_; }

	BaseWeapon* GetWeapon() const { return weapon_.get(); }

protected:

	// パラメータ
	BaseParameter baseParam_;
	BaseParameter initBaseParam_;

	// enemyのステート
	EnemyState enemyState_;

	// enmeyの種類
	EnemyType type_;

	// 右手の行列
	Math::Matrix4x4 rightHandMatrix_;

	// wapon
	std::unique_ptr<BaseWeapon> weapon_;

	// 他クラスポインタ
	EnemyBulletManager* pEnemyBulletManager_ = nullptr;

	// AI関連
	AI::BehaviorTree* behaviorTree_;
	std::unique_ptr<AI::Blackboard> blackboard_;

	// searchCollider
	AOENGINE::BaseCollider* searchCollider_;

	AOENGINE::WorldTransform* targetTransform_;

	bool isTargetDiscovery_ = false;

private:

	bool isWithinSight_; // カメラ内に収まっているかを判定する

	bool isDead_ = false;

};

