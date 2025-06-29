#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"

class FieldObject :
	public BaseEntity {
public:

	FieldObject() = default;
	~FieldObject() override = default;

	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui() override;

};

