#pragma once
#include "Game/Camera/BaseCamera.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// 3d空間上に配置するカメラ
/// </summary>
class Camera3d :
	public BaseCamera,
	public AttributeGui{
public:

	Camera3d();
	~Camera3d() override;

public:

	// 終了
	void Finalize() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 編集
	void Debug_Gui() override;

private:

};

