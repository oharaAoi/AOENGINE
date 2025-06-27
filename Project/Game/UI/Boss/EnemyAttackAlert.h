#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 敵が攻撃してくる際のアラート表示
/// </summary>
class EnemyAttackAlert :
	public AttributeGui {
public:

	EnemyAttackAlert() = default;
	~EnemyAttackAlert() = default;

	void Init();

	void Update();

	void Draw() const;

	void Debug_Gui() override;

public:

	void Alert();

	bool IsDestroy();

private:

	std::unique_ptr<Sprite> alert_;

	Vector2 scale_ = Vector2{ 0.5f, 0.5f };
	Vector2 centerPos_ = Vector2{ 640.0f, 360.0f };

	bool isDraw_;
	
	float timer_;
	float blinkingTime_ = 0.2f;
	uint32_t blinkingCount_;

};

