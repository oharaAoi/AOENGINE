#pragma once
#include "Game/Scene/BaseScene.h"

class TestScene
: public BaseScene {
public:

	TestScene() = default;
	~TestScene() override;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void OnPlayStart() override;
	
private:

};
