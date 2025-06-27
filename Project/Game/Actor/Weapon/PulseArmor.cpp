#include "PulseArmor.h"
#include "Engine/Engine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Core/GraphicsContext.h"
#include "Game/Information/ColliderCategory.h"

PulseArmor::~PulseArmor() {
	
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Init() {
	SetName("PulseArmor");
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
	material_ = Engine::CreateMaterial(Model::ModelMaterialData());
	worldTransform_ = Engine::CreateWorldTransform();
	material_->SetUseTexture("white.png");

	// dissolvebufferに関する設定
	GraphicsContext* graphicsCtx = GraphicsContext::GetInstance();
	settingBuffer_ = std::make_unique<DxResource>();
	settingBuffer_->Init(graphicsCtx->GetDevice(), graphicsCtx->GetDxHeap(), ResourceType::COMMON);
	settingBuffer_->CreateResource(sizeof(DissolveSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	// uvSrtの初期化
	uvSrt_.scale = CVector3::UNIT;
	uvSrt_.rotate = CVector3::ZERO;
	uvSrt_.translate = CVector3::ZERO;

	// bufferPtrの初期化
	setting_->uvTransform = uvSrt_.MakeAffine();
	setting_->color = Vector4(CVector3::UNIT, 1.0f);
	setting_->edgeColor = Vector4(CVector3::UNIT, 1.0f);
	setting_->threshold = 0.02f;

	noiseTexture_ = "noise1.png";

	uvMovingTween_.Init(&uvMovingValue_, Vector3(-5.0f, 0.0f, 0.0f), Vector3(5.0f, 0.0f, 0.0f), 100.0f, (int)EasingType::None::Liner, LoopType::LOOP);

	isAlive_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Update() {
	uvMovingTween_.Update(GameTimer::DeltaTime());
	uvSrt_.translate = uvMovingValue_;

	setting_->uvTransform = uvSrt_.MakeAffine();
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

	// PS
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetUseTexture(), index);
	index = pso->GetRootSignatureIndex("gMaskTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_, index);

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
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Framed;
		if (ImGui::TreeNodeEx("uvTransform", flags)) {
			if (ImGui::TreeNode("scale")) {
				ImGui::DragFloat4("uvScale", &uvSrt_.scale.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("rotation")) {
				ImGui::DragFloat4("uvRotation", &uvSrt_.rotate.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("translation")) {
				ImGui::DragFloat4("uvTranslation", &uvSrt_.translate.x, 0.01f);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
		ImGui::ColorEdit4("color", &setting_->color.x);
		ImGui::ColorEdit4("edgeColor", &setting_->edgeColor.x);
		ImGui::DragFloat("threshold", &setting_->threshold, 0.01f);

		noiseTexture_ = TextureManager::GetInstance()->SelectTexture(noiseTexture_);

		setting_->threshold = std::clamp(setting_->threshold, 0.0f, 1.0f);
	}
}

void PulseArmor::SetArmor(float _durability, const Vector3& _scale, const Vector4& _color, const Vector4& _edgeColor, const SRT& _uvSrt) {
	durability_ = _durability;
	initDurability_ = _durability;
	worldTransform_->SetScale(_scale);
	setting_->color = _color;
	setting_->edgeColor = _edgeColor;

	uvSrt_.scale = _uvSrt.scale;
	uvSrt_.rotate = _uvSrt.rotate;
	uvSrt_.translate = _uvSrt.translate;

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
