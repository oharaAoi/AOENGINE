#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Geometry/Polygon/CylinderGeometry.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"

/// <summary>
/// ヒット時の爆発クラス
/// </summary>
class HitExplode :
	public ISceneObject {
public:

	HitExplode() = default;
	~HitExplode() ;

public:

	// 終了処理
	void Finalize();
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 後から更新
	void PostUpdate() override {};
	// 前景描画
	void PreDraw() const override;
	// 描画
	void Draw() const override;
	// 爆破をセットする
	void Set(const Vector3& pos, const Color& color, const std::string& useTexture);
	// 編集処理
	void Debug_Gui() override;
	// gizumo表示
	void Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) override {};

public:

	void AddMeshManager(std::shared_ptr<Mesh>& _pMesh, const std::string& name);

	bool ExistMesh(const std::string& name);

	WorldTransform* GetWorldTransform() { return worldTransform_.get(); }

	bool GetIsAlive() const { return isAlive_; }

	void SetFrameRate(float rate) { frameRate_ = rate; }

	void SetBlendMode(int blendMode) { blendMode_ = blendMode; }

private:

	PlaneGeometry geometry_;

	std::shared_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material_ = nullptr;
	std::unique_ptr<WorldTransform> worldTransform_ = nullptr;

	float currentTimer_;
	float columns = 6.0f;
	float rows = 6.0f;

	int frameIndex = 10;
	float frameRate_;

	int blendMode_;

	bool isAlive_;
};

