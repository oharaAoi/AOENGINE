#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <string>
#include "Engine/Module/Components/Collider/BaseCollider.h"

/// <summary>
/// 箱型Collider
/// </summary>
class BoxCollider 
	: public BaseCollider {
public: // コンストラクタ

	BoxCollider();
	~BoxCollider() override;

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

public:

	void SetSize(const Vector3& _size) { size_ = _size; }
	const Vector3& GetSize() const { return size_; }

private:

	Vector3 size_;

};
