#include "TutorialStepController.h"

#include "Engine/System/Manager/ImGuiManager.h"

#include "Game/Tutorial/Step/TutorialStepConnect.h"
#include "Game/Tutorial/Step/TutorialStepDamageFloor.h"
#include "Game/Tutorial/Step/TutorialStepEnding.h"
#include "Game/Tutorial/Step/TutorialStepLaunch.h"
#include "Game/Tutorial/Step/TutorialStepMove.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepController::Init(TutorialContext& context) {

	isAllFinished_ = false;
	currentKind_ = StepKind::Move;

	SetStep(context, CreateStep(currentKind_));
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepController::Update(TutorialContext& context, float deltaTime) {

	// 最後まで終わっていたら、シーン側が次へ移すまで何もしない
	if (isAllFinished_) {
		return;
	}

	if (!currentStep_) {
		SetStep(context, CreateStep(currentKind_));
		if (!currentStep_) {
			return;
		}
	}

	currentStep_->Update(context, deltaTime);

	// 入力で送れるページなら、次へ送る入力でも先へ進む
	const bool skipped = currentStep_->CanSkipByInput() && context.IsNextTriggered();
	if (!currentStep_->IsFinished() && !skipped) {
		return;
	}

	// 最後のページなら、そこで終わり
	const StepKind next = GetNextKind(currentKind_);
	if (next == currentKind_) {
		if (currentStep_) {
			currentStep_->Exit(context);
			currentStep_.reset();
		}
		isAllFinished_ = true;
		return;
	}

	ChangeStep(context, next);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ページの切り替え
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepController::ChangeStep(TutorialContext& context, StepKind kind) {

	currentKind_ = kind;
	isAllFinished_ = false;

	SetStep(context, CreateStep(kind));
}

void TutorialStepController::SetStep(TutorialContext& context, std::unique_ptr<BaseTutorialStep> next) {

	// 今のページを終わらせる
	if (currentStep_) {
		currentStep_->Exit(context);
	}

	currentStep_ = std::move(next);

	// 新しいページを始める
	if (currentStep_) {
		currentStep_->Enter(context);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  次のページ
///////////////////////////////////////////////////////////////////////////////////////////////

TutorialStepController::StepKind TutorialStepController::GetNextKind(StepKind kind) const {

	const std::size_t index = ToIndex(kind);

	// 最後のページなら自分自身を返す。呼び出し側が終わりだと判断する
	if (index + 1 >= kStepCount) {
		return kind;
	}

	return static_cast<StepKind>(index + 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ページの生成
///////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<BaseTutorialStep> TutorialStepController::CreateStep(StepKind kind) const {

	// ページを増やしたらここにも追加する
	if (kind == StepKind::DamageFloor) {
		return std::make_unique<TutorialStepDamageFloor>();
	}

	if (kind == StepKind::Connect) {
		return std::make_unique<TutorialStepConnect>();
	}

	if (kind == StepKind::Launch) {
		return std::make_unique<TutorialStepLaunch>();
	}

	if (kind == StepKind::Ending) {
		return std::make_unique<TutorialStepEnding>();
	}

	return std::make_unique<TutorialStepMove>();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  今のページの情報
///////////////////////////////////////////////////////////////////////////////////////////////

const std::string& TutorialStepController::GetCurrentName() const {

	if (!currentStep_) {
		return kNoneName_;
	}

	return currentStep_->GetName();
}

bool TutorialStepController::CanSkipByInput() const {

	if (!currentStep_) {
		return false;
	}

	return currentStep_->CanSkipByInput();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  デバッグ表示
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialStepController::Debug_Gui(TutorialContext& context) {

	ImGui::TextUnformatted("step:");
	ImGui::SameLine();
	ImGui::TextUnformatted(GetCurrentName().c_str());

	const char* finishedState = "false";
	if (isAllFinished_) {
		finishedState = "true";
	}
	ImGui::Text("all finished: %s", finishedState);

	// ページを単体で確認するために、ここから飛べるようにする
	if (ImGui::Button("Move")) {
		ChangeStep(context, StepKind::Move);
	}
	ImGui::SameLine();
	if (ImGui::Button("DamageFloor")) {
		ChangeStep(context, StepKind::DamageFloor);
	}
	ImGui::SameLine();
	if (ImGui::Button("Connect")) {
		ChangeStep(context, StepKind::Connect);
	}
	ImGui::SameLine();
	if (ImGui::Button("Launch")) {
		ChangeStep(context, StepKind::Launch);
	}
	ImGui::SameLine();
	if (ImGui::Button("Ending")) {
		ChangeStep(context, StepKind::Ending);
	}
}
