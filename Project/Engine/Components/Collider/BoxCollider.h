#pragma once
#include "Engine/Components/Collider/ICollider.h"
#include <string>

class BoxCollider 
	: public ICollider {
public:

	BoxCollider();
	~BoxCollider();

	void Init(uint32_t bitTag, ColliderShape shape) override;
	void Update(const QuaternionSRT& srt) override;
	void Draw() const override;

private:

};
