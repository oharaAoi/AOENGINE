#pragma once
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include <string>

/// <summary>
/// Box型のCollider
/// </summary>
class LineCollider : 
	public BaseCollider {
public: // コンストラクタ

	LineCollider() = default;
	~LineCollider() override = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="categoryName"></param>
	/// <param name="shape"></param>
	void Init(const std::string& categoryName, ColliderShape shape) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="srt"></param>
	void Update(const Math::QuaternionSRT& srt) override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const override;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;
	
public:

	void SetDiff(const Math::Vector3& _diff);
	const Math::Vector3& GetDiff() const;

	const Math::Vector3& GetOrigine() const;

};

