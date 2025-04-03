#pragma once
// Engine
#include "Engine/Components/GameObject/BaseGameObject.h"

class Boss :
	public BaseGameObject {
public:

	Boss() = default;
	~Boss() = default;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

private:



};

