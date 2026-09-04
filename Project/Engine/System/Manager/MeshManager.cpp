#include "MeshManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <algorithm>

using namespace AOENGINE;

std::unordered_map<std::string, MeshManager::MeshArray> MeshManager::meshArrayMap_;
std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshManager::meshMap_;
std::vector<std::string> MeshManager::modelNameList_;
std::vector<std::string> MeshManager::meshNameList_;

MeshManager::~MeshManager() {
}

MeshManager* AOENGINE::MeshManager::GetInstance() {
	static MeshManager instance;
	return &instance;
}

void MeshManager::Init() {

}

void MeshManager::Finalize() {
	meshArrayMap_.clear();
	meshMap_.clear();
}

std::string MeshManager::CreateKey(const std::string& name) {
	const size_t separatorPos = name.find_last_of("/\\");
	const size_t extensionPos = name.find_last_of('.');

	// 拡張子がない場合や、ドットがディレクトリ名または隠しファイル名の一部である場合は変更しない。
	if (extensionPos == std::string::npos ||
		(separatorPos != std::string::npos && extensionPos <= separatorPos + 1) ||
		(separatorPos == std::string::npos && extensionPos == 0)) {
		return name;
	}

	return name.substr(0, extensionPos);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshの追加
///////////////////////////////////////////////////////////////////////////////////////////////

void MeshManager::AddMesh(ID3D12Device* device, const std::string& modelName, const std::string& meshName,
						  const std::vector<VertexData>& vertexData, const std::vector<uint32_t>& indices) {
	const std::string modelKey = CreateKey(modelName);
	const std::string meshKey = CreateKey(meshName);

	auto it = meshArrayMap_.find(modelKey);
	if (it == meshArrayMap_.end()) {
		MeshPair meshPair(meshKey, std::make_shared<Mesh>());
		meshPair.mesh->Init(device, vertexData, indices);
		// メッシュを登録
		auto& newMeshs = meshArrayMap_[modelKey].meshArray.emplace_back(std::move(meshPair));
		// 名前リストを更新
		modelNameList_.push_back(modelKey);

		// 
		meshMap_[meshKey] = newMeshs.mesh;
		meshNameList_.push_back(meshKey);
	} else {
		MeshPair meshPair(meshKey, std::make_shared<Mesh>());
		meshPair.mesh->Init(device, vertexData, indices);
		// メッシュを登録
		meshArrayMap_[modelKey].meshArray.push_back(std::move(meshPair));
		// 名前リストを更新
		modelNameList_.push_back(meshKey);
	}
}

void MeshManager::AddMesh(ID3D12Device* device, const std::string& modelName, const AOENGINE::Mesh::CreateInfo& createInfo) {
	const std::string modelKey = CreateKey(modelName);
	const std::string meshKey = CreateKey(createInfo.name);
	if (meshArrayMap_.contains(modelKey)) {
		return;
	}

	MeshPair meshPair(meshKey, std::make_shared<Mesh>());
	meshPair.mesh->Init(device, createInfo);
	auto& registeredMesh = meshArrayMap_[modelKey].meshArray.emplace_back(std::move(meshPair));
	modelNameList_.push_back(modelKey);
	meshMap_[meshKey] = registeredMesh.mesh;
	meshNameList_.push_back(meshKey);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshの取得
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<Mesh>> MeshManager::GetMeshes(const std::string& modelName) {
	std::vector<std::shared_ptr<Mesh>> result;

	auto it = meshArrayMap_.find(CreateKey(modelName));
	if (it == meshArrayMap_.end()) {
		assert(false && "Model not found!");
	}

	for (auto& origine : it->second.meshArray) {
		// MeshはGPU ResourceとMapped Addressを所有するため複製せず共有します。
		result.emplace_back(origine.mesh);
	}

	return result;
}

std::shared_ptr<Mesh> MeshManager::GetMesh(const std::string& meshName) {
	const std::string meshKey = CreateKey(meshName);
	for (auto& meshes : meshArrayMap_) {
		const std::vector<MeshPair>& meshPair = meshes.second.meshArray;
		for (uint32_t oi = 0; oi < meshPair.size(); ++oi) {
			if (meshPair[oi].meshName == meshKey) {
				return meshPair[oi].mesh;
			}
		}
	}

	// エラー出力する
	return nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshがあるかをチェック
///////////////////////////////////////////////////////////////////////////////////////////////

bool MeshManager::ExistMesh(const std::string& modelName) {
	if (meshArrayMap_.find(CreateKey(modelName)) == meshArrayMap_.end()) {
		return false;
	} else {
		return true;
	}
}

void MeshManager::RemoveMeshes(const std::string& modelName) {
	const std::string modelKey = CreateKey(modelName);
	auto it = meshArrayMap_.find(modelKey);
	if (it == meshArrayMap_.end()) {
		return;
	}

	for (const MeshPair& meshPair : it->second.meshArray) {
		meshMap_.erase(meshPair.meshName);
		std::erase(meshNameList_, meshPair.meshName);
	}

	meshArrayMap_.erase(it);
	std::erase(modelNameList_, modelKey);
}

std::string MeshManager::SelectMeshName() {
	static std::string selectedFilename;
	static int selectedIndex = -1; // 選択されたインデックス
	if (ImGui::TreeNode("Meshs")) {
		if (ImGui::BeginListBox("Mesh List")) {
			for (int i = 0; i < meshNameList_.size(); ++i) {
				const bool isSelected = (i == selectedIndex);
				std::string textureName = meshNameList_[i];
				if (ImGui::Selectable(textureName.c_str(), isSelected)) {
					selectedIndex = i;
					selectedFilename = meshNameList_[i];
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	return selectedFilename;
}
