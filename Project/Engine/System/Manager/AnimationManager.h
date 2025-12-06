#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/Rigging/Skeleton.h"
#include "Engine/Module/Components/Animation/AnimationStructures.h"

/// <summary>
/// animationの情報をまとめたクラス
/// </summary>
class AnimationManager {
public:

	AnimationManager() = default;
	~AnimationManager();
	AnimationManager(const AnimationManager&) = delete;
	const AnimationManager& operator=(const AnimationManager&) = delete;

	static AnimationManager* GetInstance();

public:

	// 初期化
	void Init();

	/// <summary>
	/// Animationが存在するかチェックする
	/// </summary>
	/// <param name="animationFile">: アニメーションファイル名</param>
	/// <returns></returns>
	bool CheckAnimationMap(const std::string& animationFile);

	/// <summary>
	/// Animationファイルを読み込む
	/// </summary>
	/// <param name="directory"></param>
	/// <param name="animationFile"></param>
	/// <returns></returns>
	Animation LoadAnimationFile(const std::string& directory, const std::string& animationFile);

	/// <summary>
	/// Animationの追加
	/// </summary>
	/// <param name="animation"></param>
	/// <param name="animationFileName"></param>
	void AddMap(const std::unordered_map<std::string, Animation>& animation, const std::string& animationFileName);

	/// <summary>
	/// 指定されたAnimationを取得する
	/// </summary>
	/// <param name="animationFile">: animationが格納されているファイル名</param>
	/// <param name="animationName">: 行いたいanimation名</param>
	/// <returns></returns>
	Animation GetAnimation(const std::string& fileName, const std::string& animationName);
	
	/// <summary>
	/// 指定したファイルが持っている最初のanimationを返す
	/// </summary>
	/// <param name="animationFileName"></param>
	/// <returns></returns>
	std::string GetAnimationFirstName(const std::string& animationFileName);

	/// <summary>
	/// 指定したモデルが持っているアニメーションの名前を返す
	/// </summary>
	/// <param name="animationFileName"></param>
	/// <returns></returns>
	std::vector<std::string>& GetModelHaveAnimationNames(const std::string& animationFileName) { return modelHaveAnimationNames_[animationFileName]; }

	/// <summary>
	/// 指定したアニメーションが存在しているかをチェックする
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	bool CheckAnimation(const std::string& name);

private:

	std::unordered_map<std::string, std::unordered_map<std::string, Animation>> animationMap_;

	std::unordered_map<std::string, std::vector<std::string>> modelHaveAnimationNames_;

};

