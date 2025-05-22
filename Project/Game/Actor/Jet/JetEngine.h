#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

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

private:

	// effects -------------------------------------------------
	BaseParticles* jetParticles_;
	BaseParticles* jetBornParticles_;
	BaseParticles* jetEnergyParticles_;


};

