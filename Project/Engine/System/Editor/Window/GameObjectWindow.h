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
public:

	GameObjectWindow();
	~GameObjectWindow() override;

	void Init();

	void AddAttributeGui(AOENGINE::AttributeGui* attribute, const std::string& label);

	void InspectorWindow() override;

	void HierarchyWindow() override;

	void ExecutionWindow() override;

public:

	AOENGINE::AttributeGui* GetSelectObject() const { return selectAttribute_; }

	void SetProcessedSceneFrame(AOENGINE::ProcessedSceneFrame* sceneFrame) { processedSceneFrame_ = sceneFrame; }
	void SetSceneRenderer(AOENGINE::SceneRenderer* _renderer) { sceneRenderer_ = _renderer; }
	void SetCanvas2d(AOENGINE::Canvas2d* _canvas) { canvas2d_ = _canvas; }

private:

	std::string MakeUniqueName(const std::string& baseName);

private:

	std::vector<AOENGINE::AttributeGui*> attributeArray_;
	AOENGINE::AttributeGui* selectAttribute_ = nullptr;

	AOENGINE::ProcessedSceneFrame* processedSceneFrame_ = nullptr;
	AOENGINE::SceneRenderer* sceneRenderer_;
	AOENGINE::Canvas2d* canvas2d_;
};

}