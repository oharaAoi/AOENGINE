#pragma once
#include <vector>
#include <functional>
#include <string>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Attribute/IEditorWindow.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"

#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Module/Components/2d/Canvas2d.h"

namespace AOENGINE {

/// <summary>
/// GameObjectをまとめたwindow
/// </summary>
class GameObjectWindow :
	public IEditorWindow {
public: // constructor

	GameObjectWindow();
	~GameObjectWindow() override;
	
public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// Itemの追加
	/// </summary>
	/// <param name="attribute"></param>
	/// <param name="label"></param>
	void AddAttributeGui(AOENGINE::AttributeGui* attribute, const std::string& label);

	/// <summary>
	/// InspectorWindow
	/// </summary>
	void InspectorWindow() override;

	/// <summary>
	/// Hierarchyの表示
	/// </summary>
	void HierarchyWindow() override;

	/// <summary>
	/// Executeの表示
	/// </summary>
	void ExecutionWindow() override;

	/// <summary>
	/// Objectの削除
	/// </summary>
	/// <param name="attribute"></param>
	void DeleteObject(AOENGINE::AttributeGui* attribute);

public: // accessor

	AOENGINE::AttributeGui* GetSelectObject() const { return selectAttribute_; }

	void SetProcessedSceneFrame(AOENGINE::ProcessedSceneFrame* sceneFrame) { processedSceneFrame_ = sceneFrame; }
	void SetSceneRenderer(AOENGINE::SceneRenderer* _renderer) { sceneRenderer_ = _renderer; }
	void SetCanvas2d(AOENGINE::Canvas2d* _canvas) { canvas2d_ = _canvas; }

private: // private method

	std::string MakeUniqueName(const std::string& baseName);

	void CreateNewObjectWindow();

private:

	std::vector<AOENGINE::AttributeGui*> attributeArray_;
	AOENGINE::AttributeGui* selectAttribute_ = nullptr;

	AOENGINE::ProcessedSceneFrame* processedSceneFrame_ = nullptr;
	AOENGINE::SceneRenderer* sceneRenderer_;
	AOENGINE::Canvas2d* canvas2d_;
};

}