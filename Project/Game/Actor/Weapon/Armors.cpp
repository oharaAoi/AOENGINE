#include "Armors.h"
#include "Engine/Render/SceneRenderer.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Armors::Init(const std::string& ownerName) {
	SetName("Armors");
	outArmor_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<PulseArmor>("BossPulseArmor", "Object_Dissolve.json", 100);
	inArmor_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<PulseArmor>("BossPulseArmor", "Object_Dissolve.json", 100);
	
	outArmor_->SetName(ownerName + "_OutArmor");
	inArmor_->SetName(ownerName + "_InArmor");

	outArmor_->Init();
	inArmor_->Init();

	isDeploy_ = false;

	chargeEmitter_ = GpuParticleManager::GetInstance()->CreateEmitter("concentration");
	chargeEmitter_->SetIsStop(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Armors::Update() {
	outArmor_->Update();
	inArmor_->Update();

	if (!outArmor_->GetIsAlive()) {
		inArmor_->SetIsAlive(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Armors::Draw() const {
	if (!outArmor_->GetIsAlive()) { return; }

	outArmor_->Draw();
	inArmor_->Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 親の設定
///////////////////////////////////////////////////////////////////////////////////////////////

void Armors::SetParent(const Math::Matrix4x4& mat) {
	outArmor_->GetTransform()->SetParent(mat);
	inArmor_->GetTransform()->SetParent(mat);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Armors::Debug_Gui() {
	if (ImGui::TreeNode("Out")) {
		outArmor_->Debug_Gui();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("In")) {
		inArmor_->Debug_Gui();
		ImGui::TreePop();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ アーマーの設定
///////////////////////////////////////////////////////////////////////////////////////////////

void Armors::SetArmor() {
	isDeploy_ = true;
	outArmor_->SetParameter();
	inArmor_->SetParameter();
}

void Armors::SetArmorParam(float _durability, const Math::Vector3& _scale, const AOENGINE::Color& _color, const AOENGINE::Color& _edgeColor, const Math::SRT& _uvSrt) {
	outArmor_->SetArmor(_durability, _scale, _color, _edgeColor, _uvSrt);
	inArmor_->SetArmor(_durability, _scale, _color, _edgeColor, _uvSrt);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 破壊処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Armors::DamageDurability(float _damage) {
	outArmor_->DamageDurability(_damage);
	inArmor_->DamageDurability(_damage);
}

bool Armors::BreakArmor() {
	return outArmor_->BreakArmor();
}

float Armors::ArmorDurability() {
	return outArmor_->ArmorDurability();
}

void Armors::SetIsAlive(bool _isAlive) {
	outArmor_->SetIsAlive(_isAlive);
	inArmor_->SetIsAlive(_isAlive);
}