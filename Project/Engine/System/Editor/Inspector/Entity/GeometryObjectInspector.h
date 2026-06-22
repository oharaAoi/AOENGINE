#pragma once
#include "Engine/Module/Components/GameObject/GeometryObject.h"

namespace AOENGINE {

/// <summary>
/// GeometryObject用のInspectorを描画するクラス。
/// </summary>
class GeometryObjectInspector {
public:

	/// <summary>
	/// GeometryObjectのTransformとMaterialを編集するUIを描画する。
	/// </summary>
	/// <param name="object">編集対象のGeometryObject。</param>
	static void Draw(GeometryObject& object);
};

}
