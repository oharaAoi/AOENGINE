#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <limits>
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/Module/Components/Rigging/Skinning.h"
#include "Engine/Utilities/Loader.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

AOENGINE::Model::Model() {
}

AOENGINE::Model::~Model() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Model::Init(ID3D12Device* device, const std::string& directorPath, const std::string& fileName) {
	name_ = fileName;
	AOENGINE::Logger::Log("[Load][Model] :" + fileName);

	if (!AOENGINE::MeshManager::GetInstance()->ExistMesh(fileName)) {
		meshArray_ = LoadMesh(directorPath, fileName, device);
	} else {
		meshArray_ = AOENGINE::MeshManager::GetInstance()->GetMeshes(fileName);
	}

	materialData_ = LoadMaterialData(directorPath, fileName);

	skinClusterArray_ = LoadSkinCluster(directorPath, fileName);

	rootNode_ = LoadNode(directorPath, fileName);
	CalculateLocalBoundingSphere();

	AOENGINE::Logger::Log(" --- success!\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug
//////////////////////////////////////////////////////////////////////////////////////////////////
void AOENGINE::Model::Debug_Gui(const std::string& name) {
	if (ImGui::TreeNode(name.c_str())) {
		for (uint32_t oi = 0; oi < meshArray_.size(); oi++) {
			std::string materialNum = std::to_string(oi);
			std::string materialName = "material" + materialNum;
			if (ImGui::TreeNode(materialName.c_str())) {
				//materialArray_[meshArray_[oi]->GetUseMaterial()]->ImGuiDraw();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}


/// //////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　 assimpでのNode解析
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::Model::Node AOENGINE::Model::ReadNode(aiNode* node, const aiScene* scene) {
	Node result;
	// ----------------------------------
	// LocalMatrixを取得する
	// ----------------------------------
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);

	//result.transform.scale = { 1, 1, 1 };
	result.transform.scale = { scale.x, scale.y, scale.z };
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w };
	result.transform.translate = { -translate.x, translate.y, translate.z };
	result.localMatrix = Math::Matrix4x4::MakeAffine(result.transform.scale, result.transform.rotate.Normalize(), result.transform.translate);
	result.name = node->mName.C_Str(); // Nodeの名前を格納

	// ----------------------------------
	// Nodeを格納する
	// ----------------------------------
	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex], scene);
	}

	return result;
}

Mesh* AOENGINE::Model::GetMesh(const uint32_t& index) {
	return meshArray_[index].get();
}

void AOENGINE::Model::CalculateLocalBoundingSphere() {
	if (meshArray_.empty()) {
		localBoundingSphere_ = Math::Sphere{ .center = CVector3::ZERO, .radius = 0.0f };
		return;
	}

	Math::Vector3 min{
		(std::numeric_limits<float>::max)(),
		(std::numeric_limits<float>::max)(),
		(std::numeric_limits<float>::max)()
	};
	Math::Vector3 max{
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest(),
		std::numeric_limits<float>::lowest()
	};
	bool hasVertex = false;

	for (const std::shared_ptr<AOENGINE::Mesh>& mesh : meshArray_) {
		if (!mesh) {
			continue;
		}

		for (const VertexData& vertex : mesh->GetVerticesData()) {
			const Math::Vector3 position{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
			min = Math::Vector3::Min(min, position);
			max = Math::Vector3::Max(max, position);
			hasVertex = true;
		}
	}

	if (!hasVertex) {
		localBoundingSphere_ = Math::Sphere{ .center = CVector3::ZERO, .radius = 0.0f };
		return;
	}

	const Math::Vector3 center = (min + max) * 0.5f;
	float radius = 0.0f;

	for (const std::shared_ptr<AOENGINE::Mesh>& mesh : meshArray_) {
		if (!mesh) {
			continue;
		}

		for (const VertexData& vertex : mesh->GetVerticesData()) {
			const Math::Vector3 position{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
			const float distance = (position - center).Length();
			if (distance > radius) {
				radius = distance;
			}
		}
	}

	localBoundingSphere_ = Math::Sphere{ .center = center, .radius = radius };
}
