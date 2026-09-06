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
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  次へ進めるか / 進める
///////////////////////////////////////////////////////////////////////////////////////////////

bool TutorialStepController::WantsAdvance(const TutorialContext& context) const {

	if (isAllFinished_ || !currentStep_) {
		return false;
	}

	// ページ自身が終わったか、入力で送れるページで送る入力が来たか
	if (currentStep_->IsFinished()) {
		return true;
	}

	return currentStep_->CanSkipByInput() && context.IsNextTriggered();
}

void TutorialStepController::Advance(TutorialContext& context) {

	if (isAllFinished_) {
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

bool TutorialStepController::WantsBack(const TutorialContext& context) const {

	if (isAllFinished_ || !currentStep_) {
		return false;
	}

	// 最初のページからは戻れない
	if (!CanBack()) {
		return false;
	}

	return context.IsBackTriggered();
}

void TutorialStepController::Back(TutorialContext& context) {

	const StepKind prev = GetPrevKind(currentKind_);
	if (prev == currentKind_) {
		return;
	}

	ChangeStep(context, prev);
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

TutorialStepController::StepKind TutorialStepController::GetPrevKind(StepKind kind) const {

	const std::size_t index = ToIndex(kind);

	// 最初のページなら自分自身を返す
	if (index == 0) {
		return kind;
	}

	return static_cast<StepKind>(index - 1);
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

const std::string& TutorialStepController::GetCurrentTextKey() const {

	if (!currentStep_) {
		return kNoneName_;
	}

	return currentStep_->GetTextKey();
}

std::size_t TutorialStepController::GetCurrentCheckCount() const {

	if (!currentStep_) {
		return 0;
	}

	return currentStep_->GetCheckCount();
}

bool TutorialStepController::IsCurrentCleared(std::size_t index) const {

	if (!currentStep_) {
		return false;
	}

	return currentStep_->IsCleared(index);
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
