#pragma once
#include <array>
#include <string>
#include <vector>

#include "Engine/Lib/Math/Vector2.h"
#include "Game/Tutorial/TutorialUIParameter.h"

namespace AOENGINE {
	class Sprite;
	class Text;
}

/// <summary>
/// チュートリアルの説明を出すテキストボックス
/// </summary>
class TutorialTextBoxUI {
public:

	// 出し入れの状態
	enum class State {
		Hidden,		// 出ていない
		Opening,	// 開いている最中
		Shown,		// 出ている
		Closing,	// 閉じている最中
	};

	TutorialTextBoxUI() = default;
	~TutorialTextBoxUI() = default;

public:
	/// <summary>
	/// 背景boxに書く内容
	/// </summary>
	struct Content {
		const std::string* body = nullptr;		// 説明の本文
		const std::string* nextText = nullptr;	// 次へ送る操作の案内
		const std::string* backText = nullptr;	// 前へ戻る操作の案内

		bool showNext = false;	// 次への案内を出すか
		bool showBack = false;	// 戻るの案内を出すか

		// チュートリアル内容が出来たかのチェック用のやつ
		std::array<bool, TutorialUIParameter::kCheckSlotCount> checkShown{};
		// チェックとボタンの置き場所を引くページ番号
		std::size_t pageIndex = 0;

		// コントローラーが繋がっているか。本文とボタンの中身がこれで変わる
		bool isPadConnected = false;

		// このページで教える操作のボタン画像名。並べた順に左から出す
		const std::vector<std::string>* buttons = nullptr;
		// 案内に添えるボタン画像名
		const std::string* nextButton = nullptr;
		const std::string* backButton = nullptr;
	};

public:
	// 初期化
	void Init();

	// 開く、閉じる
	void Open();
	void Close();

	// 更新
	void Update(float deltaTime, const Content& content);

	// パラメータ編集
	void Debug_Gui();
private:

	/// <summary>
	/// 条件を満たした時に出てくる部品。案内(枠+文字)もチェック(枠だけ)もこれで扱う
	/// </summary>
	struct AppearItem {
		AOENGINE::Sprite* sprite = nullptr;
		AOENGINE::Text* text = nullptr;	// 文字が無いものは nullptr のまま
		bool isShown = false;			// 出ている最中か
		float timer = 0.0f;				// 出現の経過時間
	};

	/// <summary>
	/// 出現を1回進めるのに要る指定。呼び出し側がパラメータから詰める
	/// </summary>
	struct AppearDesc {
		Math::Vector2 offset{};		// 本体の枠の中心からの位置
		Math::Vector2 size{};		// スプライトの大きさ
		float duration = 0.0f;		// 出現にかける時間
		int32_t easeKind = 0;		// 出現のイージング種類

		const std::string* text = nullptr;	// 文字。無ければ nullptr
		Math::Vector2 textOffset{};			// 枠からの文字の位置
		float fontSize = 0.0f;

		// 差し替える画像名。ページごとに変わるボタンで使う
		const std::string* texture = nullptr;
	};

	/// <summary>既にあれば引き当て、無ければ作る</summary>
	AOENGINE::Sprite* ResolveSprite(const std::string& name, const std::string& textureName, int renderQueue);
	AOENGINE::Text* ResolveText(const std::string& name, int renderQueue);

	// 案内(枠+文字)をセットアップする
	void SetupGuide(AppearItem& item, const std::string& boxName, const std::string& textName);

	// 絵だけのボタンをセットアップする
	void SetupButton(AppearItem& item, const std::string& name);

	// 拡大縮小のイージング更新
	void UpdateOpenClose(float deltaTime);

	/// <summary>
	/// 出現を進めて、位置と大きさを入れる。
	/// 案内もチェックもここを通るので、イージングの計算はこの1箇所だけ
	/// </summary>
	void UpdateAppear(AppearItem& item, float deltaTime, bool isVisible, const AppearDesc& desc);

	/// <summary>出現の状態を初期値へ戻す</summary>
	void ResetAppear(AppearItem& item) const;

	/// <summary>
	/// スプライトのサイズゼット
	/// </summary>
	/// <param name="item"></param>
	/// <param name="size"></param>
	void SetSpriteSize(AOENGINE::Sprite* item, const Math::Vector2& size) const;

	// 枠と本文を奥
	void PlaceBody();

	/// <summary>ページ・番号・接続状況に合う、ボタンの置き場所を返す</summary>
	const Math::Vector2& SelectButtonOffset(std::size_t pageIndex, std::size_t slot, bool isPadConnected) const;

	/// <summary>
	/// 画面上の位置と大きさを入れる
	/// </summary>
	void PlaceItem(AOENGINE::Sprite* item, const Math::Vector2& offset, float ownRate) const;

	// イージング計算
	float CalcRatio(float timer, float duration) const;
	float CalcAppearRate(float ratio, int32_t easeKind) const;

	/// <summary>まとめて表示/非表示を切り替える</summary>
	void SetItemsActive(bool isActive);

private:

	// 調整値
	TutorialUIParameter parameter_;

	// 生成した中身
	AOENGINE::Sprite* box_ = nullptr;
	AOENGINE::Text* body_ = nullptr;

	// チュートリアル内容ができたかのチェック
	std::array<AppearItem, TutorialUIParameter::kCheckSlotCount> checks_;

	// このページで教える操作のボタン画像
	std::array<AppearItem, TutorialUIParameter::kButtonSlotCount> buttons_;

	// 「次へ」と「戻る」の案内
	AppearItem next_;
	AppearItem back_;

	// 案内に添えるボタン画像
	AppearItem nextButton_;
	AppearItem backButton_;

	// 出し入れ
	State state_ = State::Hidden;
	float stateTimer_ = 0.0f;
	// 本体の拡大率。uiScale と開閉の進み具合を掛けたもの
	float boxRate_ = 0.0f;

	// シーン上での名前
	const std::string kBoxName = "TutorialTextBox";
	const std::string kBodyName = "Text_TutorialBody";
	const std::string kCheckName = "TutorialCheck";
	const std::string kNextBoxName = "TutorialNextBox";
	const std::string kNextTextName = "Text_TutorialNext";
	const std::string kBackBoxName = "TutorialBackBox";
	const std::string kBackTextName = "Text_TutorialBack";
	const std::string kButtonName = "TutorialButton";
	const std::string kNextButtonName = "TutorialNextButton";
	const std::string kBackButtonName = "TutorialBackButton";

	// チェックに使う画像名
	const std::string kCheckTextureName = "caution.png";

	// ボタン画像を作る時の仮の絵。中身は毎フレーム差し替える
	const std::string kButtonTextureName = "white.png";

	// 手前に出すための描画順
	const int kBoxRenderQueue = 100;
	const int kItemRenderQueue = 101;
	const int kTextRenderQueue = 102;

public: // accessor

	State GetState() const { return state_; }

	/// <summary>閉じきったか。シーンを移す合図に使う</summary>
	bool IsClosed() const { return state_ == State::Hidden; }
};
