#pragma once
#include <vector>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"

struct TrailPoint {
    Vector3 position;
    float lifetime;
};

struct TrailVertex {
    Vector3 position;
    Vector2 uv;
    float alpha;
};

class TrailRenderer {
public:

    void Init(ID3D12Device* _device, UINT _maxVerticesPerFrame, UINT _maxIndicesPerFrame, UINT _frameCount);
    void Update(const std::vector<TrailPoint>& _trailPoints, float _maxLifetime);
    void Draw(ID3D12GraphicsCommandList* _commandList) const;

private:

    void GenerateTrailMesh(
        const std::vector<TrailPoint>& points, float maxLifetime,
        std::vector<TrailVertex>& outVertices, std::vector<uint16_t>& outIndices);

private:

    ComPtr<ID3D12Resource> vertexBuffer_;
    ComPtr<ID3D12Resource> indexBuffer_;
    TrailVertex* mappedVB_ = nullptr;
    uint16_t* mappedIB_ = nullptr;

    UINT maxVerticesPerFrame_ = 0;
    UINT maxIndicesPerFrame_ = 0;
    UINT maxFrameCount_ = 0;
    UINT frameIndex_ = 0;

    UINT currentVertexCount_ = 0;
    UINT currentIndexCount_ = 0;


};

