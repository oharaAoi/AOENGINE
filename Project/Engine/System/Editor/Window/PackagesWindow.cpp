#include "PackagesWindow.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/System/Editor/Window/PackagesWindowSerializer.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"


AOENGINE::PackagesWindow::~PackagesWindow() {
	AOENGINE::PackagesWindowSerializer::Save(treeOpenState, currentPath_.string());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PackagesWindow::Init() {
	pTextureManager_ = AOENGINE::TextureManager::GetInstance();

	std::filesystem::path dirPath = "./Project/Packages";
	rootNode_ = BuildAssetTree(dirPath);

	// 保存状況の読み込み
	std::string path = "";
	AOENGINE::PackagesWindowSerializer::Load(treeOpenState, path);

	// フォルダ内のアイテムを構築
	if (path != "") {
		std::filesystem::path currentPath = path;
		currentPath_ = currentPath;
		currentFolderItemList_.clear();
		BuildCurrentFolderItems();
}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　指定したobjectの詳細を表示するwindow
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PackagesWindow::InspectorWindow() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Packagesの階層を表示するクラス
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PackagesWindow::HierarchyWindow() {
	static float leftWidth = 250.0f;
	const float splitterWidth = 4.0f;
	// この行で「このウィンドウの残り高さ」を先に確保する
	float fullHeight = ImGui::GetContentRegionAvail().y;
	if (fullHeight < 1.0f) fullHeight = 1.0f;

	// windowの表示
	ImGui::Begin("Packages", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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

void AOENGINE::PackagesWindow::ExecutionWindow() {

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Assetのツリーを構築する
//////////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::AssetNode AOENGINE::PackagesWindow::BuildAssetTree(const std::filesystem::path& dirPath) {
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

void AOENGINE::PackagesWindow::BuildCurrentFolderItems() {
	for (const auto& entry : std::filesystem::directory_iterator(currentPath_)) {
		currentFolderItemList_.push_back(entry.path());
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

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeの描画を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::PackagesWindow::DrawAssetTree(const AssetNode& node) {
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
		currentPath_ = node.path;
		currentFolderItemList_.clear();
		BuildCurrentFolderItems();
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

void AOENGINE::PackagesWindow::DrawFolderItems() {
	// パラメータの設定
	const float thumbnailSize = 64.0f;
	const float padding = 16.0f;

	const float startX = ImGui::GetCursorPosX();
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / (thumbnailSize + padding));
	if (columnCount < 1) columnCount = 1;

	int column = 0;

	// 表示する
	for (const auto& item : currentFolderItemList_) {
		ImGui::BeginGroup();

		// 画像ファイルのicon表示
		if (item.filename().extension() == ".png" || item.filename().extension() == ".jpeg") {
			std::string name = item.filename().string();
			DrawItemTexture(name, name, thumbnailSize);

			// 音声ファイルのicon表示
		} else if (item.filename().extension() == ".wav" || item.filename().extension() == ".mp3") {
			std::string name = item.filename().string();
			DrawItemTexture("music.png", name, thumbnailSize);

			// ddsファイルのicon表示
		} else if (item.filename().extension() == ".dds") {
			std::string name = item.filename().string();
			DrawItemTexture("dds.png", name, thumbnailSize);

			// modelファイルのicon表示
		} else if (item.filename().extension() == ".obj" || item.filename().extension() == ".gltf") {
			std::string name = item.filename().string();
			DrawItemTexture("3dModel.png", name, thumbnailSize);

			// treeファイルかどうか
		} else if (item.filename().extension() == ".aitree") {
			std::string name = item.filename().string();
			if (DrawItemTexture("AI.png", name, thumbnailSize)) {
				AI::BehaviorTreeSystem::GetInstance()->SetIsOpenEditor(name);
			}

			// folderのファイルのicon表示
		} else if (std::filesystem::is_directory(item)) {
			if (DrawItemTexture("folder.png", item.filename().string(), thumbnailSize)) {
				currentPath_ = item.string();
				currentFolderItemList_.clear();
				BuildCurrentFolderItems();
				ImGui::EndGroup();
				return;
			}
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

bool AOENGINE::PackagesWindow::DrawItemTexture(const std::string& textureName, const std::string& fileName, float size) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	D3D12_GPU_DESCRIPTOR_HANDLE texHandle = pTextureManager_->GetDxHeapHandles(textureName).handleGPU;
	ImTextureID texId = reinterpret_cast<ImTextureID>(texHandle.ptr);
	std::string guiId = "##" + fileName;
	bool result = false;

	if (ImGui::ImageButton(guiId.c_str(), texId, ImVec2(size, size))) {
		result = true;
	}
	ImGui::PopStyleColor(1);

	// nameの表示
	std::string name = EllipsisText(fileName, size);
	ImGui::TextWrapped("%s", name.c_str());

	return result;
}
