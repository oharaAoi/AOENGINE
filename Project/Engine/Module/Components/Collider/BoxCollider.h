#pragma once
#include "Engine/Module/Components/Collider/ICollider.h"
#include <string>

class BoxCollider 
	: public ICollider {
public:

	BoxCollider();
	~BoxCollider();

	void Init(const std::string& categoryName, ColliderShape shape) override;
	void Update(const QuaternionSRT& srt) override;
	void Draw() const override;

	void Debug_Gui() override;

private:

};
