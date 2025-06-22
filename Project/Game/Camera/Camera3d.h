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

	void Finalize() override;
	void Init() override;
	void Update() override;

	void Debug_Gui() override;

private:

};

