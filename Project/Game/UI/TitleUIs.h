#pragma once
#include <memory>
#include "Engine/Module/Components/2d/Sprite.h"

class TitleUIs {
public:

	TitleUIs() = default;
	~TitleUIs() = default;

	void Init();
	void Update();
	void Draw() const;

private:

	std::unique_ptr<Sprite> gameTitle_;

};

