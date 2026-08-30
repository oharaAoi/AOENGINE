#include "Loader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Engine/Module/Geometry/Structs/Vertices.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/Utilities/Loader.h"

using namespace AOENGINE;

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　MeshのLoad
//////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<Mesh>> LoadMesh(const std::string& directoryPath, const std::string& fileName, ID3D12Device* device) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	assert(scene->HasMeshes()); // meshがないのは対応しない

	Mesh::CreateInfo createInfo;
	createInfo.name = fileName;

	// mtlファイルを読み込んでおく
	Math::Vector3 uvScale = Math::Vector3(1, 1, 1);
	if (std::strcmp(GetFileExtension(fileName.c_str()), "obj") == 0) {
		LoadMtl(directoryPath, RemoveExtension(fileName) + ".mtl", uvScale);
	}

	// -------------------------------------------------
	// ↓ meshの解析
	// -------------------------------------------------
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		const int32_t baseVertex = static_cast<int32_t>(createInfo.vertices.size());
		const uint32_t firstIndex = static_cast<uint32_t>(createInfo.indices.size());

		// -------------------------------------------------
		// ↓ faceの解析をする
		// -------------------------------------------------
		std::vector<VertexData> vertices(mesh->mNumVertices);
		// vertexの解析を行う
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			vertices[vertexIndex].pos = { position.x, position.y, position.z, 1.0f };
			
			// normal
			if (mesh->mNormals) {
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				vertices[vertexIndex].normal = { normal.x, normal.y, normal.z };
			} else {
				vertices[vertexIndex].normal = { 0.0f, 0.0f, 1.0f };
			}

			// texcoord
			if (mesh->mTextureCoords[0]) {
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				vertices[vertexIndex].texcoord = { texcoord.x * uvScale.x, texcoord.y * uvScale.y };
			} else {
				vertices[vertexIndex].texcoord = { 0.0f, 0.0f };
			}

			// tangent
			if (mesh->HasTangentsAndBitangents()) {
				aiVector3D N = mesh->mNormals[vertexIndex];
				aiVector3D T = mesh->mTangents[vertexIndex];
				aiVector3D B = mesh->mBitangents[vertexIndex];

				Math::Vector3 vN = { N.x, N.y, N.z };
				Math::Vector3 vT = { T.x, T.y, T.z };
				Math::Vector3 vB = { B.x, B.y, B.z };

				// glTF tangent.w を復元
				float tangentW =
					(Dot(Cross(vN, vT), vB) < 0.0f) ? -1.0f : 1.0f;
				vertices[vertexIndex].tangent = { vT.x, vT.y, vT.z, tangentW };
			} else {
				vertices[vertexIndex].tangent = { 0.0f, 0.0f, 0.0f, 0.0f };
			}

			// 読み込み後の処理
			vertices[vertexIndex].pos.x *= -1.0f;
			vertices[vertexIndex].normal.x *= -1.0f;
			vertices[vertexIndex].materialSlot = mesh->mMaterialIndex;
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				createInfo.indices.push_back(static_cast<uint32_t>(baseVertex) + vertexIndex);
			}
		}

		createInfo.vertices.insert(createInfo.vertices.end(), vertices.begin(), vertices.end());
		createInfo.subMeshes.push_back(SubMesh{
			.firstIndex = firstIndex,
			.indexCount = static_cast<uint32_t>(createInfo.indices.size()) - firstIndex,
			.baseVertex = 0,
			.materialSlot = mesh->mMaterialIndex,
			.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			.name = mesh->mName.C_Str()
		});
	}

	AOENGINE::MeshManager::GetInstance()->AddMesh(device, fileName, createInfo);
	return AOENGINE::MeshManager::GetInstance()->GetMeshes(fileName);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　MaterialのLoad
//////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, std::unique_ptr<Material>> LoadMaterial(const std::string& directoryPath, const std::string& fileName) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	assert(scene->HasMeshes()); // meshがないのは対応しない

	std::unordered_map<std::string, ModelMaterialData> materialData;
	std::vector<std::string> materials;
	// -------------------------------------------------
	// ↓ materialの解析
	// -------------------------------------------------
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		aiString materialName;
		if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, materialName)) {
			std::string nameStr = materialName.C_Str();
			if (nameStr == "DefaultMaterial") {
				continue;
			}
		}

		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			materials.push_back(materialName.C_Str());
			materialData[materialName.C_Str()] = ModelMaterialData();
			std::string objTexture = textureFilePath.C_Str();
			materialData[materialName.C_Str()].textureFilePath = objTexture;
			AOENGINE::TextureManager::GetInstance()->StackTexture(directoryPath, textureFilePath.C_Str());
		}
	}

	std::unordered_map<std::string, std::unique_ptr<Material>> materialResult;// 結果
	for (uint32_t oi = 0; oi < materials.size(); oi++) {
		materialResult[materials[oi]] = std::make_unique<Material>();
		materialResult[materials[oi]]->Init();
		materialResult[materials[oi]]->SetMaterialData(materialData[materials[oi]]);
	}

	return materialResult;
}

