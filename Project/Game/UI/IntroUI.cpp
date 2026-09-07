#include "IntroUI.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include <Engine/Core/Engine.h>
#include <Engine/Lib/Math/Easing.h>
#include <Engine/Lib/Math/MyMath.h>
#include <Engine/Module/Components/2d/Canvas2d.h>
#include <Engine/Module/Components/2d/Sprite.h>
#include <Engine/Module/Components/2d/Text.h>
#include <Engine/System/Manager/ImGuiManager.h>
#include <Engine/Utilities/SceneObjectFinder.h>

using namespace AOENGINE;

namespace {
	// アンカーはどれも中心。位置は中心座標で指定する
	const Math::Vector2 kCenterAnchor{ 0.5f, 0.5f };
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 状態ごとの進め方
//////////////////////////////////////////////////////////////////////////////////////////////////

IntroUI::IntroUI() {

	// 文字ごとの進め方。中身はそれぞれの関数に置いてある
	stateUpdaters_[ToIndex(ItemState::MoveIn)] = [this](Item& item, bool isGo) { UpdateMoveIn(item, isGo); };
	stateUpdaters_[ToIndex(ItemState::MoveOut)] = [this](Item& item, bool) { UpdateMoveOut(item); };
	stateUpdaters_[ToIndex(ItemState::Hold)] = [this](Item& item, bool) { UpdateHold(item); };
	stateUpdaters_[ToIndex(ItemState::Fade)] = [this](Item& item, bool) { UpdateFade(item); };

	// 演出全体の流れ
	phaseUpdaters_[ToIndex(Phase::BossDescend)] = [this](float) { UpdateBossDescend(); };
	phaseUpdaters_[ToIndex(Phase::ObjectiveWait)] = [this](float) { UpdateObjectiveWait(); };
	phaseUpdaters_[ToIndex(Phase::ObjectiveIn)] = [this](float) { UpdateObjectiveIn(); };
	phaseUpdaters_[ToIndex(Phase::ObjectiveHold)] = [this](float) { UpdateObjectiveHold(); };
	phaseUpdaters_[ToIndex(Phase::ObjectiveOut)] = [this](float) { UpdateObjectiveOut(); };
	phaseUpdaters_[ToIndex(Phase::Countdown)] = [this](float deltaTime) { UpdateCountdown(deltaTime); };
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 文字ごとの進め方
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::UpdateMoveIn(Item& item, bool isGo) {

	// easeTの計算、ratioを使ってイージング呼び出し
	const float ratio = CalcRatio(item.timer, parameter_.inTime);
	const float eased = Math::CallEasing(parameter_.inEaseKind, ratio);

	// 位置はイージングで寄せる
	PlaceItem(item, Math::Vector2::Lerp(CalcRightPos(), CalcCenterPos(), eased), ratio, 1.0f);

	if (ratio < 1.0f) {
		return;
	}

	// 中心へ着いたので、次の文字を出す間合いを数え始める
	isWaitingNext_ = true;
	nextTimer_ = 0.0f;

	// Goだけは左へ抜けず、中心で止まってから消える
	ItemState next = ItemState::MoveOut;
	if (isGo) {
		next = ItemState::Hold;
	}

	// 切り替え
	ChangeState(item, next);
}

void IntroUI::UpdateMoveOut(Item& item) {

	// 入りとは別のイージングで抜けていく
	const float ratio = CalcRatio(item.timer, parameter_.outTime);
	const float eased = Math::CallEasing(parameter_.outEaseKind, ratio);

	// 形は入りの終わりの見た目のまま流す
	PlaceItem(item, Math::Vector2::Lerp(CalcCenterPos(), CalcLeftPos(), eased), 1.0f, 1.0f);

	// イージング終わったんなら終わりです
	if (ratio >= 1.0f) {
		FinishItem(item);
	}
}

void IntroUI::UpdateHold(Item& item) {

	// Goだけが通る。中心で見せたまま止める
	PlaceItem(item, CalcCenterPos(), 1.0f, 1.0f);

	if (item.timer >= parameter_.goHoldTime) {
		ChangeState(item, ItemState::Fade);
	}
}

void IntroUI::UpdateFade(Item& item) {

	// 左へは行かず、中心に置いたままアルファだけ落とす
	const float ratio = CalcRatio(item.timer, parameter_.goFadeTime);
	const float eased = Math::CallEasing(parameter_.goFadeEaseKind, ratio);

	PlaceItem(item, CalcCenterPos(), 1.0f, 1.0f - eased);

	if (ratio >= 1.0f) {
		FinishItem(item);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 演出全体の流れ
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::UpdateBossDescend() {

	// 降りている間は暗転だけ見せる。終わったかはシーン側から渡してもらう
	if (!isBossDescendFinished_) {
		return;
	}

	ChangePhase(Phase::ObjectiveWait);
}

void IntroUI::UpdateObjectiveWait() {

	// 待っている間は暗転だけ見せる
	if (phaseTimer_ < parameter_.objectiveStartWaitTime) {
		return;
	}

	// 出す前に始まりの見た目を入れておく
	PlaceObjective(0.0f);
	if (objective_ != nullptr) {
		objective_->SetActive(true);
	}

	ChangePhase(Phase::ObjectiveIn);
}

void IntroUI::UpdateObjectiveIn() {

	// 位置は動かさず、大きさと回転だけで見せる
	const float ratio = CalcRatio(phaseTimer_, parameter_.objectiveInTime);
	PlaceObjective(Math::CallEasing(parameter_.objectiveInEaseKind, ratio));

	if (ratio >= 1.0f) {
		ChangePhase(Phase::ObjectiveHold);
	}
}

void IntroUI::UpdateObjectiveHold() {

	// 出したまま見せる
	PlaceObjective(1.0f);

	if (phaseTimer_ >= parameter_.objectiveHoldTime) {
		ChangePhase(Phase::ObjectiveOut);
	}
}

void IntroUI::UpdateObjectiveOut() {

	// 出る時とは別のイージングで、始まりの見た目へ帰す
	const float ratio = CalcRatio(phaseTimer_, parameter_.objectiveOutTime);
	PlaceObjective(1.0f - Math::CallEasing(parameter_.objectiveOutEaseKind, ratio));

	if (ratio < 1.0f) {
		return;
	}

	// 戻しきったら消して、カウントダウンへ移る
	if (objective_ != nullptr) {
		objective_->SetActive(false);
	}

	ChangePhase(Phase::Countdown);
	StartItem(0);
}

void IntroUI::UpdateCountdown(float deltaTime) {

	UpdateNextStart(deltaTime);

	// 最後の要素が Go
	const std::size_t goIndex = items_.size() - 1;
	for (std::size_t i = 0; i < items_.size(); ++i) {
		UpdateItem(items_[i], i == goIndex, deltaTime);
	}

	// 全部終わってから、少しだけ余韻を置く
	for (const Item& item : items_) {
		if (item.state != ItemState::Done) {
			return;
		}
	}

	endTimer_ += deltaTime;
	if (endTimer_ >= parameter_.goEndWaitTime) {
		ChangePhase(Phase::Done);
		isPlaying_ = false;
		SetItemsActive(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 状態の切り替え
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::ChangeState(Item& item, ItemState next) const {
	item.state = next;
	item.timer = 0.0f;
}

void IntroUI::ResetItem(Item& item) const {
	item.state = ItemState::Waiting;
	item.timer = 0.0f;
	item.lifeTime = 0.0f;
}

void IntroUI::ChangePhase(Phase next) {
	phase_ = next;
	phaseTimer_ = 0.0f;
}

void IntroUI::FinishItem(Item& item) const {
	item.state = ItemState::Done;
	if (item.text != nullptr) {
		item.text->SetActive(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 画面上の置き場所
//////////////////////////////////////////////////////////////////////////////////////////////////

Math::Vector2 IntroUI::CalcCenterPos() const {
	return parameter_.centerPos;
}

Math::Vector2 IntroUI::CalcRightPos() const {
	return parameter_.centerPos + parameter_.rightOffset;
}

Math::Vector2 IntroUI::CalcLeftPos() const {
	return parameter_.centerPos + parameter_.leftOffset;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::Init() {

	// 保存済みの調整値を読み込む
	parameter_.Load();

	// 背景を暗くするスプライト
	back_ = ResolveSprite(kBackName, "white.png", kBackRenderQueue);
	if (back_ != nullptr) {
		back_->SetAnchorPoint(kCenterAnchor);
	}

	// 最初に出す目的の絵
	objective_ = ResolveText(kObjectiveName, kTextRenderQueue);
	if (objective_ != nullptr) {
		objective_->SetFontPath(parameter_.fontPath);
		objective_->SetAnchorPoint(kCenterAnchor);
		objective_->SetTextAnchorPoint(kCenterAnchor);
	}

	// 文字は数だけ用意する。中身は変わらないのでここで入れておく
	for (std::size_t i = 0; i < items_.size(); ++i) {

		items_[i].text = ResolveText(kItemName + std::to_string(i), kTextRenderQueue);
		ResetItem(items_[i]);

		if (items_[i].text == nullptr) {
			continue;
		}

		items_[i].text->SetFontPath(parameter_.fontPath);
		items_[i].text->SetAnchorPoint(kCenterAnchor);
		items_[i].text->SetTextAnchorPoint(kCenterAnchor);
		items_[i].text->SetText(kItemTexts[i]);
	}

	startedCount_ = 0;
	isWaitingNext_ = false;
	nextTimer_ = 0.0f;
	isPlaying_ = false;
	endTimer_ = 0.0f;
	ChangePhase(Phase::Done);

	SetItemsActive(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 再生
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::Start() {

	for (Item& item : items_) {
		ResetItem(item);
	}

	startedCount_ = 0;
	isWaitingNext_ = false;
	nextTimer_ = 0.0f;
	endTimer_ = 0.0f;
	isPlaying_ = true;

	// 暗転だけ先に出す。目的と文字は順番が来たものから出す
	SetItemsActive(false);
	if (back_ != nullptr) {
		back_->SetActive(true);
	}

	PlaceBack();

	// ボスが降りきるのを待つところから始める
	isBossDescendFinished_ = false;
	ChangePhase(Phase::BossDescend);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 中身の用意
//////////////////////////////////////////////////////////////////////////////////////////////////

Sprite* IntroUI::ResolveSprite(const std::string& name, const std::string& textureName, int renderQueue) {

	// シーンに置かれていればそれを使う
	if (Sprite* found = FindSceneObject<Sprite>(name)) {
		return found;
	}

	Canvas2d* canvas = Engine::GetCanvas2d();
	if (canvas == nullptr) {
		return nullptr;
	}

	return canvas->AddSprite(textureName, name, renderQueue);
}

Text* IntroUI::ResolveText(const std::string& name, int renderQueue) {

	if (Text* found = FindSceneObject<Text>(name)) {
		return found;
	}

	Canvas2d* canvas = Engine::GetCanvas2d();
	if (canvas == nullptr) {
		return nullptr;
	}

	return canvas->AddText(name, "", renderQueue);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 進み具合
//////////////////////////////////////////////////////////////////////////////////////////////////

float IntroUI::CalcRatio(float timer, float duration) const {

	// 時間が0なら一瞬で終わらせる
	if (duration <= 0.0f) {
		return 1.0f;
	}

	return std::clamp(timer / duration, 0.0f, 1.0f);
}

float IntroUI::RepeatRatio(float ratio, int32_t loopCount) const {

	if (loopCount <= 1) {
		return ratio;
	}

	//終わりの値
	if (ratio >= 1.0f) {
		return 1.0f;
	}

	const float scaled = ratio * static_cast<float>(loopCount);
	return scaled - std::floor(scaled);
}

float IntroUI::CalcSwingDegree(float ratio) const {

	// 左右の真ん中と、そこからの振れ幅
	const float center = (parameter_.rotateStart + parameter_.rotateEnd) * 0.5f;
	const float amplitude = (parameter_.rotateEnd - parameter_.rotateStart) * 0.5f;

	// 進むほど振れ幅を細くして、最後は真ん中で止める
	const float damping = 1.0f - std::clamp(ratio, 0.0f, 1.0f);

	// cosなので出だしは端から始まり、そこから左右へ往復する
	const float wave = std::cos(
		ratio * static_cast<float>(parameter_.rotateLoopCount) * 2.0f * std::numbers::pi_v<float>);

	return center + amplitude * wave * damping;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 置く
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::PlaceBack() const {

	if (back_ == nullptr) {
		return;
	}

	// 大きさは2つ揃えて入れないと絵が並んでしまう
	back_->ReSetTextureSize(parameter_.backSize);
	back_->SetDrawRange(parameter_.backSize);
	back_->SetTranslate(parameter_.backCenter);
	back_->SetColor(parameter_.backColor);
}

void IntroUI::PlaceObjective(float ratio) const {

	if (objective_ == nullptr) {
		return;
	}

	// 同じ文字列を毎フレーム流し込むと、そのたびに文字のテクスチャを作り直すことになる
	if (objective_->GetText() != parameter_.objectiveText) {
		objective_->SetText(parameter_.objectiveText);
	}

	objective_->SetFontSize(parameter_.objectiveFontSize);
	objective_->SetTextColor(parameter_.textColor);
	objective_->SetTranslate(parameter_.objectivePos);

	const float scale = parameter_.objectiveScaleStart +
		(parameter_.objectiveScaleEnd - parameter_.objectiveScaleStart) * ratio;
	objective_->SetScale(Math::Vector2(scale, scale));

	const float degree = parameter_.objectiveRotateStart +
		(parameter_.objectiveRotateEnd - parameter_.objectiveRotateStart) * ratio;
	objective_->SetRotate(degree * kToRadian);
}

void IntroUI::PlaceItem(const Item& item, const Math::Vector2& position, float shapeRatio, float alpha) const {

	if (item.text == nullptr) {
		return;
	}

	// フォントサイズと位置をセット
	item.text->SetFontSize(parameter_.fontSize);
	item.text->SetTranslate(position);

	// 回転は左右に振る。移動とは別の時間で動かすので、出てきてからの時間で見る
	const float swingRatio = CalcRatio(item.lifeTime, parameter_.rotateSwingTime);
	item.text->SetRotate(CalcSwingDegree(swingRatio) * kToRadian);

	// 大きさは指定した回数だけ始まり->終わりを繰り返す
	const float scaleRatio = RepeatRatio(shapeRatio, parameter_.scaleLoopCount);
	const float scale = parameter_.scaleStart + (parameter_.scaleEnd - parameter_.scaleStart) * scaleRatio;
	item.text->SetScale(Math::Vector2(scale, scale));

	// カラー
	Color color = parameter_.textColor;
	color.a *= alpha;
	item.text->SetTextColor(color);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 1文字ぶんの進行
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::StartItem(std::size_t index) {

	if (index >= items_.size()) {
		return;
	}

	Item& item = items_[index];
	ResetItem(item);
	ChangeState(item, ItemState::MoveIn);

	if (item.text != nullptr) {
		item.text->SetActive(true);
	}

	startedCount_ = index + 1;
}

void IntroUI::UpdateItem(Item& item, bool isGo, float deltaTime) {

	// 出ていないものと終わったものは時間も進めない
	if (item.state == ItemState::Waiting || item.state == ItemState::Done) {
		return;
	}

	item.timer += deltaTime;
	// 振りは状態をまたいで進めたいので、こちらは数え続ける
	item.lifeTime += deltaTime;

	// 中身は状態ごとの関数に任せる
	const ItemUpdater& updater = stateUpdaters_[ToIndex(item.state)];
	if (updater) {
		updater(item, isGo);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 次の文字を出す間合い
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::UpdateNextStart(float deltaTime) {

	if (!isWaitingNext_) {
		return;
	}

	// 出す文字が残っていなければ数えるものが無い
	if (startedCount_ >= items_.size()) {
		isWaitingNext_ = false;
		return;
	}

	nextTimer_ += deltaTime;
	if (nextTimer_ < parameter_.nextOffsetTime) {
		return;
	}

	StartItem(startedCount_);
	isWaitingNext_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::Update(float deltaTime) {

	if (!isPlaying_) {
		return;
	}

	PlaceBack();

	// 押しっぱなしの間は時間を速く進める。Goの直前で元の速さへ戻る
	deltaTime *= GetTimeScale();

	phaseTimer_ += deltaTime;

	// 中身は流れごとの関数に任せる
	const PhaseUpdater& updater = phaseUpdaters_[ToIndex(phase_)];
	if (updater) {
		updater(deltaTime);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 早送り
//////////////////////////////////////////////////////////////////////////////////////////////////

bool IntroUI::CanFastForward() const {

	// Goが動き出したら、そこから先は見せ切りたいので早送りしない
	return startedCount_ < items_.size();
}

float IntroUI::GetTimeScale() const {

	if (!isPlaying_ || !isFastForwardHeld_ || !CanFastForward()) {
		return 1.0f;
	}

	// 遅くする方には使わせない
	return (std::max)(parameter_.fastForwardScale, 1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 表示の切り替え
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::SetItemsActive(bool isActive) {

	if (back_ != nullptr) { back_->SetActive(isActive); }
	if (objective_ != nullptr) { objective_->SetActive(isActive); }

	for (Item& item : items_) {
		if (item.text != nullptr) { item.text->SetActive(isActive); }
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 調整値の編集
//////////////////////////////////////////////////////////////////////////////////////////////////

void IntroUI::Debug_Gui() {

	parameter_.Debug_Gui();

	// イージングは番号を覚えなくていいように、名前で選べる形で出す
	ImGui::SeparatorText("Easing");
	Math::SelectEasing(parameter_.objectiveInEaseKind, "IntroObjectiveIn");
	Math::SelectEasing(parameter_.objectiveOutEaseKind, "IntroObjectiveOut");
	Math::SelectEasing(parameter_.inEaseKind, "IntroIn");
	Math::SelectEasing(parameter_.outEaseKind, "IntroOut");
	Math::SelectEasing(parameter_.goFadeEaseKind, "IntroGoFade");

	if (ImGui::Button("Replay")) {
		Start();
	}

	parameter_.SaveAndLoad();
}
