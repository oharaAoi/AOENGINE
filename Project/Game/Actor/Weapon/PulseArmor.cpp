#include "PulseArmor.h"
#include "Engine/Engine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Information/ColliderCategory.h"

PulseArmor::~PulseArmor() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Init() {
	armorParam_.FromJson(JsonItems::GetData(GetName(), armorParam_.GetName()));
	geometry_.Init(CVector2::UNIT, 16, "armor");

	// meshの設定
	std::string name = geometry_.GetGeometryName();
	if (!MeshManager::GetInstance()->ExistMesh(name)) {
		mesh_ = std::make_shared<Mesh>();
		mesh_->Init(GraphicsContext::GetInstance()->GetDevice(), geometry_.GetVertex(), geometry_.GetIndex());
		MeshManager::GetInstance()->AddMesh(GraphicsContext::GetInstance()->GetDevice(), name, name, mesh_->GetVerticesData(), mesh_->GetIndices());
	} else {
		mesh_ = MeshManager::GetInstance()->GetMesh(name);
	}

	// material/worldTransformに関する設定
	material_ = std::make_unique<Material>();
	material_->Init();
	worldTransform_ = Engine::CreateWorldTransform();
	material_->SetAlbedoTexture(armorParam_.baseTexture);

	// dissolvebufferに関する設定
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(DissolveSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	// uvSrtの初期化
	for (size_t index = 0; index < 3; ++index) {
		uvSrt_[index].scale = CVector3::UNIT;
		uvSrt_[index].rotate = CVector3::ZERO;
		uvSrt_[index].translate = CVector3::ZERO;

		setting_->uvTransform[index] = uvSrt_[index].MakeAffine();
	}

	// bufferPtrの初期化
	setting_->color = Color::white;
	setting_->edgeColor = Color::white;
	setting_->threshold = 0.02f;

	noiseTexture_[0] = "noise0.png";
	noiseTexture_[1] = "noise5.png";
	noiseTexture_[2] = "noise6.png";

	for (size_t index = 0; index < 3; ++index) {
		Vector3 min = RandomVector3(CVector3::UNIT * -5.0f, CVector3::UNIT * 5.0f);
		Vector3 max = RandomVector3(CVector3::UNIT * 5.0f, CVector3::UNIT * 5.0f);
		uvMovingTween_[index].Init(min, max, RandomFloat(100.0f, 200.0f), (int)EasingType::None::Liner, LoopType::RETURN);
	}

	thresholdTween_.Init(armorParam_.minThreshold, armorParam_.maxThreshold, 4.0f, (int)EasingType::None::Liner, LoopType::RETURN);

	isAlive_ = false;
	worldTransform_->SetScale(CVector3::ZERO);
	//SetArmor(armorParam_.durability, armorParam_.scale, armorParam_.color, armorParam_.edgeColor, armorParam_.uvTransform);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Update() {
	for (size_t index = 0; index < 3; ++index) {
		uvSrt_[index].translate = uvMovingTween_[index].GetValue();
		uvMovingTween_[index].Update(GameTimer::DeltaTime());
		setting_->uvTransform[index] = uvSrt_[index].MakeAffine();
	}

	thresholdTween_.Update(GameTimer::DeltaTime());
	setting_->threshold = thresholdTween_.GetValue();

	worldTransform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Draw() const {
	if (!isAlive_) { return; }

	Engine::SetPipeline(PSOType::Object3d, "Object_Dissolve.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();

	// VS
	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());

	UINT index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	Render::GetInstance()->GetViewProjection()->BindCommandListPrev(commandList, index);

	// PS
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetAlbedoTexture(), index);
	index = pso->GetRootSignatureIndex("gMaskTexture1");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_[0], index);
	index = pso->GetRootSignatureIndex("gMaskTexture2");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_[1], index);
	index = pso->GetRootSignatureIndex("gMaskTexture3");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_[2], index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Debug_Gui() {
	worldTransform_->Debug_Gui();
	material_->Debug_Gui();

	// dissolveに関する設定を行う
	if (ImGui::CollapsingHeader("Setting")) {
		ImGui::ColorEdit4("color", &setting_->color.r);
		ImGui::ColorEdit4("edgeColor", &setting_->edgeColor.r);
		ImGui::DragFloat("threshold", &setting_->threshold, 0.01f);

		for (size_t index = 0; index < 3; ++index) {
			std::string name = "Noise" + std::to_string(index);
			if (ImGui::TreeNode(name.c_str())) {
				noiseTexture_[index] = TextureManager::GetInstance()->SelectTexture(noiseTexture_[index]);
				ImGui::TreePop();
			}
		}

		setting_->threshold = std::clamp(setting_->threshold, 0.0f, 1.0f);
	}

	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::DragFloat("durability", &armorParam_.durability, 0.1f);
		ImGui::DragFloat3("scale", &armorParam_.scale.x, 0.1f);

		ImGui::DragFloat("minThreshold", &armorParam_.minThreshold, 0.01f);
		ImGui::DragFloat("maxThreshold", &armorParam_.maxThreshold, 0.01f);
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::TreeNodeEx("uvTransform", flags)) {
			if (ImGui::TreeNode("scale")) {
				ImGui::DragFloat3("uvScale", &armorParam_.uvTransform.scale.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("rotation")) {
				ImGui::DragFloat3("uvRotation", &armorParam_.uvTransform.rotate.x, 0.01f);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (ImGui::Button("Save")) {
			armorParam_.color = setting_->color;
			armorParam_.edgeColor = setting_->edgeColor;
			armorParam_.baseTexture = material_->GetAlbedoTexture();
			armorParam_.noiseTexture1 = noiseTexture_[0];
			armorParam_.noiseTexture2 = noiseTexture_[1];
			armorParam_.noiseTexture3 = noiseTexture_[2];
			JsonItems::Save(GetName(), armorParam_.ToJson(armorParam_.GetName()));
		}

		if (ImGui::Button("Applay")) {
			armorParam_.scale = worldTransform_->GetScale();
			armorParam_.localTranslate = worldTransform_->GetPos();
			armorParam_.color = setting_->color;
			armorParam_.edgeColor = setting_->edgeColor;
			armorParam_.uvTransform.scale = uvSrt_[0].scale;
			armorParam_.uvTransform.rotate = uvSrt_[0].rotate;
			thresholdTween_.Init(armorParam_.minThreshold, armorParam_.maxThreshold, RandomFloat(4.0f, 8.0f), (int)EasingType::None::Liner, LoopType::RETURN);
			SetParameter();
		}
	}
}

void PulseArmor::ArmorParameter::Debug_Gui() {

}

void PulseArmor::SetParameter() {
	SetArmor(armorParam_.durability, armorParam_.scale, armorParam_.color, armorParam_.edgeColor, armorParam_.uvTransform);

	noiseTexture_[0] = armorParam_.noiseTexture1;
	noiseTexture_[1] = armorParam_.noiseTexture2;
	noiseTexture_[2] = armorParam_.noiseTexture3;

	material_->SetAlbedoTexture(armorParam_.baseTexture);
}

void PulseArmor::SetArmor(float _durability, const Vector3& _scale, const Color& _color, const Color& _edgeColor, const SRT& _uvSrt) {
	durability_ = _durability;
	initDurability_ = _durability;
	worldTransform_->SetScale(_scale);
	worldTransform_->SetTranslate(armorParam_.localTranslate);
	setting_->color = _color;
	setting_->edgeColor = _edgeColor;

	for (size_t index = 0; index < 3; ++index) {
		uvSrt_[index].scale = _uvSrt.scale;
		uvSrt_[index].rotate = _uvSrt.rotate;
		uvSrt_[index].translate = _uvSrt.translate;
	}

	isAlive_ = true;
}

void PulseArmor::DamageDurability(float _damage) {
	durability_ -= _damage;
}

bool PulseArmor::BreakArmor() {
	if (durability_ <= 0.0f) {
		isAlive_ = false;
		return true;
	}
	return false;
}