std::unordered_map<std::string, ModelMaterialData> LoadMaterialData(const std::string& directoryPath, const std::string& fileName) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	assert(scene->HasMeshes()); // meshがないのは対応しない

	std::unordered_map<std::string, ModelMaterialData> materialData;

	// -------------------------------------------------
	// ↓ materialの解析
	// -------------------------------------------------
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		aiString materialName;
		if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, materialName)) {
		} else {
			materialName = "not set MaterialName" + std::to_string(materialIndex);
		}

		materialData[materialName.C_Str()] = ModelMaterialData();
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);			
			std::string objTexture = textureFilePath.C_Str();
			materialData[materialName.C_Str()].textureFilePath = objTexture;
			AOENGINE::TextureManager::GetInstance()->StackTexture(directoryPath, objTexture);
		}

		aiColor3D color;
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
			materialData[materialName.C_Str()].color = { color.r, color.g, color.b, 1.0f };
		}
	}

	return materialData;
}

std::vector<std::string> LoadMaterialSlotNames(const std::string& directoryPath, const std::string& fileName) {
	Assimp::Importer importer;
	const std::string filePath = directoryPath + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_Triangulate);
	assert(scene != nullptr && scene->HasMaterials());

	std::vector<std::string> result(scene->mNumMaterials);
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiString materialName;
		if (AI_SUCCESS == scene->mMaterials[materialIndex]->Get(AI_MATKEY_NAME, materialName)) {
			result[materialIndex] = materialName.C_Str();
		} else {
			result[materialIndex] = "not set MaterialName" + std::to_string(materialIndex);
		}
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　MaterialのLoad
//////////////////////////////////////////////////////////////////////////////////////////////////

