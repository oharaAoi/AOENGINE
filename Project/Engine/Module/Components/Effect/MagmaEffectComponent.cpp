#include "MagmaEffectComponent.h"
#include <algorithm>
#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Render/Render.h"
using namespace AOENGINE;
MagmaEffectComponent::MagmaEffectComponent() = default;
MagmaEffectComponent::~MagmaEffectComponent() = default;
void MagmaEffectComponent::Init(BaseGameObject& owner) {
    PlaneGeometry geometry;
    geometry.Init({0.5f, 0.5f});
    auto vertices = geometry.GetVertex();
    for (auto& vertex : vertices) {
        vertex.pos.z = vertex.pos.y;
        vertex.pos.y = 0.0f;
        vertex.normal = {0.0f, 1.0f, 0.0f};
    }
    mesh_ = std::make_unique<Mesh>();
    mesh_->Init(GraphicsContext::GetInstance()->GetDevice(), vertices, geometry.GetIndex());
    transform_ = std::make_unique<WorldTransform>();
    transform_->Init();
    buffer_ = CreateBufferResource(GraphicsContext::GetInstance()->GetDevice(), sizeof(Parameters));
    buffer_->Map(0, nullptr, reinterpret_cast<void**>(&parameters_));
    Update(owner, true);
}
void MagmaEffectComponent::SetSettings(const MagmaEffectSettings& settings) {
    settings_ = settings;
    settings_.size = (std::max)(settings_.size, 0.001f);
    settings_.patternScale = std::clamp(settings_.patternScale, 1.0f, 512.0f);
    settings_.temperature = std::clamp(settings_.temperature, 1.0f, 60.0f);
    settings_.emissiveIntensity = std::clamp(settings_.emissiveIntensity, 0.0f, 100.0f);
}
void MagmaEffectComponent::Update(const BaseGameObject& owner, bool editor) {
    if (!transform_ || !owner.GetTransform()) return;
    (void)editor;
    if (settings_.enabled) elapsed_ += (std::max)(GameTimer::DeltaTime(), 0.0f);
    transform_->PostUpdate();
    transform_->SetParent(owner.GetTransform()->GetWorldMatrix());
    transform_->SetScale({settings_.size, 1.0f, settings_.size});
    transform_->SetTranslate(settings_.localPosition);
    transform_->Update();
    *parameters_ = {elapsed_ * settings_.flowSpeed, settings_.patternScale,
        settings_.temperature, settings_.emissiveIntensity};
}
void MagmaEffectComponent::Draw() const {
    if (!settings_.enabled || !mesh_) return;
    auto* pipeline = Engine::SetPipeline(PSOType::Object3d, "Object_Magma.json");
    auto* cmd = GraphicsContext::GetInstance()->GetCommandList();
    mesh_->Bind(cmd);
    transform_->BindCommandList(cmd, pipeline->GetRootSignatureIndex("gWorldTransformMatrix"));
    Render::GetViewProjection()->BindCommandList(cmd, pipeline->GetRootSignatureIndex("gViewProjectionMatrix"));
    Render::GetViewProjection()->BindCommandListPrev(cmd, pipeline->GetRootSignatureIndex("gViewProjectionMatrixPrev"));
    cmd->SetGraphicsRootConstantBufferView(pipeline->GetRootSignatureIndex("gMagma"), buffer_->GetGPUVirtualAddress());
    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}
