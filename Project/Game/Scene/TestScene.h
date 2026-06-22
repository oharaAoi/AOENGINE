#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"

class TestScene 
: public BaseScene {
public:


public:

	TestScene();
	~TestScene() override;

	void Finalize() override;
	void Init() override;
	void Update() override;
	
private:


};

