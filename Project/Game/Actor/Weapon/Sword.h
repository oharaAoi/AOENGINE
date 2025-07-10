#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"

class Sword 
	: public BaseEntity {
public:

	Sword() = default;
	~Sword() = default;

	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui() override;


};

