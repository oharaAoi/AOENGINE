#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/Module/Components/Rigging/Skinning.h"
#include "Engine/Utilities/Loader.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Utilities/Logger.h"

Model::Model() {
}

Model::~Model() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void Model::Init(ID3D12Device* device, const std::string& directorPath, const std::string& fileName) {
	Logger::Log("[Load][Model] :" + fileName);
	//LoadObj(directorPath, fileName, device);

	if (!MeshManager::GetInstance()->ExistMesh(fileName)) {
		meshArray_ = LoadMesh(directorPath, fileName, device);
	} else {
		meshArray_ = MeshManager::GetInstance()->GetMeshes(fileName);
	}

	materialData_ = LoadMaterialData(directorPath, fileName);

	skinClusterArray_ = LoadSkinCluster(directorPath, fileName);

	rootNode_ = LoadNode(directorPath, fileName);

	Logger::Log(" --- success!\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 描画関数
//////////////////////////////////////////////////////////////////////////////////////////////////
void Model::Draw(ID3D12GraphicsCommandList* commandList,
				 const Pipeline* pipeline,
				 const WorldTransform* worldTransform,
				 const ViewProjection* viewProjection,
				 const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials) {

	UINT index = 0;
	for (uint32_t oi = 0; oi < meshArray_.size(); oi++) {
		std::string useMaterial = meshArray_[oi]->GetUseMaterial();
		BaseMaterial* material = materials.at(useMaterial).get();
		commandList->IASetVertexBuffers(0, 1, &meshArray_[oi]->GetVBV());
		commandList->IASetIndexBuffer(&meshArray_[oi]->GetIBV());
		index = pipeline->GetRootSignatureIndex("gMaterial");
		commandList->SetGraphicsRootConstantBufferView(index, material->GetBufferAddress());
		index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
		worldTransform->BindCommandList(commandList, index);
		index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
		viewProjection->BindCommandList(commandList, index);
		index = pipeline->GetRootSignatureIndex("gViewProjectionMatrixPrev");
		viewProjection->BindCommandListPrev(commandList, index);

		std::string textureName = material->GetAlbedoTexture();
		index = pipeline->GetRootSignatureIndex("gTexture");
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);

		commandList->DrawIndexedInstanced(meshArray_[oi]->GetIndexNum(), 1, 0, 0, 0);
	}
}

void Model::Draw(ID3D12GraphicsCommandList* commandList,
				 const Pipeline* pipeline,
				 const WorldTransform* worldTransform, const ViewProjection* viewprojection, 
				 const D3D12_VERTEX_BUFFER_VIEW& vbv, const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials) {

	UINT index = 0;
	for (uint32_t oi = 0; oi < meshArray_.size(); oi++) {
		std::string useMaterial = meshArray_[oi]->GetUseMaterial();
		BaseMaterial* material = materials.at(useMaterial).get();
		commandList->IASetVertexBuffers(0, 1, &vbv);
		commandList->IASetIndexBuffer(&meshArray_[oi]->GetIBV());
		index = pipeline->GetRootSignatureIndex("gMaterial");
		commandList->SetGraphicsRootConstantBufferView(index, material->GetBufferAddress());
		index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
		worldTransform->BindCommandList(commandList, index);
		index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
		viewprojection->BindCommandList(commandList, index);

		std::string textureName = material->GetAlbedoTexture();
		index = pipeline->GetRootSignatureIndex("gTexture");
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);

		commandList->DrawIndexedInstanced(meshArray_[oi]->GetIndexNum(), 1, 0, 0, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug
//////////////////////////////////////////////////////////////////////////////////////////////////
void Model::Debug_Gui(const std::string& name) {
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

Model::Node Model::ReadNode(aiNode* node, const aiScene* scene) {
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
	result.localMatrix = Matrix4x4::MakeAffine(result.transform.scale, result.transform.rotate.Normalize(), result.transform.translate);
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

Mesh* Model::GetMesh(const uint32_t& index) {
	return meshArray_[index].get();
}
