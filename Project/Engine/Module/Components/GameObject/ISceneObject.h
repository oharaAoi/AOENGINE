#pragma once
#include "Engine/Module/Components/GameObject/SceneObject.h"
#include "Engine/Lib/Math/MathStructures.h"
#include <imgui.h>

namespace AOENGINE {

/// <summary>
/// sceneに配置するobjctが継承するクラス
/// </summary>
class ISceneObject :
	public AOENGINE::SceneObject {
public: // constructor

	ISceneObject() = default;
	virtual ~ISceneObject() = default;

public: // public method

	// 初期化処理
	virtual void Init() = 0;
	// 更新処理
	virtual void Update() = 0;
	// 後から更新
	virtual void PostUpdate() = 0;
	// 描画前に行う描画
	virtual void PreDraw() const = 0;
	// 描画処理
	virtual void Draw() const = 0;
	// gizumo表示
	virtual void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) = 0;
	// Frustum Cullingを有効にするか
	virtual bool IsFrustumCullingEnabled() const { return false; }
	// Frustum Cullingに使用するワールド空間の境界球
	virtual Math::Sphere GetWorldBoundingSphere() const { return Math::Sphere{ .center = CVector3::ZERO, .radius = 0.0f }; }

public: // accessor

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; }
	bool GetIsDestroy() const { return isDestroy_; }

protected: // protected variable

	bool isDestroy_;

};

}
