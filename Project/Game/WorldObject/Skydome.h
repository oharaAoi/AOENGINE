#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"

/// <summary>
/// skydome
/// </summary>
class Skydome 
	: public BaseEntity {
public:

	Skydome();
	~Skydome();

public:

	// 初期化
	void Init();
	// 編集
	void Debug_Gui() override {};
	
private:


};

