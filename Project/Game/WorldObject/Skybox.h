#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

/// <summary>
/// skybox
/// </summary>
class Skybox :
	public AOENGINE::ISceneObject {
public:

	Skybox() = default;
	~Skybox();

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 後から更新
	void PostUpdate() override {};
	// 前景描画
	void PreDraw() const override {};
	// 描画
	void Draw() const override;
	// 編集処理
	void Debug_Gui() override {};
	// gizumo表示
	void Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) override {};

public:

	void AddMeshManager(std::shared_ptr<AOENGINE::Mesh>& _pMesh, const std::string& name);

	bool ExistMesh(const std::string& name);

	const std::string& GetTexture() const { return useTexture_; }

private:

	AOENGINE::CubeGeometry cube_;

	std::shared_ptr<AOENGINE::Mesh> mesh_;
	std::unique_ptr<AOENGINE::Material> material_ = nullptr;
	std::unique_ptr<AOENGINE::WorldTransform> transform_ = nullptr;

	std::string useTexture_ = "output_skybox.dds";
};

