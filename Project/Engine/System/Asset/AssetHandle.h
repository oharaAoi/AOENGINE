#pragma once
#include <cstdint>

namespace AOENGINE {

enum class AssetType {
    Texture,
    Model,
    Material,
    Sound,
    AI,
    Other
};

/// <summary>
/// AssetをImGui上で扱うためのハンドルクラス
/// </summary>
struct AssetHandle {
    AssetType type;
    uint32_t id;

    static uint32_t nextId;

    AssetHandle(AssetType assetType) : type(assetType), id(nextId) {
        nextId++;
    }
};

}