#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"

class Skydome 
	: public BaseEntity {
public:

	Skydome();
	~Skydome();

	void Init();

	void Debug_Gui() override {};
	
private:


};

