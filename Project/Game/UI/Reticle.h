#pragma once
#include <memory>
#include "Engine/Components/2d/Sprite.h"

class Reticle {
public:

	Reticle() = default;
	~Reticle() = default;

	void Init();
	void Update();
	void Draw() const;

private:

	std::unique_ptr<Sprite> reticle_;

};

