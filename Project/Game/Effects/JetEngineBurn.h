#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Geometry/Polygon/CylinderGeometry.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/GameObject/GeometryObject.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

/// <summary>
/// ジェットエンジンのエフェクト
/// </summary>
class JetEngineBurn :
	public AOENGINE::ISceneObject {
public:

	struct NoiseUV {
		Math::Matrix4x4 uv;
	};

public:

	struct Parameter : public AOENGINE::IJsonConverter {
		AOENGINE::Color color;
		Math::Vector3 scale;
		Math::Quaternion rotate;
		Math::Vector3 translate;
		Math::Vector3 noiseScale;

		std::string  materialTexture = "white.png";
		std::string  blendTexture = "white.png";

		Parameter() {
			SetGroupName("Effect");
			SetName("jetBurnParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("color", color)
				.Add("scale", scale)
				.Add("rotate", rotate)
				.Add("translate", translate)
				.Add("noiseScale", noiseScale)
				.Add("materialTexture", materialTexture)
				.Add("blendTexture", blendTexture)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "color", color);
			Convert::fromJson(jsonData, "scale", scale);
			Convert::fromJson(jsonData, "rotate", rotate);
			Convert::fromJson(jsonData, "translate", translate);
			Convert::fromJson(jsonData, "noiseScale", noiseScale);
			Convert::fromJson(jsonData, "materialTexture", materialTexture);
			Convert::fromJson(jsonData, "blendTexture", blendTexture);
		}

		void Debug_Gui() override;
	};

public:

	JetEngineBurn() = default;
	~JetEngineBurn() override = default;

public:

	// 終了処理
	void Finalize() ;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// あとから更新
	void PostUpdate() override;
	// 前景描画
	void PreDraw() const override {};
	// 描画
	void Draw() const override;
	// 編集
	void Debug_Gui() override;
	// gizumo表示
	void Manipulate([[maybe_unused]] const ImVec2& windowSize, [[maybe_unused]] const ImVec2& imagePos) override {};

public:

	void AddMeshManager(std::shared_ptr<AOENGINE::Mesh>& _pMesh, const std::string& name);

	bool ExistMesh(const std::string& name);

	AOENGINE::WorldTransform* GetWorldTransform() { return worldTransform_.get(); }

private:

	AOENGINE::SphereGeometry geometry_;

	std::shared_ptr<AOENGINE::Mesh> mesh_;
	std::unique_ptr<AOENGINE::Material> material_ = nullptr;
	std::unique_ptr<AOENGINE::WorldTransform> worldTransform_ = nullptr;

	ComPtr<ID3D12Resource> noiseBuffer_;
	NoiseUV* noiseUV_;
	Math::SRT noiseSRT_;

	Parameter param_;

	AOENGINE::VectorTween<float> noiseAnimation_;

	Math::Vector3 initScale_;
};