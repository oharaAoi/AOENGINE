#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

/// <summary>
/// BaseGameObjectが参照するModelを表示・差し替えするInspector。
/// </summary>
class ModelInspector {
public:
    /// <summary>
    /// 現在のModel表示、Assetドロップ、読み込み済みModel選択UIを描画する。
    /// </summary>
    /// <param name="object">Modelを差し替える対象のBaseGameObject。</param>
    static void Draw(BaseGameObject& object);
};

}
