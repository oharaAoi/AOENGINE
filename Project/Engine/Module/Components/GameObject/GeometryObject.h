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

	void Init() override;

	void Update() override;
	void PostUpdate() override {};

	void PreDraw() const override {};
	void Draw() const override;

	void Debug_Gui() override;
	void Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) override {};

	void SetEditorWindow();

public:

	Mesh* GetMesh() { return mesh_.get(); }
	Material* GetMaterial() { return material_.get(); }

	void SetUseTexture(const std::string& name) { material_->SetAlbedoTexture(name); }
	const std::string& GetUseTexture() const { return material_->GetAlbedoTexture(); }

	WorldTransform* GetTransform() { return transform_.get(); }

private:

	std::string id_ = "new GeometryObject";

	std::shared_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> transform_ = nullptr;

	std::string useTexture_ = "circle.png";

};

