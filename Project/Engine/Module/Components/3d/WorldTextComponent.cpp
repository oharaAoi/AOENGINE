#include "WorldTextComponent.h"

#include <algorithm>
#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Manager/FontManager.h"
#include "Engine/System/Manager/TextureManager.h"

using namespace AOENGINE;

uint64_t WorldTextComponent::nextRuntimeId_ = 1;

WorldTextComponent::WorldTextComponent() : runtimeId_(nextRuntimeId_++) {}
WorldTextComponent::~WorldTextComponent() { Finalize(); }

void WorldTextComponent::Init(BaseGameObject& owner) {
	Finalize();
	PlaneGeometry geometry;
	geometry.Init(Math::Vector2{ 1.0f, 1.0f });
	auto vertices = geometry.GetVertex();
	for (VertexData& vertex : vertices) {
		vertex.texcoord.y = 1.0f - vertex.texcoord.y;
	}
	planeMesh_ = std::make_shared<Mesh>();
	planeMesh_->Init(GraphicsContext::GetInstance()->GetDevice(), vertices, geometry.GetIndex());
	material_ = std::make_unique<Material>();
	material_->Init();
	material_->SetIsLighting(false);
	material_->SetDiscardValue(0.01f);
	transform_ = std::make_unique<WorldTransform>();
	transform_->Init();
	generatedTextureName_ = "__WorldText_" + std::to_string(runtimeId_);
	textureDirty_ = true;
	Update(owner);
}

void WorldTextComponent::Update(const BaseGameObject& owner) {
	if (!settings_.enabled || !planeMesh_ || !material_ || !transform_) { return; }
	if (textureDirty_) { RebuildTexture(); }
	material_->SetColor(settings_.color);
	material_->Update();
	UpdateTransform(owner);
}

void WorldTextComponent::Draw() const {
	if (!settings_.enabled || settings_.text.empty() || !planeMesh_ || !material_ || !transform_) { return; }
	Pipeline* pipeline = Engine::SetPipeline(
		PSOType::Object3d, settings_.depthTest ? "Object_WorldText.json" : "Object_WorldTextNoDepth.json");
	ID3D12GraphicsCommandList* commandList = GraphicsContext::GetInstance()->GetCommandList();
	Render::DrawLightGroup(pipeline);
	planeMesh_->Bind(commandList);
	UINT index = pipeline->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());
	index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
	transform_->BindCommandList(commandList, index);
	index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
	Render::GetViewProjection()->BindCommandList(commandList, index);
	index = pipeline->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	Render::GetViewProjection()->BindCommandListPrev(commandList, index);
	index = pipeline->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetAlbedoTexture(), index);
	index = pipeline->GetRootSignatureIndex("gShadowMap");
	commandList->SetGraphicsRootDescriptorTable(index, Render::GetShadowMap()->GetDeptSrvHandle().handleGPU);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexedInstanced(planeMesh_->GetIndexNum(), 1, 0, 0, 0);
}

void WorldTextComponent::Finalize() {
	if (!generatedTextureName_.empty()) {
		TextureManager::GetInstance()->ReleaseGeneratedTexture(generatedTextureName_);
		generatedTextureName_.clear();
	}
	transform_.reset();
	material_.reset();
	planeMesh_.reset();
}

void WorldTextComponent::SetText(const std::string& text) {
	if (settings_.text == text) { return; }
	settings_.text = text;
	textureDirty_ = true;
}

void WorldTextComponent::SetFontPath(const std::string& path) {
	if (settings_.fontPath == path) { return; }
	settings_.fontPath = path;
	textureDirty_ = true;
}

void WorldTextComponent::SetFontSize(float size) {
	size = std::clamp(size, 1.0f, 512.0f);
	if (settings_.fontSize == size) { return; }
	settings_.fontSize = size;
	textureDirty_ = true;
}

void WorldTextComponent::SetHeight(float height) { settings_.height = (std::max)(height, 0.001f); }

void WorldTextComponent::SetSettings(const WorldTextSettings& settings) {
	const bool rebuild = settings_.text != settings.text || settings_.fontPath != settings.fontPath || settings_.fontSize != settings.fontSize;
	settings_ = settings;
	settings_.fontSize = std::clamp(settings_.fontSize, 1.0f, 512.0f);
	settings_.height = (std::max)(settings_.height, 0.001f);
	textureDirty_ |= rebuild;
}

void WorldTextComponent::RebuildTexture() {
	const auto texture = FontManager::GetInstance()->CreateTextTexture(
		settings_.text, settings_.fontPath, settings_.fontSize, generatedTextureName_);
	generatedTextureSize_ = texture.size;
	material_->SetAlbedoTexture(texture.textureName);
	textureDirty_ = false;
}

void WorldTextComponent::UpdateTransform(const BaseGameObject& owner) {
	const WorldTransform* parent = owner.GetTransform();
	if (!parent) { return; }
	transform_->PostUpdate();
	const float aspect = generatedTextureSize_.y > 0.0f ? generatedTextureSize_.x / generatedTextureSize_.y : 1.0f;
	Math::QuaternionSRT srt{};
	const float horizontalDirection = settings_.billboardMode == WorldTextBillboardMode::FaceCamera ? -1.0f : 1.0f;
	srt.scale = { settings_.height * aspect * horizontalDirection, settings_.height, 1.0f };
	srt.translate = settings_.localPosition;
	srt.rotate = settings_.localRotation;
	transform_->SetSRT(srt);
	if (settings_.billboardMode == WorldTextBillboardMode::None) {
		transform_->SetBillBoard(false);
		transform_->SetParent(parent->GetWorldMatrix());
	} else {
		transform_->ClearParent();
		const Math::Vector3 worldPosition = TransformCoord(settings_.localPosition, parent->GetWorldMatrix());
		transform_->SetTranslate(worldPosition);
		transform_->SetBillBoard(settings_.billboardMode == WorldTextBillboardMode::FaceCamera);
	}
	if (settings_.billboardMode == WorldTextBillboardMode::YAxisOnly) {
		Math::Vector3 direction = Render::GetEyePos() - transform_->GetTranslate();
		direction.y = 0.0f;
		if (direction.Length() > 0.0001f) { transform_->SetRotate(Math::Quaternion::LookRotation(direction.Normalize())); }
	}
	transform_->Update();
}
