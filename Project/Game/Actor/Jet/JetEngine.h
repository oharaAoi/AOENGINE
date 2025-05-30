#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/GameObject/GeometryObject.h"

#include "Game/Effects/JetEngineBurn.h"

/// <summary>
/// jetのエンジンとなるクラス
/// </summary>
class JetEngine :
public BaseGameObject {
public:

	JetEngine() = default;
	~JetEngine();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

	void JetIsStop();

	void JetIsStart();

public:	// accessor Method

	void SetIsBoostMode() { isBoostMode_ = !isBoostMode_; }

	bool GetIsBoostMode() const { return isBoostMode_; }

private:

	// Parameter -------------------------------------------------
	bool isBoostMode_;

	// effects -------------------------------------------------
	BaseParticles* jetBurn_;
	BaseParticles* jetBornParticles_;
	BaseParticles* jetEnergyParticles_;

	std::unique_ptr<GeometryObject> cylinder_;

	std::unique_ptr<JetEngineBurn> jetEngineBurn_;

};