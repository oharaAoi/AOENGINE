#pragma once
#include <vector>
#include <memory>
#include "Engine/Module/Components/Animation/AnimationClip.h"
#include "Engine/Module/Components/Rigging/Skinning.h"
#include "Engine/Module/Components/Rigging/Skeleton.h"
#include "Engine/System/Manager/AnimationManager.h"

namespace AOENGINE {

/// <summary>
/// SkeletonやSkinningの情報を持っているクラス
/// </summary>
class Animator {
public:

	Animator();
	~Animator();

public:

	// 終了処理
	void Finalize();
	// 更新処理
	void Update();
	// animationTimeを指定する更新処理
	void UpdateScript(float& animationTime);
	// スキニングの更新
	void UpdateSkinning();

	/// <summary>
	/// animationのデータを読み込みつつ設定を行う
	/// </summary>
	/// <param name="directoryPath">: animationファイルがあるディレクトリパス</param>
	/// <param name="fileName">:animationファイルの名前</param>
	/// <param name="model">: 対応するモデル</param>
	/// <param name="isSkinning">: スキニングを行うか</param>
	/// <param name="isLoop">: animationのloopを行うか</param>
	/// <param name="isControlScript">: animationをスクリプトで制御するか</param>
	void LoadAnimation(const std::string& directoryPath, const std::string& fileName, AOENGINE::Model* model, bool isSkinning, bool isLoop, bool isControlScript);

	/// <summary>
	/// Animationを遷移させる
	/// </summary>
	/// <param name="preAnimation">: 遷移前アニメーション</param>
	/// <param name="afterAnimation">: 遷移後アニメーション</param>
	void TransitionAnimation(const std::string& preAnimation, const std::string& afterAnimation, float blendSpeed);

	/// <summary>
	/// Animationを遷移させる(今のAnimationを指定しない)
	/// </summary>
	/// <param name="afterAnimation"></param>
	void TransitionAnimation(const std::string& afterAnimation, float blendSpeed);

	void Debug_Gui();

public:

	std::string SelectAnimationName() { return animationClip_->SelectAnimationName(); }

	Skinning* GetSkinning(uint32_t index) { return skinning_[index].get(); }
	Skeleton* GetSkeleton() { return skeleton_.get(); }
	AOENGINE::AnimationClip* GetAnimationClip() { return animationClip_.get(); }

	const Math::Matrix4x4 GetAnimationMat() const { return animationClip_->GetMatrix(); }

	const float GetAnimationDuration() const { return animationClip_->GetAnimationDuration(); }

	// 今のanimationの名前を取得
	const std::string& GetAnimationName() const { return animationClip_->GetAnimationName(); }

	// animationの時間を取得・設定
	const float GetAnimationTime() const { return animationClip_->GetAnimationTime(); }	// 取得
	void SetAnimationTime(float time) { animationClip_->SetAnimationTime(time); }		// 設定

	// Animationの切り替えがあるかを取得
	const bool GetIsAnimationChange() const { return animationClip_->GetIsChange(); }

	// モデルに入っているAnimationの名前を取得
	const std::vector<std::string>& GetAnimationNames() { return animationClip_->GetAnimationNames(); }

	// アニメーションの速度を設定する
	void SetAnimationSpeed(float speed) { animationClip_->SetAnimationSpeed(speed); }

	// animationの制御をスクリプトで行うかの取得・設定
	const bool GetIsControlScript() const { return isControlScript_; }			// 取得
	void SetIsControlScript(bool isControl) { isControlScript_ = isControl; }	// 設定

	// skinningを行うかを取得
	const bool GetIsSkinning() const { return isSkinning_; }

	const std::vector<std::unique_ptr<Skinning>>& GetSkinningArray() const { return skinning_; }

private:

	AnimationManager* manager_ = nullptr;

	std::unique_ptr<AOENGINE::AnimationClip> animationClip_ = nullptr;
	std::unique_ptr<Skeleton> skeleton_ = nullptr;
	std::vector<std::unique_ptr<Skinning>> skinning_;

	bool isSkinning_ = true;

	// -------------------------------------------------
	// ↓ Animationの遷移に関する変数
	// -------------------------------------------------
	bool isControlScript_ = false;

	float transitionTime_ = 0.0f;
};

}