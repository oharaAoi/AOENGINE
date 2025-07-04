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
				 const std::vector<std::unique_ptr<Material>>& materials) {

	UINT index = 0;
	for (uint32_t oi = 0; oi < meshArray_.size(); oi++) {
		commandList->IASetVertexBuffers(0, 1, &meshArray_[oi]->GetVBV());
		commandList->IASetIndexBuffer(&meshArray_[oi]->GetIBV());
		index = pipeline->GetRootSignatureIndex("gMaterial");
		commandList->SetGraphicsRootConstantBufferView(index, materials[oi]->GetBufferAdress());
		index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
		worldTransform->BindCommandList(commandList, index);
		index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
		viewProjection->BindCommandList(commandList, index);

		std::string textureName = materials[oi]->GetUseTexture();
		index = pipeline->GetRootSignatureIndex("gTexture");
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);

		commandList->DrawIndexedInstanced(meshArray_[oi]->GetIndexNum(), 1, 0, 0, 0);
	}
}

void Model::Draw(ID3D12GraphicsCommandList* commandList,
				 const Pipeline* pipeline,
				 const WorldTransform* worldTransform, const ViewProjection* viewprojection, 
				 const D3D12_VERTEX_BUFFER_VIEW& vbv, const std::vector<std::unique_ptr<Material>>& materials) {

	UINT index = 0;
	for (uint32_t oi = 0; oi < meshArray_.size(); oi++) {
		commandList->IASetVertexBuffers(0, 1, &vbv);
		commandList->IASetIndexBuffer(&meshArray_[oi]->GetIBV());
		index = pipeline->GetRootSignatureIndex("gMaterial");
		commandList->SetGraphicsRootConstantBufferView(index, materials[oi]->GetBufferAdress());
		index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
		worldTransform->BindCommandList(commandList, index);
		index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
		viewprojection->BindCommandList(commandList, index);

		std::string textureName = materials[oi]->GetUseTexture();
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


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Modelの解析
//////////////////////////////////////////////////////////////////////////////////////////////////

//void Model::LoadObj(const std::string& directoryPath, const std::string& fileName, ID3D12Device* device) {
//	Assimp::Importer importer;
//	std::string filePath = directoryPath + fileName;
//	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs |
//											 aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
//	if (!scene) {
//		std::cerr << "Error: " << importer.GetErrorString() << std::endl;
//	}
//	assert(scene->HasMeshes()); // meshがないのは対応しない
//
//	std::vector<std::vector<Mesh::VertexData>> meshVertices;
//	std::vector<std::vector<uint32_t>> meshIndices;
//	std::vector<std::string> useMaterial;
//
//	std::unordered_map<std::string, ModelMaterialData> materialData;
//	std::unordered_map<std::string, Matrix4x4> meshUvData;
//	std::vector<std::string> materials;
//
//	// mtlファイルを読み込んでおく
//	Vector3 uvScale = Vector3(1,1,1);
//	if (std::strcmp(GetFileExtension(fileName.c_str()), "obj") == 0) {
//		LoadMtl(directoryPath, RemoveExtension(fileName) + ".mtl", uvScale);
//	}
//
//	// -------------------------------------------------
//	// ↓ meshの解析
//	// -------------------------------------------------
//	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
//		std::vector<Mesh::VertexData> triangle;
//		aiMesh* mesh = scene->mMeshes[meshIndex];
//		assert(mesh->HasNormals()); // 法線がないなら非対応
//		assert(mesh->HasTextureCoords(0)); // texcoordがないmeshは非対応
//
//		meshIndices.resize(scene->mNumMeshes);
//
//		// -------------------------------------------------
//		// ↓ faceの解析をする
//		// -------------------------------------------------
//		std::vector<Mesh::VertexData> vertices;
//		vertices.resize(mesh->mNumVertices);
//		// vertexの解析を行う
//		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
//			//uint32_t vertexIndex = .mIndices[element];
//			aiVector3D& position = mesh->mVertices[vertexIndex];
//			aiVector3D& normal = mesh->mNormals[vertexIndex];
//			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
//			aiVector3D& tangent = mesh->mTangents[vertexIndex];
//
//			vertices[vertexIndex].pos = { position.x, position.y, position.z, 1.0f };
//			vertices[vertexIndex].normal = { normal.x, normal.y, normal.z };
//			vertices[vertexIndex].texcoord = { texcoord.x * uvScale.x, texcoord.y * uvScale.y };
//			vertices[vertexIndex].tangent = { tangent.x, tangent.y, tangent.z };
//
//			vertices[vertexIndex].pos.x *= -1.0f;
//			vertices[vertexIndex].normal.x *= -1.0f;
//		}
//
//		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
//			aiFace& face = mesh->mFaces[faceIndex];
//			assert(face.mNumIndices == 3);
//
//			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
//				uint32_t vertexIndex = face.mIndices[element];
//				meshIndices[meshIndex].push_back(vertexIndex);
//			}
//		}
//
//		// -------------------------------------------------
//		// ↓ メッシュのマテリアルインデックスを取得
//		// -------------------------------------------------
//		uint32_t materialIndex = mesh->mMaterialIndex;
//		if (materialIndex < scene->mNumMaterials) {
//			aiMaterial* material = scene->mMaterials[materialIndex];
//			aiString materialName;
//			if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, materialName)) {
//				std::string nameStr = materialName.C_Str();
//				// DefaultMaterialを除く処理
//				if (nameStr == "DefaultMaterial") {
//					continue;
//				}
//				useMaterial.push_back(nameStr);
//			}
//		}
//
//		// -------------------------------------------------
//		// ↓ skinningを取得する用の処理
//		// -------------------------------------------------
//		std::map<std::string, JointWeightData> newMap;
//		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
//			// jointごとの格納領域を作る
//			aiBone* bone = mesh->mBones[boneIndex];
//			std::string jointName = bone->mName.C_Str();
//			JointWeightData& jointWeightData = newMap[jointName];
//
//			// InverseBindPoseMatrixの抽出
//			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
//			aiVector3D scale, translate;
//			aiQuaternion rotate;
//			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
//			Matrix4x4 bindPoseMatrix = Matrix4x4::MakeAffine({ scale.x, scale.y, scale.z },
//														Quaternion{ rotate.x, -rotate.y, -rotate.z, rotate.w },
//														{ -translate.x, translate.y, translate.z }
//			);
//			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();
//
//			// Weight情報を取り出す
//			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
//				jointWeightData.vertexWeight.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
//			}
//		}
//
//		// nodeの解析
//		rootNode_ = ReadNode(scene->mRootNode, scene);
//		meshVertices.push_back(vertices);
//	}
//
//	// -------------------------------------------------
//	// ↓ materialの解析
//	// -------------------------------------------------
//	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
//		aiMaterial* material = scene->mMaterials[materialIndex];
//
//		aiString materialName;
//		if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, materialName)) {
//			std::string nameStr = materialName.C_Str();
//			if (nameStr == "DefaultMaterial") {
//				continue;
//			}
//			materials.push_back(materialName.C_Str());
//		}
//
//		// textureの取得
//		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
//			aiString textureFilePath;
//			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
//			materialData[materialName.C_Str()] = ModelMaterialData();
//			std::string objTexture = textureFilePath.C_Str();
//			materialData[materialName.C_Str()].textureFilePath = objTexture;
//			TextureManager::LoadTextureFile(directoryPath, textureFilePath.C_Str());
//
//			hasTexture_ = true;
//		}
//	}
//
//	std::vector<std::unique_ptr<Mesh>> result;
//	meshArray_.resize(meshVertices.size());
//	for (uint32_t oi = 0; oi < meshVertices.size(); oi++) {
//		/*mesh->SetUseMaterial(useMaterial[oi]);*/
//		MeshManager::GetInstance()->AddMesh(device, fileName, meshVertices[oi], meshIndices[oi]);
//		meshArray_ = (MeshManager::GetInstance()->GetMeshes(fileName));
//		meshArray_[oi]->SetUseMaterial(useMaterial[oi]);
//	}
//
//	std::unordered_map<std::string, std::unique_ptr<Material>> materialResult;// 結果
//	for (uint32_t oi = 0; oi < materials.size(); oi++) {
//		materialData_[materials[oi]] = materialData[materials[oi]];
//	}
//}

Mesh* Model::GetMesh(const uint32_t& index) {
	return meshArray_[index].get();
}
