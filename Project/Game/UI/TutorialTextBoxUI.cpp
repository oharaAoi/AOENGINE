#include "TutorialTextBoxUI.h"

#include <algorithm>
#include <string>

#include <Engine/Core/Engine.h>
#include <Engine/Lib/Color.h>
#include <Engine/Lib/Math/Easing.h>
#include <Engine/Module/Components/2d/Canvas2d.h>
#include <Engine/Module/Components/2d/Sprite.h>
#include <Engine/Module/Components/2d/Text.h>
#include <Engine/System/Manager/ImGuiManager.h>
#include <Engine/System/Manager/TextureManager.h>
#include <Engine/Utilities/SceneObjectFinder.h>

using namespace AOENGINE;

namespace {
	// アンカーはどれも中心。位置は中心座標で指定する
	const Math::Vector2 kCenterAnchor{ 0.5f, 0.5f };
	// 本体の枠の色
	const AOENGINE::Color kBoxColor{ 0.0f, 0.0f, 0.0f, 0.7f };
	// 案内の枠の色。文字は黒なので明るくする
	const AOENGINE::Color kGuideBoxColor{ 1.0f, 1.0f, 1.0f, 0.85f };
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::Init() {

	// 保存済みの調整値を読み込む
	parameter_.Load();

	// 背景boxとテキスト内容を作る
	box_ = ResolveSprite(kBoxName, "white.png", kBoxRenderQueue);
	body_ = ResolveText(kBodyName, kTextRenderQueue);

	// チェックは数だけ用意する
	for (std::size_t i = 0; i < checks_.size(); ++i) {

		// チェックスプライト作成、値初期化
		checks_[i].sprite = ResolveSprite(kCheckName + std::to_string(i), kCheckTextureName, kItemRenderQueue);
		ResetAppear(checks_[i]);

		// アンカーポイントセット
		if (checks_[i].sprite != nullptr) {
			checks_[i].sprite->SetAnchorPoint(kCenterAnchor);
		}
	}

	// 背景boxのパラメータセット
	if (box_ != nullptr) {
		box_->SetAnchorPoint(kCenterAnchor);
		box_->SetColor(kBoxColor);
	}

	// 本文テキストのパラメータセット
	if (body_ != nullptr) {
		body_->SetAnchorPoint(kCenterAnchor);
		body_->SetTextAnchorPoint(kCenterAnchor);
		body_->SetTextColor(Colors::Linear::white);
	}

	// 操作ボタンも数だけ用意する。絵はページごとに差し替える
	for (std::size_t i = 0; i < buttons_.size(); ++i) {

		buttons_[i].sprite = ResolveSprite(kButtonName + std::to_string(i), kButtonTextureName, kItemRenderQueue);
		ResetAppear(buttons_[i]);

		if (buttons_[i].sprite != nullptr) {
			buttons_[i].sprite->SetAnchorPoint(kCenterAnchor);
		}
	}

	// 「次へ」と「戻る」を同じ作りで用意する
	SetupGuide(next_, kNextBoxName, kNextTextName);
	SetupGuide(back_, kBackBoxName, kBackTextName);

	// 案内に添えるボタン。枠より手前に出す
	SetupButton(nextButton_, kNextButtonName);
	SetupButton(backButton_, kBackButtonName);

	state_ = State::Hidden;
	stateTimer_ = 0.0f;
	boxRate_ = 0.0f;

	// アクティブ化
	SetItemsActive(false);
}

void TutorialTextBoxUI::SetupGuide(AppearItem& item, const std::string& boxName, const std::string& textName) {

	// シーンに置かれていればそれを使い、無ければ作る。出現の状態も初期値へ戻す
	item.sprite = ResolveSprite(boxName, "white.png", kItemRenderQueue);
	item.text = ResolveText(textName, kTextRenderQueue);
	ResetAppear(item);

	// Boxパラメータセット
	if (item.sprite != nullptr) {
		item.sprite->SetAnchorPoint(kCenterAnchor);
		item.sprite->SetColor(kGuideBoxColor);
	}

	// 文字パラメータセット
	if (item.text != nullptr) {
		item.text->SetAnchorPoint(kCenterAnchor);
		item.text->SetTextAnchorPoint(kCenterAnchor);
		// 案内の枠は明るいので、文字は黒にする
		item.text->SetTextColor(Colors::Linear::black);
	}
}

void TutorialTextBoxUI::SetupButton(AppearItem& item, const std::string& name) {

	// 文字は持たない。絵はページごとに差し替えるので、ここでは仮のものを入れておく
	item.sprite = ResolveSprite(name, kButtonTextureName, kTextRenderQueue);
	item.text = nullptr;
	ResetAppear(item);

	if (item.sprite != nullptr) {
		item.sprite->SetAnchorPoint(kCenterAnchor);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 開閉の指示
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::Open() {

	// 既に出ている途中なら数え直さない
	if (state_ == State::Opening || state_ == State::Shown) {
		return;
	}

	// 初期状態セット
	state_ = State::Opening;
	stateTimer_ = 0.0f;

	// 案内もチェックも、また最初から出てくるように戻す
	ResetAppear(next_);
	ResetAppear(back_);
	ResetAppear(nextButton_);
	ResetAppear(backButton_);
	for (AppearItem& item : checks_) {
		ResetAppear(item);
	}
	for (AppearItem& item : buttons_) {
		ResetAppear(item);
	}

	// アクティブ化
	SetItemsActive(true);
}

void TutorialTextBoxUI::Close() {

	if (state_ == State::Closing || state_ == State::Hidden) {
		return;
	}

	state_ = State::Closing;
	stateTimer_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 枠と中身の用意
//////////////////////////////////////////////////////////////////////////////////////////////////

Sprite* TutorialTextBoxUI::ResolveSprite(const std::string& name, const std::string& textureName, int renderQueue) {

	// シーンに置かれていればそれを使う
	if (Sprite* found = FindSceneObject<Sprite>(name)) {
		return found;
	}

	Canvas2d* canvas = Engine::GetCanvas2d();
	if (canvas == nullptr) {
		return nullptr;
	}

	// 背景Boxスプライトを追加
	return canvas->AddSprite(textureName, name, renderQueue);
}

Text* TutorialTextBoxUI::ResolveText(const std::string& name, int renderQueue) {

	if (Text* found = FindSceneObject<Text>(name)) {
		return found;
	}

	Canvas2d* canvas = Engine::GetCanvas2d();
	if (canvas == nullptr) {
		return nullptr;
	}

	// テキストを追加
	return canvas->AddText(name, "", renderQueue);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 進み具合と出現の倍率
//////////////////////////////////////////////////////////////////////////////////////////////////

float TutorialTextBoxUI::CalcRatio(float timer, float duration) const {

	// 時間が0なら一瞬で終わらせる
	if (duration <= 0.0f) {
		return 1.0f;
	}

	return std::clamp(timer / duration, 0.0f, 1.0f);
}

float TutorialTextBoxUI::CalcAppearRate(float ratio, int32_t easeKind) const {

	// 出現し始めの大きさから 1.0 まで、イージングで繋ぐ
	const float easedT = Math::CallEasing(easeKind, ratio);
	return parameter_.appearScale + (1.0f - parameter_.appearScale) * easedT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 位置と大きさを入れる
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::SetSpriteSize(Sprite* item, const Math::Vector2& size) const {

	if (item == nullptr) {
		return;
	}

	// サイズのセット
	item->ReSetTextureSize(size);
	item->SetDrawRange(size);
}

void TutorialTextBoxUI::PlaceItem(Sprite* item, const Math::Vector2& offset, float ownRate) const {

	if (item == nullptr) {
		return;
	}

	// 枠が縮めば相対位置も一緒に縮む。中心へ吸い込まれるように見える
	item->SetTranslate(parameter_.boxCenter + offset * boxRate_);

	const float scale = boxRate_ * ownRate;
	item->SetScale(Math::Vector2(scale, scale));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 開閉
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::UpdateOpenClose(float deltaTime) {

	
	float rate = 1.0f;

	// 開ける
	if (state_ == State::Opening) {

		// 出現の大きさ -> 1.0へ
		stateTimer_ += deltaTime;
		const float ratio = CalcRatio(stateTimer_, parameter_.openTime);
		rate = CalcAppearRate(ratio, parameter_.openEaseKind);

		// 開ききったら、以降は等倍のまま止まる
		if (ratio >= 1.0f) {
			state_ = State::Shown;
		}
	}
	// 閉じる
	else if (state_ == State::Closing) {

		// 1.0 -> 出現の大きさへ
		stateTimer_ += deltaTime;
		const float ratio = CalcRatio(stateTimer_, parameter_.closeTime);
		rate = CalcAppearRate(1.0f - ratio, parameter_.closeEaseKind);

		// 閉じきったら中身をまとめて消す
		if (ratio >= 1.0f) {
			state_ = State::Hidden;
			SetItemsActive(false);
		}
	}
	// 出ていない間。次に開く時と同じ大きさにしておく
	else if (state_ == State::Hidden) {
		rate = parameter_.appearScale;
	}

	// ここで出した倍率が、本体も案内もチェックも全部に掛かる
	boxRate_ = parameter_.uiScale * rate;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 本体の枠と本文
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::PlaceBody() {

	if (box_ != nullptr) {
		SetSpriteSize(box_, parameter_.boxSize);
		PlaceItem(box_, Math::Vector2(0.0f, 0.0f), 1.0f);
	}

	if (body_ != nullptr) {
		body_->SetFontSize(parameter_.bodyFontSize);
		PlaceItem(body_, parameter_.bodyOffset, 1.0f);
	}
}

const Math::Vector2& TutorialTextBoxUI::SelectButtonOffset(
	std::size_t pageIndex, std::size_t slot, bool isPadConnected) const {

	// 同じ番号でも中身がキーボードとパッドで別物なので、置き場所も別々に持っている
	if (isPadConnected) {
		return parameter_.buttonPadOffset[pageIndex][slot];
	}

	return parameter_.buttonKeyboardOffset[pageIndex][slot];
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 出現
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::ResetAppear(AppearItem& item) const {

	item.isShown = false;
	item.timer = 0.0f;
}

void TutorialTextBoxUI::UpdateAppear(AppearItem& item, float deltaTime, bool isVisible, const AppearDesc& desc) {

	if (item.sprite == nullptr) {
		return;
	}

	// 出す条件が外れたら、次に出す時また最初から動くように戻す
	if (!isVisible) {
		ResetAppear(item);
		item.sprite->SetActive(false);
		if (item.text != nullptr) { item.text->SetActive(false); }
		return;
	}

	// 出た瞬間から数え始める
	if (!item.isShown) {
		item.isShown = true;
		item.timer = 0.0f;
		item.sprite->SetActive(true);
		if (item.text != nullptr) { item.text->SetActive(true); }
	}

	// 出現の倍率を出すのはここだけ。あとは枠にも文字にも同じ値を掛ける
	item.timer += deltaTime;
	const float ratio = CalcRatio(item.timer, desc.duration);
	const float rate = CalcAppearRate(ratio, desc.easeKind);

	// ページごとに絵が変わるものは、変わった時だけ差し替える。
	if (desc.texture != nullptr && !desc.texture->empty() &&
		item.sprite->GetTextureName() != *desc.texture) {

		// jsonに書いた絵をまだ用意していない場合がある
		if (TextureManager::GetInstance()->ExistTexture(*desc.texture)) {
			item.sprite->ReSetTexture(*desc.texture);
		}
	}

	// 枠を置く
	SetSpriteSize(item.sprite, desc.size);
	PlaceItem(item.sprite, desc.offset, rate);

	// 文字が付いていれば、枠の位置を基準にずらして置く
	if (item.text != nullptr) {

		// 同じ文字列を毎フレーム流し込むと、そのたびに文字のテクスチャを作り直すことになる
		if (desc.text != nullptr && item.text->GetText() != *desc.text) {
			item.text->SetText(*desc.text);
		}

		item.text->SetFontSize(desc.fontSize);
		PlaceItem(item.text, desc.offset + desc.textOffset, rate);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::Update(float deltaTime, const Content& content) {

	UpdateOpenClose(deltaTime);

	// 閉じきっていたら中身は触らない
	if (state_ == State::Hidden) {
		return;
	}

	// 同じ文字列を毎フレーム流し込むと、そのたびに文字のテクスチャを作り直すことになる
	if (body_ != nullptr && content.body != nullptr && body_->GetText() != *content.body) {
		body_->SetText(*content.body);
	}

	// ページごとに置き場所を変えられる
	std::size_t index = content.pageIndex;
	if (index >= TutorialUIParameter::kCheckCount) {
		index = 0;
	}

	PlaceBody();

	// 案内は2つとも同じ作りなので、違うのは置き場所と文言だけ
	AppearDesc guide{};
	guide.size = parameter_.guideBoxSize;
	guide.duration = parameter_.guideTime;
	guide.easeKind = parameter_.guideEaseKind;
	guide.textOffset = parameter_.guideOffset;
	guide.fontSize = parameter_.guideFontSize;

	// 条件を満たすと進むページでは次への案内を、最初のページでは戻るの案内を出さない
	guide.offset = parameter_.nextBoxOffset;
	guide.text = content.nextText;
	UpdateAppear(next_, deltaTime, content.showNext, guide);

	guide.offset = parameter_.backBoxOffset;
	guide.text = content.backText;
	UpdateAppear(back_, deltaTime, content.showBack, guide);

	// 案内に添えるボタン。枠と同じ条件で出す
	AppearDesc guideButton{};
	guideButton.size = parameter_.guideButtonSize;
	guideButton.duration = parameter_.guideTime;
	guideButton.easeKind = parameter_.guideEaseKind;

	guideButton.offset = parameter_.nextBoxOffset + parameter_.guideButtonOffset;
	guideButton.texture = content.nextButton;
	UpdateAppear(nextButton_, deltaTime, content.showNext, guideButton);

	guideButton.offset = parameter_.backBoxOffset + parameter_.guideButtonOffset;
	guideButton.texture = content.backButton;
	UpdateAppear(backButton_, deltaTime, content.showBack, guideButton);

	// このページで教える操作のボタンを、用意された数だけ置く。位置は1つずつ持っている
	std::size_t buttonCount = 0;
	if (content.buttons != nullptr) {
		buttonCount = content.buttons->size();
	}

	AppearDesc button{};
	button.size = parameter_.buttonSize;
	button.duration = parameter_.buttonTime;
	button.easeKind = parameter_.buttonEaseKind;

	for (std::size_t i = 0; i < buttons_.size(); ++i) {

		const bool isVisible = i < buttonCount;

		button.texture = nullptr;
		if (isVisible) {
			button.texture = &content.buttons->at(i);
		}

		button.offset = SelectButtonOffset(index, i, content.isPadConnected);
		UpdateAppear(buttons_[i], deltaTime, isVisible, button);
	}

	// チェックは文字を持たないので、枠だけの指定になる
	AppearDesc check{};
	check.size = parameter_.checkSize;
	check.duration = parameter_.checkTime;
	check.easeKind = parameter_.checkEaseKind;

	// 2つ目は移動のページのジャンプ用だけなので、置き場所も専用のものを使う
	check.offset = parameter_.checkOffset[index];
	UpdateAppear(checks_[0], deltaTime, content.checkShown[0], check);

	check.offset = parameter_.moveJumpCheckOffset;
	UpdateAppear(checks_[1], deltaTime, content.checkShown[1], check);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 表示の切り替え
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::SetItemsActive(bool isActive) {

	// box,本文のActive
	if (box_ != nullptr) { box_->SetActive(isActive); }
	if (body_ != nullptr) { body_->SetActive(isActive); }

	// 案内・チェック・操作ボタンは、出す条件を満たした時に出るので、ここでは一旦消す
	AppearItem* items[] = { &next_, &back_, &nextButton_, &backButton_ };
	for (AppearItem* item : items) {
		if (item->sprite != nullptr) { item->sprite->SetActive(false); }
		if (item->text != nullptr) { item->text->SetActive(false); }
	}

	for (AppearItem& item : checks_) {
		if (item.sprite != nullptr) { item.sprite->SetActive(false); }
	}

	for (AppearItem& item : buttons_) {
		if (item.sprite != nullptr) { item.sprite->SetActive(false); }
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// 調整値の編集
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialTextBoxUI::Debug_Gui() {

	parameter_.Debug_Gui();

	// イージングは名前で選べる形で出す
	ImGui::SeparatorText("Easing");
	Math::SelectEasing(parameter_.openEaseKind, "TutorialOpen");
	Math::SelectEasing(parameter_.closeEaseKind, "TutorialClose");
	Math::SelectEasing(parameter_.guideEaseKind, "TutorialGuide");
	Math::SelectEasing(parameter_.checkEaseKind, "TutorialCheck");
	Math::SelectEasing(parameter_.buttonEaseKind, "TutorialButton");

	parameter_.SaveAndLoad();
}
