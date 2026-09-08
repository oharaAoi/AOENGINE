#pragma once

/// stl
#include <cstdint>
#include <functional>
#include <string>

/// engine
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"

/// <summary>
/// ポップアップ表示の見た目の調整値。
/// ComboTextUIParameter / DamageTextUIParameter がそれぞれ1つだけ持ち、
/// 表示側はその実体を参照し続ける(実行中に編集した値がその場で効く)
/// </summary>
struct PopupTextParams{
	float visibleSize = 0.3f;		// 出ている時の文字の大きさ(WorldTextComponentのheight)
	float fadeInTime = 0.8f;		// 出きるまでの時間
	float fadeOutTime = 0.8f;		// 消えきるまでの時間
	float firstVelocity = 6.0f;		// 跳ね上がる初速
	float gravityRate = 2.0f;		// 重力の倍率。上げるほど低く速く跳ねる
	float damping = 0.83f;			// 跳ね返った時に残る速さの割合
	int32_t targetBounceCount = 1;	// この回数だけ跳ねたら止まる
};

/// <summary>
/// ワールド上に一度だけ出して、跳ねながら出て消える文字表示の土台。
/// 「どのPrefabで何と出すか」は派生クラスが決め、このクラスは出る・跳ねる・消えるだけを受け持つ。
/// 出す・消す・捨てるの管理は、それぞれのUIのManagerが行う
/// </summary>
class PopupTextUI 
	: public AOENGINE::BaseEntity{
public:
	PopupTextUI() = default;
	~PopupTextUI() override = default;

	void Update();

	/// <summary>
	/// 表示する文字を差し替える。出ている途中でも入れ替えられる
	/// </summary>
	void SetText(const std::string& text);

	/// <summary>
	/// その場でもう一度跳ねさせる。大きさはそのまま
	/// </summary>
	void Bounce();

	/// <summary>
	/// 出るところからやり直す。小さい状態から出て、もう一度跳ねる
	/// </summary>
	void Replay();

	/// <summary>
	/// 消え始める。既に消え始めている場合は何もしない
	/// </summary>
	void StartFadeOut();

	/// <summary>消え終わったか。trueになったら管理側が破棄する</summary>
	bool IsFinished() const{ return isFinished_; }

protected:

	/// <summary>
	/// Prefabから実体を作って表示を始める。派生クラスの Spawn() から呼ぶ
	/// </summary>
	/// <param name="prefabName">生成に使うPrefab名</param>
	/// <param name="text">表示する文字</param>
	/// <param name="position">表示する位置</param>
	/// <param name="params">調整値。管理側が持っているものを参照し続ける</param>
	/// <returns>生成できた場合はtrue</returns>
	bool SpawnText(const std::string& prefabName,const std::string& text,
				   const Math::Vector3& position,const PopupTextParams& params);

private:
	void FadeIn();
	void FadeOut();
	void UpdateBounce();

private:
	// 調整値(非所有)。実行中の編集をそのまま反映させるため、値のコピーは持たない
	const PopupTextParams* pParams_ = nullptr;

	std::function<void()> fadeCallback_ = nullptr;

	float timer_ = 0.f;
	float textSize_ = 0.f;

	int bounceCount_ = 0;

	Math::Vector3 basePosition_{};	// 生成時に渡された位置。X/Zはここから動かさない
	float currentY_ = 0.f;
	float baseY_ = 0.f;

	float velocity_ = 0.f;

	bool isFadingOut_ = false;	// StartFadeOut() が呼ばれたか
	bool isFinished_ = false;	// 消え終わったか
};
