#pragma once

class Player;
class Boss;
class StageBlockField;
class FollowCamera;

/// <summary>
/// チュートリアルの各ページが触りたいものをまとめて渡す入れ物
/// </summary>
class TutorialContext {
public:

	TutorialContext() = default;
	~TutorialContext() = default;

private:

	Player* pPlayer_ = nullptr;
	Boss* pBoss_ = nullptr;
	StageBlockField* pBlockField_ = nullptr;
	FollowCamera* pCamera_ = nullptr;

	// 次のページへ送る入力が今フレーム押されたか
	bool isNextTriggered_ = false;

	// 前のページへ戻る入力が今フレーム押されたか
	bool isBackTriggered_ = false;

	// コントローラーが繋がっているか。説明文をキーボード用へ切り替えるのに使う
	bool isPadConnected_ = false;

public: // accessor

	Player* GetPlayer() const { return pPlayer_; }
	Boss* GetBoss() const { return pBoss_; }
	StageBlockField* GetBlockField() const { return pBlockField_; }
	FollowCamera* GetCamera() const { return pCamera_; }

	void SetPlayer(Player* player) { pPlayer_ = player; }
	void SetBoss(Boss* boss) { pBoss_ = boss; }
	void SetBlockField(StageBlockField* field) { pBlockField_ = field; }
	void SetCamera(FollowCamera* camera) { pCamera_ = camera; }

	bool IsNextTriggered() const { return isNextTriggered_; }
	void SetNextTriggered(bool isTriggered) { isNextTriggered_ = isTriggered; }

	bool IsBackTriggered() const { return isBackTriggered_; }
	void SetBackTriggered(bool isTriggered) { isBackTriggered_ = isTriggered; }

	bool IsPadConnected() const { return isPadConnected_; }
	void SetPadConnected(bool isConnected) { isPadConnected_ = isConnected; }
};
