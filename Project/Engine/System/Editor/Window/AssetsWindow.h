#pragma once
#include <Engine/Lib/Math/MyMath.h>
#include <array>
#include <list>
#include <string>
#include <filesystem>
#include <unordered_map>
#include "Engine/Module/Components/Attribute/IEditorWindow.h"
#include "Engine/System/Manager/TextureManager.h"

namespace AOENGINE {

/// <summary>
/// アセットツリーのノード情報
/// </summary>
struct AssetNode {
	std::string name;
	std::filesystem::path path;
	std::vector<AssetNode> children;
};

/// <summary>
/// Assets内のファイルを実行画面に表示するクラス
/// </summary>
class AssetsWindow :
	public IEditorWindow {
public: // コンストラクタ

	AssetsWindow() = default;
	~AssetsWindow() override;

public: // method

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 指定したobjectの詳細を表示するwindow
	/// </summary>
	void InspectorWindow() override;

	/// <summary>
	/// 階層を表示するwindow
	/// </summary>
	void HierarchyWindow() override;

	/// <summary>
	/// 実行画面となるwindow
	/// </summary>
	void ExecutionWindow() override;

	/// <summary>
	/// Assetのツリーを構築する
	/// </summary>
	/// <param name="dirPath"></param>
	AssetNode BuildAssetTree(const std::filesystem::path& dirPath);

private:

	/// <summary>
	/// 選択したフォルダの中の項目を構築する
	/// </summary>
	void BuildCurrentFolderItems();
	bool SetCurrentPath(const std::filesystem::path& path);
	void SyncCurrentPathInput();
	void DrawFolderToolBar();

	/// <summary>
	/// Treeの描画を行う
	/// </summary>
	/// <param name="node"></param>
	void DrawAssetTree(const AssetNode& node);

	/// <summary>
	/// フォルダーの中の項目を表示する
	/// </summary>
	void DrawFolderItems();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="textureName"></param>
	/// <param name="fileName"></param>
	/// <param name="size"></param>
	bool DrawItemTexture(AssetType assetType, const std::string& textureName, const std::string& fileName, float size);

	void DropSource(AssetType assetType, const std::string& name);

private:

	TextureManager* pTextureManager_ = nullptr;

	std::filesystem::path currentPath_ = "";
	std::array<char, 512> currentPathBuffer_ = {};
	std::array<char, 256> searchBuffer_ = {};

	AssetNode rootNode_;
	std::unordered_map<std::string, bool> treeOpenState;

	std::list<std::filesystem::path> currentFolderItemList_;
	
};

}
