#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"

class Player;
class FollowCamera;

class TestScene
: public BaseScene {
public:

	TestScene();
	~TestScene() override;

	void Finalize() override;
	void Init() override;
	// シーンJSON読み込み後に一度だけ呼ばれる。ここで配置済みオブジェクトを取得する
	void OnPlayStart() override;
	void Update() override;
	void PostUpdate() override;
	void Draw() const override;

private:

	// シーンから "Player" を取得して Player エンティティに束ねる
	void TryAcquirePlayer();

private:

	std::unique_ptr<Player> player_;
	std::unique_ptr<FollowCamera> followCamera_;

	bool playerResolved_ = false;	// "Player" の取得が完了したか
};
