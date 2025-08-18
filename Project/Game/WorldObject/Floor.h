#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components//WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class Floor :
	public AttributeGui {
public:

	Floor();
	~Floor();

	void Init();

	void Debug_Gui() override;

private:

	BaseGameObject* floor_;
	WorldTransform* transform_;

};

