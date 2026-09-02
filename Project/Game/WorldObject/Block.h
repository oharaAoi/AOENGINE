#pragma once

#include "Engine/Module/Components/GameObject/BaseEntity.h"

/// <summary>
/// ステージ上の足場 としての tagクラス
/// </summary>
class Block : public AOENGINE::BaseEntity{
private:
	
	bool isConnected = false;
public:
	bool GetIsConnected() const{ return isConnected; }
	void SetIsConnected(bool value){ isConnected = value; }
};