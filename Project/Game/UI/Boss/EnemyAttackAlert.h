#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

enum class AttackAlertDirection {
	Front,
	Back,
	Left,
	Right
};

/// <summary>
/// 敵が攻撃してくる際のアラート表示
/// </summary>
class EnemyAttackAlert :
	public AttributeGui {
public:

	EnemyAttackAlert() = default;
	~EnemyAttackAlert() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_dir"></param>
	void Init(AttackAlertDirection _dir);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

public:

	/// <summary>
	/// 警告
	/// </summary>
	void Alert();

	/// <summary>
	/// 破壊処理
	/// </summary>
	/// <returns></returns>
	bool IsDestroy();

private:

	Sprite* alert_;

	Vector2 scale_ = Vector2{ 0.6f, 0.6f };
	Vector2 centerPos_ = Vector2{ 640.0f, 360.0f };

	bool isDraw_;
	
	float timer_;
	float blinkingTime_ = 0.1f;
	uint32_t blinkingCount_;

};

