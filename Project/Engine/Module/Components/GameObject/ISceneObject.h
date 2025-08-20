#pragma once
#include "Engine/Module/Components/Attribute/AttributeGui.h"

class ISceneObject : 
	public AttributeGui {
public:

	ISceneObject() = default;
	virtual ~ISceneObject() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void PostUpdate() = 0;

	virtual void PreDraw() const = 0;
	virtual void Draw() const = 0;

	virtual void Debug_Gui() = 0;
	
	virtual void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) = 0;

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; }
	bool GetIsDestroy() const { return isDestroy_; }

protected:

	bool isDestroy_;

};