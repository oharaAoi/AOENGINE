#pragma once
#include <memory>
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class TitleUIs :
	public AttributeGui {
public:

	TitleUIs() = default;
	~TitleUIs() = default;

	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui() override;

private:

	std::unique_ptr<Sprite> gameTitle_;

};