void LoadMtl(const std::string& directoryPath, const std::string& fileName, Math::Vector3& scale) {
	std::unordered_map<std::string, ModelMaterialData> materialDatas;// 後で一気に結果の変数に代入するための物

	std::string line;// ファイルから読み込んだ1行を格納する物
	std::string materialName; // newmtlの名前

	// マテリアルの名前を格納しておく
	std::vector<std::string> materials;

	// mtlファイルを開く
	std::ifstream file2(directoryPath + "/" + fileName);
	assert(file2.is_open());

	// ファイルを読む
	while (std::getline(file2, line)) {
		std::string materialIdentifier;
		std::istringstream s(line);
		s >> materialIdentifier; // 先頭の識別子を読む

		if (materialIdentifier == "newmtl") {
			s >> materialName;
			materials.push_back(materialName);

		} else if (materialIdentifier == "map_Kd") {
			
			// テクスチャのスケーリングオプションを処理
			std::string scalingOption;
			if (s >> scalingOption && scalingOption == "-s") {
				// スケーリング値を読み取る
				float scaleX, scaleY, scaleZ;
				s >> scaleX >> scaleY >> scaleZ;
				// スケーリング情報を保存
				scale = Math::Vector3(scaleX, scaleY, scaleZ);
			}

		} else if (materialIdentifier == "Ka") {
			// アルベド色を読み取る(環境反射率)
			Math::Vector4 color;
			s >> color.x >> color.y >> color.z;
			
		} else if (materialIdentifier == "Kd") {
			// ディフューズ色を読み取る(拡散反射率)
			Math::Vector4 color;
			s >> color.x >> color.y >> color.z;
			
		} else if (materialIdentifier == "Ks") {
			// スペキュラ色(鏡面反射率)
			Math::Vector4 color;
			s >> color.x >> color.y >> color.z;
			

		} else if (materialIdentifier == "Ke") {
			// 自己発光
			Math::Vector4 color;
			s >> color.x >> color.y >> color.z;
			

		} else if (materialIdentifier == "Ni") {
			// 屈折率
			float refraction;
			s >> refraction;
			

		} else if (materialIdentifier == "Ns") {
			// shininess(鏡面反射の鋭さ)
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeを返す
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::Model::Node LoadNode(const std::string& directoryPath, const std::string& fileName) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	
	AOENGINE::Model::Node result;
	result = ReadNode(scene->mRootNode, scene);

	return result;
}

AOENGINE::Model::Node ReadNode(aiNode* node, const aiScene* scene) {
	AOENGINE::Model::Node result;
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

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Animationを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

std::unordered_map<std::string, Animation> LoadAnimation(const std::string directoryPath, const std::string& animationFile) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + animationFile;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	if (!scene || scene->mNumAnimations == 0) {
		throw std::runtime_error("Failed to load animations or no animations present");
	}

	AOENGINE::Logger::Log("[Load Animation] :" + animationFile + "]\n");

	std::unordered_map<std::string, Animation> animationMap{};
	uint32_t unnamedAnimationIndex = 0;

	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
		// sceneからanimationの情報を取得する
		aiAnimation* animationAssimp = scene->mAnimations[animationIndex];

		std::string animationName = animationAssimp->mName.C_Str();										// animationの名前
		if (animationName.empty()) {
			do {
				animationName = "Animation(" + std::to_string(unnamedAnimationIndex++) + ")";
			} while (animationMap.contains(animationName));
		}
		Animation animationData{};																		// animationのデータ
		animationData.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);	// 時間の単位を秒に変換
		animationData.animationName = animationName;													// animatonの名前を取得

		
		// -------------------------------------------------
		// ↓ アニメーションの解析
		// -------------------------------------------------
		// assimpでは個々のNodeのAnimationをchannelと呼ぶ
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = animationData.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

			// -------------------------------------------------
			// ↓ Math::Vector3の読み込み
			// -------------------------------------------------
			
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				KeyframeVector3 keyframe{};
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);	// 秒に変換
				keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y, keyAssimp.mValue.z };
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}

			// -------------------------------------------------
			// ↓ Math::Quaternionの読み込み
			// -------------------------------------------------
			
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {

				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				KeyframeQuaternion keyframe{};
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);	// 秒に変換
				keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w };
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}

			// -------------------------------------------------
			// ↓ Scaleの読み込み
			// -------------------------------------------------
			
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				KeyframeVector3 keyframe{};
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);	// 秒に変換
				keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y, keyAssimp.mValue.z };
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}

		animationMap.try_emplace(animationName, animationData);
	}

	AOENGINE::Logger::Log("success\n");

	return animationMap;
}

std::vector<std::unique_ptr<SkinCluster>> LoadSkinCluster(const std::string& directoryPath, const std::string& fileName) {
	Assimp::Importer importer;
	std::string filePath = directoryPath + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	assert(scene->HasMeshes()); // meshがないのは対応しない

	std::vector<std::unique_ptr<SkinCluster>> result;
	std::map<std::string, JointWeightData> combinedSkinCluster;
	uint32_t baseVertex = 0;
	
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// -------------------------------------------------
		// ↓ skinningを取得する用の処理
		// -------------------------------------------------
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			// jointごとの格納領域を作る
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = combinedSkinCluster[jointName];

			// InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Math::Matrix4x4 bindPoseMatrix = Math::Matrix4x4::MakeAffine({ scale.x, scale.y, scale.z },
															 Math::Quaternion{ rotate.x, -rotate.y, -rotate.z, rotate.w },
															 { -translate.x, translate.y, translate.z }
			);
			jointWeightData.inverseBindPoseMatrix = bindPoseMatrix.Inverse();

			// Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeight.push_back({
					bone->mWeights[weightIndex].mWeight,
					baseVertex + bone->mWeights[weightIndex].mVertexId
				});
			}
		}
		baseVertex += mesh->mNumVertices;
	}

	auto& combinedData = result.emplace_back(std::make_unique<SkinCluster>());
	combinedData->AddData(combinedSkinCluster);

	return result;
}

//================================================================================================//
//
// ファイル拡張子の探索
//
//================================================================================================//

const char* GetFileExtension(const char* filename) {
	const char* ext = std::strrchr(filename, '.'); // 最後のピリオドを探す
	if (ext == nullptr) {
		return ""; // 拡張子がない場合は空文字を返す
	}
	return ext + 1; // ピリオドの次の文字から拡張子を返す
}

std::string RemoveExtension(const std::string& filename) {
	size_t dotPos = filename.find_last_of('.');
	if (dotPos != std::string::npos) {
		return filename.substr(0, dotPos); // ドットより前の部分を返す
	}
	return filename; // ドットがない場合はそのまま返す
}
