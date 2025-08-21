#pragma once
#include <vector>
#include <string>
#include <unordered_map>
//#include <Lib/tiny_gltf.h> // Assimpの場合使わない
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Rigging/SkinCluster.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/ViewProjection.h"
#include "Engine/Module/Components/Animation/AnimationStructures.h"
#include "Engine/Lib/Math/MathStructures.h"
#include <memory>

class Material;
class PBRMaterial;

class Model {
public:

	struct NodeAnimationData {
		std::vector<NodeAnimation> animations;
		float tickPerSecond;	// 一秒間に何回の処理が行われるか
		float duration;			// tickPerSecondの持続時間
		float animationTime;	// animationをする時間
	};

	struct Node {
		QuaternionSRT transform;
		Matrix4x4 localMatrix;				 // NodeのLocalMatrix
		std::string name;					 // Nodeの名前
		std::vector<Node> children;			 // 子供のNode
		NodeAnimationData animationsData;	 // ノードに関するアニメーション
	};

public:

	Model();
	~Model();

	void Init(ID3D12Device* device, const std::string& directorPath, const std::string& fileName);
	
	void Draw(ID3D12GraphicsCommandList* commandList,
			  const Pipeline* pipeline,
			  const WorldTransform* worldTransform, const ViewProjection* viewprojection,
			  const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials);
	
	void Draw(ID3D12GraphicsCommandList* commandList,
			  const Pipeline* pipeline,
			  const WorldTransform* worldTransform, const ViewProjection* viewprojection,
			  const D3D12_VERTEX_BUFFER_VIEW& vbv, const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials);

	void Debug_Gui(const std::string& name);

	/// <summary>
	/// assimpでのNode解析
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	Node ReadNode(aiNode* node, const aiScene* scene);

public:

	const std::string& GetRootNodeName() const { return rootNode_.name; }

	Node& GetNode() { return rootNode_; }

	const std::map<std::string, JointWeightData>& GetSkinClustersData(uint32_t index) { return skinClusterArray_[index]->GetSkinClustersData(); }

	std::unordered_map<std::string, ModelMaterialData>& GetMaterialData() { return materialData_; }

	Mesh* GetMesh(const uint32_t& index);
	size_t GetMeshsNum() const { return meshArray_.size(); }

	const size_t GetMaterialsSize() const { return materialData_.size(); }

private:

	// 頂点バッファやインデックスバッファを持つ
	std::vector<std::shared_ptr<Mesh>> meshArray_;
	// materialの情報
	std::unordered_map<std::string, ModelMaterialData> materialData_;
	// skinningのデータ
	std::vector<std::unique_ptr<SkinCluster>> skinClusterArray_;
	// ノード
	Node rootNode_;
};