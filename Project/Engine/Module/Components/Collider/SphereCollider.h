#pragma once
#include "Engine/Module/Components/Collider/BaseCollider.h"

/// <summary>
/// 球型のコライダー
/// </summary>
class SphereCollider
	: public BaseCollider {
public: // コンストラクタ

	SphereCollider();
	~SphereCollider();

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="categoryName"></param>
	/// <param name="shape"></param>
	void Init(const std::string& categoryName, ColliderShape shape) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="srt"></param>
	void Update(const QuaternionSRT& srt) override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const override;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

public:

	// ------------ 半径 ------------ // 
	void SetRadius(const float& radius) { std::get<Sphere>(shape_).radius = radius; }
	float GetRadius() const { return std::get<Sphere>(shape_).radius; }

private:



};

