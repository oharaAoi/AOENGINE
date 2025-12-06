#pragma once
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// sceneに配置するobjctが継承するクラス
/// </summary>
class ISceneObject : 
	public AOENGINE::AttributeGui {
public: // コンストラクタ

	ISceneObject() = default;
	virtual ~ISceneObject() = default;

public:

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
	// 編集処理
	virtual void Debug_Gui() = 0;
	// gizumo表示
	virtual void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) = 0;

public:

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; }
	bool GetIsDestroy() const { return isDestroy_; }

protected:

	bool isDestroy_;

};