#pragma once
#include <vector>
#include <functional>
#include <string>
#include <unordered_map>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/Attribute/IEditorWindow.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"

#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Module/Components/2d/Canvas2d.h"

namespace AOENGINE {

/// <summary>
/// AttributeGuiをObjectHandleベースのHierarchy/Inspectorに乗せるためのEditor用SceneObject。
/// </summary>
class AttributeGuiSceneObject :
	public ISceneObject {
public:
	explicit AttributeGuiSceneObject(AOENGINE::AttributeGui* attribute = nullptr);
	~AttributeGuiSceneObject() override = default;

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void PreDraw() const override;
	void Draw() const override;
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) override;

	void DrawInspector();
	void SyncFromAttribute();
	void SetAttributeName(const std::string& name);
	void SetAttributeActive(bool isActive);

	AOENGINE::AttributeGui* GetAttribute() const { return attribute_; }

private:
	AOENGINE::AttributeGui* attribute_ = nullptr;
};

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

	AOENGINE::SceneObject* GetSelectObject() const;

	void SetProcessedSceneFrame(AOENGINE::ProcessedSceneFrame* sceneFrame) { processedSceneFrame_ = sceneFrame; }
	void SetSceneRenderer(AOENGINE::SceneRenderer* _renderer);
	void SetCanvas2d(AOENGINE::Canvas2d* _canvas) { canvas2d_ = _canvas; }

private: // private method

	std::string MakeUniqueName(const std::string& baseName, const AOENGINE::SceneObject* ignoreObject = nullptr) const;

	void EnsureAttributeGuiObjects();
	AOENGINE::ObjectHandle EnsureAttributeGuiObjectRecursive(AOENGINE::AttributeGui* attribute, const AOENGINE::ObjectHandle& parentHandle);
	bool HasRegisteredAttribute(AOENGINE::AttributeGui* attribute) const;

	void CreateNewObjectWindow();
	void DrawHierarchyObject(AOENGINE::SceneObject& object);
	bool IsSelected(const AOENGINE::ObjectHandle& handle) const;

private:

	AOENGINE::ObjectHandle selectedObjectHandle_;

	AOENGINE::ProcessedSceneFrame* processedSceneFrame_ = nullptr;
	AOENGINE::SceneRenderer* sceneRenderer_ = nullptr;
	AOENGINE::Canvas2d* canvas2d_ = nullptr;

	std::vector<AOENGINE::AttributeGui*> registeredAttributes_;
	std::unordered_map<AOENGINE::AttributeGui*, AOENGINE::ObjectHandle> attributeObjectHandles_;
};

}
