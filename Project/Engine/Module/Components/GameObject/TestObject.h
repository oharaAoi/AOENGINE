#pragma once
#include <memory>
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class TestObject
	: public BaseEntity {
public:

	TestObject();
	~TestObject();

	void Finalize();
	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui() override;

private:

};

