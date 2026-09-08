#pragma once
#include <memory>
#include <d3d12.h>
#include <wrl.h>
#include "Engine/Module/Components/IComponent.h"
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {
class BaseGameObject;
class Mesh;
class WorldTransform;
struct MagmaEffectSettings {
    bool enabled = true;
    float size = 4.0f;
    float patternScale = 128.0f;
    float flowSpeed = 0.15f;
    float temperature = 30.0f;
    float emissiveIntensity = 1.0f;
    Math::Vector3 localPosition{0.0f, 0.01f, 0.0f};
};
class MagmaEffectComponent final : public IComponent {
public:
    MagmaEffectComponent();
    ~MagmaEffectComponent() override;
    void Init(BaseGameObject& owner);
    void Update(const BaseGameObject& owner, bool editor = false);
    void Draw() const;
    const MagmaEffectSettings& GetSettings() const { return settings_; }
    void SetSettings(const MagmaEffectSettings& settings);
private:
    struct Parameters {
        float time, patternScale, temperature, intensity;
    };
    MagmaEffectSettings settings_;
    std::unique_ptr<Mesh> mesh_;
    std::unique_ptr<WorldTransform> transform_;
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer_;
    Parameters* parameters_ = nullptr;
    float elapsed_ = 0.0f;
};
}
