#pragma once
#include "Engine/Module/Components/Collider/ICollider.h"
#include <string>

/// <summary>
/// 箱型Collider
/// </summary>
class BoxCollider 
	: public ICollider {
public: // コンストラクタ

	BoxCollider();
	~BoxCollider();

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
	void Update(const QuaternionSRT& srt) override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() const override;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

private:

};
