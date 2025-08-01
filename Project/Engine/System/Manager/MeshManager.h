#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "Engine/Module/Components/Meshes/Mesh.h"

class MeshManager {
public:

	struct MeshPair {
		std::string meshName;
		std::shared_ptr<Mesh> mesh;

		MeshPair(const std::string& name, std::shared_ptr<Mesh> pMesh) {
			meshName = name;
			mesh = std::move(pMesh);
		}
	};

	struct MeshArray {
		std::vector<MeshPair> meshArray;
	};

public:

	MeshManager() = default;
	~MeshManager();
	MeshManager(const MeshManager&) = delete;
	const MeshManager& operator=(const MeshManager&) = delete;

	static MeshManager* GetInstance();

	void Init();
	void Finalize();

	static void AddMesh(ID3D12Device* device, const std::string& modelName,
						const std::string& meshName, const std::vector<VertexData>& vertexData, const std::vector<uint32_t>& indices);

	static std::vector<std::shared_ptr<Mesh>> GetMeshes(const std::string& modelName);

	bool ExistMesh(const std::string& modelName);

	std::shared_ptr<Mesh> GetMesh(const std::string& meshName);

	std::string SelectMeshName();

private:

	static std::unordered_map<std::string, MeshArray> meshArrayMap_;
	static std::unordered_map<std::string, std::shared_ptr<Mesh>> meshMap_;
	static std::vector<std::string> modelNameList_;
	static std::vector<std::string> meshNameList_;

};

