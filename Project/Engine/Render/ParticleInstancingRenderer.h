#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/DirectX/Resource/ShaderResource.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"

/// <summary>
/// Particleをインスタンシング描画するためのクラス
/// </summary>
class ParticleInstancingRenderer {
public:		// 構造体

	struct ParticleData {
		Matrix4x4 worldMat;		// 座標
		Vector4 color;			// 色
		uint32_t draw2d;		// 2d描画かどうか
		float discardValue;	// discardの値
	};

	struct Information {
		std::shared_ptr<Mesh> pMesh;
		std::shared_ptr<Material> materials;
		std::string textureName;
		ComPtr<ID3D12Resource> particleResource_;
		DescriptorHandles srvHandle_;
		ParticleData* particleData;
		uint32_t useIndex = 0;

		bool isAddBlend;
	};

	struct PerView {
		Matrix4x4 viewProjection;
		Matrix4x4 viewProjection2d;
		Matrix4x4 billboardMat;
	};

public:

	ParticleInstancingRenderer() = default;
	~ParticleInstancingRenderer();

	void Init(uint32_t instanceNum);

	void Update(const std::string& id, const std::vector<ParticleData>& particleData, bool addBlend);

	void PostUpdate();

	void Draw(ID3D12GraphicsCommandList* commandList) const;

public:

	std::shared_ptr<Material> AddParticle(const std::string& id, const std::string& textureName, std::shared_ptr<Mesh> _pMesh, bool isAddBlend = true);

	void ChangeMesh(const std::string& id, std::shared_ptr<Mesh> _mesh);

	void SetView(const Matrix4x4& view, const Matrix4x4& view2d, const Matrix4x4& bill) {
		perView_->viewProjection = view;
		perView_->viewProjection2d = view2d;
		perView_->billboardMat = bill;
	}

private:

	uint32_t maxInstanceNum_;

	std::unordered_map<std::string, Information> particleMap_;

	ComPtr<ID3D12Resource> perViewBuffer_;
	PerView* perView_;

};