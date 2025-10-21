#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/GameObject/GeometryObject.h"

#include "Game/Effects/JetEngineBurn.h"

/// <summary>
/// jetのエンジンとなるクラス
/// </summary>
class JetEngine :
	public BaseEntity {
public:

	JetEngine() = default;
	~JetEngine();

	void Finalize();
	void Init();
	void Update();
	
	void Debug_Gui() override;

	void JetIsStop();

	void JetIsStart();

	void BoostOn() { isBoostMode_ = true; }

public:	// accessor Method

	void SetIsBoostMode() { isBoostMode_ = !isBoostMode_; }

	bool GetIsBoostMode() const { return isBoostMode_; }

private:

	// Parameter -------------------------------------------------
	bool isBoostMode_;

	// effects -------------------------------------------------
	BaseParticles* jetBurn_;
	BaseParticles* jetEnergyParticles_;

	JetEngineBurn* jetEngineBurn_;
	JetEngineBurn* jetEngineBurn_2;

};