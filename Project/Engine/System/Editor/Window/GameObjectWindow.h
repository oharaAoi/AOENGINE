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
#include "Engine/Module/PostEffect/PostProcess.h"

namespace AOENGINE {

/// <summary>
/// PostProcessをHierarchyで選択するためのEditor専用SceneObject。
/// PostProcess本体の所有権は持たない。
/// </summary>
class PostProcessSceneObject :
	public ISceneObject {
public:
	explicit PostProcessSceneObject(AOENGINE::PostProcess* postProcess = nullptr);
	~PostProcessSceneObject() override = default;

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void PreDraw() const override;
	void Draw() const override;
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) override;

	AOENGINE::PostProcess* GetPostProcess() const { return postProcess_; }

private:
	AOENGINE::PostProcess* postProcess_ = nullptr;
};

/// <summary>
/// 個別PostEffectをHierarchyで選択するためのEditor専用SceneObject。
/// Resize時のEffect再生成に追従できるよう、Effectポインタではなくownerとtypeを保持する。
/// </summary>
class PostEffectSceneObject :
	public ISceneObject {
public:
	PostEffectSceneObject(AOENGINE::PostProcess* owner = nullptr, PostEffectType type = PostEffectType::Grayscale);
	~PostEffectSceneObject() override = default;

	void Init() override;
	void Update() override;
	void PostUpdate() override;
	void PreDraw() const override;
	void Draw() const override;
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) override;

	std::shared_ptr<PostEffect::IPostEffect> ResolveEffect() const;
	AOENGINE::PostProcess* GetOwner() const { return owner_; }
	PostEffectType GetEffectType() const { return type_; }

private:
	AOENGINE::PostProcess* owner_ = nullptr;
	PostEffectType type_ = PostEffectType::Grayscale;
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

	/// <summary>選択やドラッグ中の状態だけをリセットする。</summary>
	void ResetInteractionState();

	/// <summary>
	/// Itemの追加
	/// </summary>
	/// <param name="attribute"></param>
	/// <param name="label"></param>
	void AddPostProcess(AOENGINE::PostProcess* postProcess, const std::string& label);

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
public: // accessor

	AOENGINE::SceneObject* GetSelectObject() const;

	void SetProcessedSceneFrames(AOENGINE::ProcessedSceneFrame* gameFrame, AOENGINE::ProcessedSceneFrame* editorFrame) {
		processedSceneFrame_ = gameFrame;
		editorSceneFrame_ = editorFrame;
	}
	void SetSceneRenderer(AOENGINE::SceneRenderer* _renderer);
	void SetCanvas2d(AOENGINE::Canvas2d* canvas);

private: // private method

	std::string MakeUniqueName(const std::string& baseName, const AOENGINE::SceneObject* ignoreObject = nullptr) const;

	void EnsurePostProcessObjects();
	void EnsureCanvasObject();
	void EnsureParticleObjects();
	void InstantiatePrefabAtScenePosition(
		const std::string& prefabName, const ImVec2& pixelPos, const ImVec2& imageSize);

	void CreateNewObjectWindow();
	void DrawHierarchyObject(AOENGINE::SceneObject& object);
	void DrawHierarchyDragDrop(AOENGINE::SceneObject& object);
	void DrawHierarchyContextMenu(AOENGINE::SceneObject& object);
	void ApplyPendingHierarchyAction();
	void ApplyPendingParentChange();
	void SyncTransformParent(const AOENGINE::ObjectHandle& child, const AOENGINE::ObjectHandle& parent);
	bool IsSelected(const AOENGINE::ObjectHandle& handle) const;

private:

	AOENGINE::ObjectHandle selectedObjectHandle_;
	AOENGINE::ObjectHandle pendingDeleteHandle_;
	AOENGINE::ObjectHandle pendingDuplicateHandle_;
	AOENGINE::ObjectHandle pendingParentChildHandle_;
	AOENGINE::ObjectHandle pendingParentHandle_;
	bool pendingMoveToRoot_ = false;

	AOENGINE::ProcessedSceneFrame* processedSceneFrame_ = nullptr;
	AOENGINE::ProcessedSceneFrame* editorSceneFrame_ = nullptr;
	AOENGINE::SceneRenderer* sceneRenderer_ = nullptr;
	AOENGINE::Canvas2d* canvas2d_ = nullptr;

	AOENGINE::PostProcess* registeredPostProcess_ = nullptr;
	std::string postProcessLabel_ = "PostProcess";
	AOENGINE::ObjectHandle postProcessObjectHandle_;
	std::unordered_map<PostEffectType, AOENGINE::ObjectHandle> postEffectObjectHandles_;
};

}
