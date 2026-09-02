#include "AssetsWindow.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/AI/BehaviorTreeSystem.h"
#include "Engine/System/Editor/Window/AssetsWindowSerializer.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/Lib/Path.h"
#include <algorithm>
#include <cctype>
#include <cfloat>
#include <utility>
#include <vector>

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

std::string GetPrefabName(const std::filesystem::path& path) {
	std::string name = path.filename().string();
	constexpr std::string_view suffix = ".prefab.json";
	if (name.size() >= suffix.size() && ToLower(name).ends_with(suffix)) {
		name.resize(name.size() - suffix.size());
	}
	return name;
}

const char* GetPrefabOperationMessage(AOENGINE::PrefabOperationResult result) {
	using AOENGINE::PrefabOperationResult;
	switch (result) {
	case PrefabOperationResult::NotFound: return "Prefab file was not found.";
	case PrefabOperationResult::AlreadyExists: return "A Prefab with that name already exists.";
	case PrefabOperationResult::InvalidName: return "The Prefab name is invalid.";
	case PrefabOperationResult::ReferencedByScene: return "The Prefab is referenced by a scene.";
	case PrefabOperationResult::FileSystemError: return "The file operation failed.";
	case PrefabOperationResult::InvalidPrefab: return "The Prefab file is invalid.";
	default: return "";
	}
}

void DrawAssetFileName(const std::string& fileName, float width) {
	ImFont* font = ImGui::GetFont();
	const float fontSize = ImGui::GetFontSize() * 0.8f;
	const float fontScale = fontSize / font->FontSize;
	const char* text = fileName.c_str();
	const char* textEnd = text + fileName.size();

	std::vector<std::pair<const char*, const char*>> lines;
	while (text < textEnd) {
		const char* lineEnd = font->CalcWordWrapPositionA(fontScale, text, textEnd, width);
		if (lineEnd == text) {
			lineEnd++;
			while (lineEnd < textEnd && (*lineEnd & 0xC0) == 0x80) {
				lineEnd++;
			}
		}

		const char* visibleEnd = lineEnd;
		while (visibleEnd > text && (visibleEnd[-1] == ' ' || visibleEnd[-1] == '\t')) {
			visibleEnd--;
		}
		lines.emplace_back(text, visibleEnd);

		text = lineEnd;
		while (text < textEnd && (*text == ' ' || *text == '\t')) {
			text++;
		}
	}

	const ImVec2 start = ImGui::GetCursorScreenPos();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
	for (size_t lineIndex = 0; lineIndex < lines.size(); ++lineIndex) {
		const auto& [lineBegin, lineEnd] = lines[lineIndex];
		const float lineWidth = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, lineBegin, lineEnd).x;
		const ImVec2 position(
			start.x + std::max(0.0f, (width - lineWidth) * 0.5f),
			start.y + static_cast<float>(lineIndex) * fontSize);
		drawList->AddText(font, fontSize, position, textColor, lineBegin, lineEnd);
	}

	ImGui::Dummy(ImVec2(width, std::max(fontSize, static_cast<float>(lines.size()) * fontSize)));
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

		// モデルに付随するマテリアルファイルはAssetsウィンドウに表示しない
		if (ToLower(it->path().extension().string()) == ".mtl") {
			continue;
		}

		if (ToLower(it->path().extension().string()) == ".bin") {
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
	DrawPrefabDropTarget();

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
		const std::string lowerFileName = ToLower(item.filename().string());

		// 画像ファイルのicon表示
		if (lowerFileName.ends_with(".prefab.json")) {
			const std::string name = item.filename().string();
			DrawItemTexture(AssetType::Other, "file.png", name, thumbnailSize, &item);

		} else if (item.filename().extension() == ".png" || item.filename().extension() == ".jpeg") {
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

	DrawPrefabOperationDialogs();
}

void AOENGINE::AssetsWindow::DrawPrefabDropTarget() {
	const float width = ImGui::GetContentRegionAvail().x;
	ImGui::Button("Drop GameObject here to create Prefab", ImVec2(width, 36.0f));
	if (!ImGui::BeginDragDropTarget()) { return; }

	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT_HANDLE")) {
		if (payload->DataSize == sizeof(ObjectHandle)) {
			const ObjectHandle handle = *static_cast<const ObjectHandle*>(payload->Data);
			SceneRenderer* renderer = SceneRenderer::GetInstance();
			SceneObject* object = renderer ? renderer->FindObject(handle) : nullptr;
			if (object && object->GetScenePersistence() == ScenePersistence::SceneData) {
				const std::string prefabName = MakeUniquePrefabName(object->GetName());
				if (PrefabManager::GetInstance()->SavePrefab(prefabName, handle, *renderer)) {
					rootNode_ = BuildAssetTree(kAssetPath);
					BuildCurrentFolderItems();
				}
			}
		}
	}
	ImGui::EndDragDropTarget();
	ImGui::Separator();
}

std::string AOENGINE::AssetsWindow::MakeUniquePrefabName(const std::string& objectName) const {
	std::string baseName = objectName.empty() ? "New Prefab" : objectName;
	for (char& character : baseName) {
		if (character == '<' || character == '>' || character == ':' || character == '"' ||
			character == '/' || character == '\\' || character == '|' || character == '?' || character == '*') {
			character = '_';
		}
	}

	const std::filesystem::path directory = PrefabManager::GetInstance()->GetPrefabDirectory();
	std::string candidate = baseName;
	uint32_t suffix = 1;
	while (std::filesystem::exists(directory / (candidate + ".prefab.json"))) {
		candidate = baseName + " " + std::to_string(suffix++);
	}
	return candidate;
}

