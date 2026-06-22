#pragma once
#include "Engine/Module/Components/GameObject/BaseGameObject.h"

namespace AOENGINE {

/// <summary>
/// BaseGameObject全体のInspector描画をまとめるクラス。
/// Model、Material、Colliderなどの個別Inspectorを呼び出す。
/// </summary>
class BaseGameObjectInspector {
public:
    /// <summary>
    /// BaseGameObjectのInspector UIを描画する。
    /// </summary>
    /// <param name="object">編集対象のBaseGameObject。</param>
    static void Draw(BaseGameObject& object);

    /// <summary>
    /// Component追加用のポップアップUIを描画する。
    /// </summary>
    /// <param name="object">Componentを追加する対象のBaseGameObject。</param>
    static void DrawAddComponent(BaseGameObject& object);
};

}
