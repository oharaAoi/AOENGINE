#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
// Effect
#include "Game/Effects/JetParticles.h"
#include "Game/Effects/JetBornParticles.h"
#include "Game/Effects/JetEnergyParticles.h"

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
	std::unique_ptr<JetParticles> jetParticles_;
	std::unique_ptr<JetBornParticles> jetBornParticles_;
	std::unique_ptr<JetEnergyParticles> jetEnergyParticles_;


};