bool AOENGINE::AssetsWindow::DrawItemTexture(AssetType assetType, const std::string& textureName,
	const std::string& fileName, float size, const std::filesystem::path* prefabPath) {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	D3D12_GPU_DESCRIPTOR_HANDLE texHandle = pTextureManager_->GetDxHeapHandles(textureName).handleGPU;
	ImTextureID texId = reinterpret_cast<ImTextureID>(texHandle.ptr);
	std::string guiId = "##" + fileName;
	bool result = false;

	if (ImGui::ImageButton(guiId.c_str(), texId, ImVec2(size, size))) {
		result = true;
	}

	// BeginDragDropSourceはドラッグ元となるImageButtonがLastItemの間に呼ぶ必要がある。
	// ファイル名描画後ではDummyがLastItemになり、ImGuiのID assertionが発生する。
	if (prefabPath) {
		DrawPrefabContextMenu(*prefabPath);
		PrefabDropSource(*prefabPath);
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

	// UnityのProjectビューのように、アイコン下へ小さめの文字で中央揃えして折り返す
	DrawAssetFileName(fileName, size);

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

void AOENGINE::AssetsWindow::PrefabDropSource(const std::filesystem::path& path) {
	if (!ImGui::BeginDragDropSource()) { return; }

	std::string prefabName = path.filename().string();
	constexpr std::string_view suffix = ".prefab.json";
	if (prefabName.size() >= suffix.size()) {
		prefabName.resize(prefabName.size() - suffix.size());
	}
	ImGui::SetDragDropPayload("PREFAB_ASSET", prefabName.c_str(), prefabName.size() + 1);
	ImGui::Text("Prefab: %s", prefabName.c_str());
	ImGui::EndDragDropSource();
}

void AOENGINE::AssetsWindow::DrawPrefabContextMenu(const std::filesystem::path& path) {
	const std::string pathId = path.string();
	ImGui::PushID(pathId.c_str());
	if (!ImGui::BeginPopupContextItem("##PrefabContextMenu")) {
		ImGui::PopID();
		return;
	}

	if (ImGui::MenuItem("Rename")) {
		selectedPrefabPath_ = path;
		CopyToInputBuffer(prefabRenameBuffer_, GetPrefabName(path));
		prefabOperationError_.clear();
		openPrefabRenamePopup_ = true;
	}
	if (ImGui::MenuItem("Delete")) {
		selectedPrefabPath_ = path;
		prefabOperationError_.clear();
		openPrefabDeletePopup_ = true;
	}
	ImGui::EndPopup();
	ImGui::PopID();
}

void AOENGINE::AssetsWindow::DrawPrefabOperationDialogs() {
	if (openPrefabRenamePopup_) {
		ImGui::OpenPopup("Rename Prefab");
		openPrefabRenamePopup_ = false;
	}
	if (openPrefabDeletePopup_) {
		ImGui::OpenPopup("Delete Prefab");
		openPrefabDeletePopup_ = false;
	}

	if (ImGui::BeginPopupModal("Rename Prefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		const std::string oldName = GetPrefabName(selectedPrefabPath_);
		ImGui::Text("Rename '%s'", oldName.c_str());
		ImGui::SetNextItemWidth(320.0f);
		const bool submit = ImGui::InputText("##PrefabName", prefabRenameBuffer_.data(),
			prefabRenameBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue);
		if (!prefabOperationError_.empty()) {
			ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", prefabOperationError_.c_str());
		}

		if (submit || ImGui::Button("Rename")) {
			const PrefabOperationResult result = PrefabManager::GetInstance()->RenamePrefab(
				oldName, prefabRenameBuffer_.data());
			if (result == PrefabOperationResult::Success) {
				RefreshAssets();
				ImGui::CloseCurrentPopup();
			} else {
				prefabOperationError_ = GetPrefabOperationMessage(result);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Delete Prefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		const std::string prefabName = GetPrefabName(selectedPrefabPath_);
		const PrefabReferenceInfo references = PrefabManager::GetInstance()->FindReferences(prefabName);
		ImGui::Text("Delete '%s.prefab.json'?", prefabName.c_str());
		if (references.HasReferences()) {
			ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f), "This Prefab is currently referenced.");
			if (references.activeInstanceCount != 0) {
				ImGui::BulletText("Active scene instances: %zu", references.activeInstanceCount);
			}
			for (const std::string& scene : references.sceneFiles) {
				ImGui::BulletText("%s", scene.c_str());
			}
			ImGui::TextWrapped("Deleting anyway keeps active objects but removes their Prefab link. Saved scenes may contain missing references.");
		}
		if (!prefabOperationError_.empty()) {
			ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "%s", prefabOperationError_.c_str());
		}

		const char* deleteLabel = references.HasReferences() ? "Delete Anyway" : "Delete";
		if (ImGui::Button(deleteLabel)) {
			const PrefabOperationResult result = PrefabManager::GetInstance()->DeletePrefab(
				prefabName, references.HasReferences());
			if (result == PrefabOperationResult::Success) {
				RefreshAssets();
				ImGui::CloseCurrentPopup();
			} else {
				prefabOperationError_ = GetPrefabOperationMessage(result);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void AOENGINE::AssetsWindow::RefreshAssets() {
	rootNode_ = BuildAssetTree(kAssetPath);
	BuildCurrentFolderItems();
}
