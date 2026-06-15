#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Geometry/GeometryFactory.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Module/Geometry/Polygon/RingGeometry.h"
#include "Engine/Module/Geometry/Polygon/CylinderGeometry.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"

namespace AOENGINE {

/// <summary>
/// Geometryの各形状をScene上に描画する際に使用するクラス
/// </summary>
class GeometryObject final
	: public ISceneObject {
public:

	GeometryObject() = default;
	~GeometryObject();

	/// <summary>
	/// Geometryを設定する
	/// </summary>
	/// <typeparam name="ShapePolicy">: 型</typeparam>
	/// <typeparam name="...Args">: 可変長引数</typeparam>
	/// <param name="...args">: 型に応じた引数</param>
	template <typename ShapePolicy, typename... Args>
	void Set(Args&&... args) {
		Init();
		GeometryFactory::GetInstance().Create<ShapePolicy>(mesh_, std::forward<Args>(args)...);
	}

	// 初期化処理
	void Init() override;
	// 更新処理
	void Update() override;
	// 後から更新する処理
	void PostUpdate() override {};
	// モデル描画前に行う処理
	void PreDraw() const override {};
	// 描画処理
	void Draw() const override;
	// 編集処理
	void Debug_Gui() override;
	// gizumo表示
	void Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) override {};
	// editorに登録
	void SetEditorWindow();

public:

	AOENGINE::Mesh* GetMesh() { return mesh_.get(); }
	AOENGINE::Material* GetMaterial() { return material_.get(); }

	void SetUseTexture(const std::string& name) { material_->SetAlbedoTexture(name); }
	const std::string& GetUseTexture() const { return material_->GetAlbedoTexture(); }

	AOENGINE::WorldTransform* GetTransform() { return transform_.get(); }

private:

	std::string id_ = "new GeometryObject";

	std::shared_ptr<AOENGINE::Mesh> mesh_;
	std::unique_ptr<AOENGINE::Material> material_ = nullptr;
	std::unique_ptr<AOENGINE::WorldTransform> transform_ = nullptr;

	std::string useTexture_ = "circle.png";

};

}