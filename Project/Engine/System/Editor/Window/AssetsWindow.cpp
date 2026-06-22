#include "AssetsWindow.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/System/Editor/Window/AssetsWindowSerializer.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Lib/Path.h"
#include <algorithm>
#include <cctype>

namespace {

template<size_t Size>
void CopyToInputBuffer(std::array<char, Size>& buffer, const std::string& text) {
	buffer.fill('\0');
	const size_t copySize = std::min(text.size(), buffer.size() - 1);
	std::copy_n(text.data(), copySize, buffer.data());
}

std::string ToLower(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
				   });
	return value;
}

bool ContainsIgnoreCase(const std::string& text, const std::string& keyword) {
	if (keyword.empty()) {
		return true;
	}
	return ToLower(text).find(ToLower(keyword)) != std::string::npos;
}

}

AOENGINE::AssetsWindow::~AssetsWindow() {
	AOENGINE::AssetsWindowSerializer::Save(treeOpenState, currentPath_.string());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::AssetsWindow::Init() {
	pTextureManager_ = AOENGINE::TextureManager::GetInstance();

	std::filesystem::path dirPath = kAssetPath;
	std::filesystem::path dire(dirPath);
	if (!std::filesystem::exists(dirPath)) {
		std::filesystem::create_directories(dirPath);
	}
	rootNode_ = BuildAssetTree(dirPath);

	// 保存状況の読み込み
	std::string path = "";
	AOENGINE::AssetsWindowSerializer::Load(treeOpenState, path);

	// フォルダ内のアイテムを構築
	if (path.empty() || !SetCurrentPath(path)) {
		SetCurrentPath(dirPath);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　指定したobjectの詳細を表示するwindow
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::AssetsWindow::InspectorWindow() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Assetsの階層を表示するクラス
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::AssetsWindow::HierarchyWindow() {
	static float leftWidth = 250.0f;
	const float splitterWidth = 4.0f;
	// この行で「このウィンドウの残り高さ」を先に確保する
	float fullHeight = ImGui::GetContentRegionAvail().y;
	if (fullHeight < 1.0f) fullHeight = 1.0f;

	// windowの表示
	ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	// folderTreeの表示
	ImGui::BeginChild("##Left", ImVec2(leftWidth, 0), true);
	DrawAssetTree(rootNode_);
	ImGui::EndChild();

	ImGui::SameLine();

	// windowを分割する
	ImGui::InvisibleButton("##Splitter", ImVec2(splitterWidth, fullHeight));
	if (ImGui::IsItemHovered()) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
	if (ImGui::IsItemActive()) {
		leftWidth += ImGui::GetIO().MouseDelta.x;
		// 最低幅をクランプ（好みで調整）
		leftWidth = std::max(leftWidth, 120.0f);
	}

	ImGui::SameLine(0.0f, 0.0f);

	// folder内のアイテムを表示
	ImGui::BeginChild("##Right", ImVec2(0, 0), true);
	DrawFolderItems();
	ImGui::EndChild();

	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　実行画面となるwindow
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::AssetsWindow::ExecutionWindow() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Assetのツリーを構築する
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::AssetNode AOENGINE::AssetsWindow::BuildAssetTree(const std::filesystem::path& dirPath) {
	AssetNode node;
	node.name = dirPath.filename().string();
	node.path = dirPath;

	// フォルダ内のアイテムをchildに入れる
	for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
		AssetNode child;
		if (entry.is_directory()) {
			node.children.push_back(
				BuildAssetTree(entry.path())
			);
		}
	}

	// rootNodeを返す
	return node;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Folder内のアイテムをまとめる
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::AssetsWindow::BuildCurrentFolderItems() {
	currentFolderItemList_.clear();

	std::error_code ec;
	if (!std::filesystem::exists(currentPath_, ec) || ec || !std::filesystem::is_directory(currentPath_, ec)) {
		return;
	}

	std::filesystem::directory_iterator it(currentPath_, std::filesystem::directory_options::skip_permission_denied, ec);
	std::filesystem::directory_iterator end;
	for (; it != end; it.increment(ec)) {
		if (ec) {
			ec.clear();
			continue;
		}
		currentFolderItemList_.push_back(it->path());
	}

	// folderを先頭に並び替える
	currentFolderItemList_.sort(
		[](const std::filesystem::path& a, const std::filesystem::path& b) {
			const bool aIsDir = std::filesystem::is_directory(a);
			const bool bIsDir = std::filesystem::is_directory(b);

			// フォルダを先に
			if (aIsDir != bIsDir) {
				return aIsDir > bIsDir;
			}

			// 同じ種類なら名前順（任意）
			return a.filename().string() < b.filename().string();
		}
	);
}

bool AOENGINE::AssetsWindow::SetCurrentPath(const std::filesystem::path& path) {
	std::error_code ec;
	if (!std::filesystem::exists(path, ec) || ec || !std::filesystem::is_directory(path, ec)) {
		return false;
	}

	currentPath_ = path.lexically_normal();
	BuildCurrentFolderItems();
	SyncCurrentPathInput();
	return true;
}

void AOENGINE::AssetsWindow::SyncCurrentPathInput() {
	CopyToInputBuffer(currentPathBuffer_, currentPath_.string());
}

void AOENGINE::AssetsWindow::DrawFolderToolBar() {
	if (currentPathBuffer_[0] == '\0' && !currentPath_.empty()) {
		SyncCurrentPathInput();
	}

	ImGui::Text("Path");
	ImGui::SameLine();
	const float goButtonWidth = 38.0f;
	const float pathInputWidth = std::max(120.0f, ImGui::GetContentRegionAvail().x - goButtonWidth - ImGui::GetStyle().ItemSpacing.x);
	ImGui::SetNextItemWidth(pathInputWidth);
	bool applyPath = ImGui::InputText("##AssetCurrentPath", currentPathBuffer_.data(), currentPathBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine();
	applyPath |= ImGui::Button("Go", ImVec2(goButtonWidth, 0.0f));
	if (applyPath) {
		SetCurrentPath(std::filesystem::path(currentPathBuffer_.data()));
	}

	ImGui::Text("Search");
	ImGui::SameLine();
	const float clearButtonWidth = 24.0f;
	const float searchInputWidth = std::max(120.0f, ImGui::GetContentRegionAvail().x - clearButtonWidth - ImGui::GetStyle().ItemSpacing.x);
	ImGui::SetNextItemWidth(searchInputWidth);
	ImGui::InputTextWithHint("##AssetSearch", "Search file...", searchBuffer_.data(), searchBuffer_.size());
	ImGui::SameLine();
	if (ImGui::Button("X", ImVec2(clearButtonWidth, 0.0f))) {
		searchBuffer_[0] = '\0';
	}

	ImGui::Separator();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeの描画を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::AssetsWindow::DrawAssetTree(const AssetNode& node) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_SpanFullWidth;
	auto key = node.path.string();

	// mapから状態を参照する
	auto it = treeOpenState.find(key);
	if (it != treeOpenState.end()) {
		ImGui::SetNextItemOpen(it->second, ImGuiCond_Always);
	}

	// 開いているかどうか
	bool opened = ImGui::TreeNodeEx((node.name + "##" + node.path.string()).c_str(), flags);
	treeOpenState[key] = opened;

	// ファイルクリック
	if (ImGui::IsItemClicked()) {
		// 選択処理
		SetCurrentPath(node.path);
	}

	// 次のfolderを表示
	if (opened) {
		for (const auto& child : node.children) {
			DrawAssetTree(child);
		}
		ImGui::TreePop();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Folder内のitemを表示する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::AssetsWindow::DrawFolderItems() {
	DrawFolderToolBar();

	// パラメータの設定
	const float thumbnailSize = 64.0f;
	const float padding = 16.0f;

	const float startX = ImGui::GetCursorPosX();
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / (thumbnailSize + padding));
	if (columnCount < 1) columnCount = 1;

	int column = 0;

	// 表示する
	const std::string searchText = searchBuffer_.data();
	for (const auto& item : currentFolderItemList_) {
		if (!ContainsIgnoreCase(item.filename().string(), searchText)) {
			continue;
		}

		ImGui::BeginGroup();

		// 画像ファイルのicon表示
		if (item.filename().extension() == ".png" || item.filename().extension() == ".jpeg") {
			std::string name = item.filename().string();
			DrawItemTexture(AssetType::Texture, name, name, thumbnailSize);

			// 音声ファイルのicon表示
		} else if (item.filename().extension() == ".wav" || item.filename().extension() == ".mp3") {
			std::string name = item.filename().string();
			DrawItemTexture(AssetType::Sound, "music.png", name, thumbnailSize);

			// ddsファイルのicon表示
		} else if (item.filename().extension() == ".dds") {
			std::string name = item.filename().string();
			DrawItemTexture(AssetType::Other, "dds.png", name, thumbnailSize);

			// modelファイルのicon表示
		} else if (item.filename().extension() == ".obj" || item.filename().extension() == ".gltf") {
			std::string name = item.filename().string();
			DrawItemTexture(AssetType::Model, "3dModel.png", name, thumbnailSize);

			// treeファイルかどうか
		} else if (item.filename().extension() == ".aitree") {
			std::string name = item.filename().string();
			if (DrawItemTexture(AssetType::AI, "AI.png", name, thumbnailSize)) {
				AI::BehaviorTreeSystem::GetInstance()->SetIsOpenEditor(name);
			}

			// folderのファイルのicon表示
		} else if (std::filesystem::is_directory(item)) {
			if (DrawItemTexture(AssetType::Other, "folder.png", item.filename().string(), thumbnailSize)) {
				SetCurrentPath(item);
				ImGui::EndGroup();
				return;
			}
		} else {
			std::string name = item.filename().string();
			DrawItemTexture(AssetType::Other, "file.png", name, thumbnailSize);
		}

		// フォルダの表示

		ImGui::EndGroup();

		// 折り返し表示
		column++;
		if (column < columnCount) {
			ImGui::SameLine(startX + column * (thumbnailSize + padding));
		} else {
			column = 0;
		}
	}
}

bool AOENGINE::AssetsWindow::DrawItemTexture(AssetType assetType, const std::string& textureName, const std::string& fileName, float size) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	D3D12_GPU_DESCRIPTOR_HANDLE texHandle = pTextureManager_->GetDxHeapHandles(textureName).handleGPU;
	ImTextureID texId = reinterpret_cast<ImTextureID>(texHandle.ptr);
	std::string guiId = "##" + fileName;
	bool result = false;

	if (ImGui::ImageButton(guiId.c_str(), texId, ImVec2(size, size))) {
		result = true;
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Always);
		ImGui::BeginTooltip();
		ImGui::Text("%s", fileName.c_str());
		ImGui::EndTooltip();
	}

	if (assetType != AssetType::Other) {
		DropSource(assetType, fileName);
	}
	ImGui::PopStyleColor(1);

	// nameの表示
	std::string name = EllipsisText(fileName, size);
	ImGui::TextWrapped("%s", name.c_str());

	return result;
}

void AOENGINE::AssetsWindow::DropSource(AssetType assetType, const std::string& name) {
	if (ImGui::BeginDragDropSource()) {
		if (assetType == AssetType::Texture) {
			auto  handle = TextureManager::GetInstance()->SearchAssetHandle(name);
			if (handle) {
				ImGui::SetDragDropPayload("ASSET_HANDLE", &handle, sizeof(handle));
				ImGui::Text("Texture");
			}

		} else if (assetType == AssetType::Model) {
			auto  handle = ModelManager::GetInstance()->SearchAssetHandle(name);
			if (handle) {
				ImGui::SetDragDropPayload("ASSET_HANDLE", &handle, sizeof(handle));
				ImGui::Text("Model");
			}

		} else if (assetType == AssetType::Sound) {

		} else if (assetType == AssetType::Material) {

		}
		ImGui::EndDragDropSource();
	}
}
