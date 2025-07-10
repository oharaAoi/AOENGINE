#pragma once
#include <memory>
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Actor/Weapon/Sword.h"

class TestObject
	: public BaseEntity {
public:

	TestObject();
	~TestObject();

	void Finalize();
	void Init();
	void Update();
	
	void Debug_Gui() override;

private:

	std::unique_ptr<Sword> sword_;
	Matrix4x4 swordMat_;

};

