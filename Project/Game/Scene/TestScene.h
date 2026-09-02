#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"

/// game
#include "Game/Stage/StageBlockField.h"
#include "Game/Stage/StageSegment.h"

class TestScene
: public BaseScene {
public:


public:

	TestScene();
	~TestScene() override;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void OnPlayStart() override;

private:

	/// セグメントを跨いでブロックの連結グループを管理する表
	StageBlockField stageBlockField_;
	/// CSVから読み込んだステージセグメント
	StageSegment stageSegment_;

};

