#pragma once
// c++
#include <cstdint>
#include <string>
// directX
#include <d3d12.h>
// engine
#include "Engine/Lib/Math/MathStructures.h"

namespace AOENGINE {

/// <summary>
/// Meshの描画範囲をまとめた構造体
/// </summary>
struct SubMesh {
    // Mesh共通IndexBuffer内での開始位置
    uint32_t firstIndex = 0;

    // 描画するIndex数
    uint32_t indexCount = 0;

    // DrawIndexedInstancedのBaseVertexLocation
    int32_t baseVertex = 0;

    // RendererのMaterial配列に対するスロット番号
    uint32_t materialSlot = 0;

    D3D12_PRIMITIVE_TOPOLOGY topology =
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    std::string name;
};

}
