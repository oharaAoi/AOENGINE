#pragma once
#include <list>
#include "Engine/Module/Components/2d/Sprite.h"

class Canvas2d {
public:

	Canvas2d() = default;
	~Canvas2d();

	void Init();

	void Update();

	void Draw() const;

public:

	void AddSprite(Sprite* _sprite) { spriteList_.push_back(_sprite); }

	void DeleteSprite(Sprite* _sprite);

private:

	std::list<Sprite*> spriteList_;

};

