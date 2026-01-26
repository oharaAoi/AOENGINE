#pragma once
#include <string>
#include <memory>
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/ScreenTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Render;

/// <summary>
/// spriteのgaugeAnimation
/// </summary>
enum class FillStartingPoint {
	Top,
	Bottom,
	Left,
	Right,
	TopBottom,
	LeftRight,
};

/// <summary>
/// どの方向にSpriteを満たすか
/// </summary>
enum class FillMethod {
	Vertical,		// 垂直
	Horizontal,		// 水平
	Radial,			// 円形
	BothEnds		// 両端
};

namespace AOENGINE {

/// <summary>
/// 円ゲージに必要な構造体
/// </summary>
struct ArcGaugeParam {
	Math::Vector2 center;		// 中心座標
	float fillAmount;	// 塗りつぶし量
	float innerRadius;	// 内半径
	float outerRadius;	// 外半径
	float startAngle;	// 開始角度
	float arcRange;		// 弧の最大角度
	int clockwise;		// 回転方向
};

/// <summary>
/// Spriteのパラメータ
/// </summary>
struct SpriteParameter : public AOENGINE::IJsonConverter {
	Math::SRT transform = Math::SRT();
	Math::SRT uvTransform = Math::SRT();
	std::string textureName = "white.png";
	AOENGINE::Color color = Colors::Linear::white;
	Math::Vector2 drawRange = Math::Vector2(64.0f, 64.0f);
	Math::Vector2 leftTop = CMath::Vector2::ZERO;
	Math::Vector2 anchorPoint = Math::Vector2(0.5f, 0.5f);
	bool isFlipX = false;
	bool isFlipY = false;
	Math::Vector2 spriteSize = Math::Vector2(64.0f, 64.0f);

	int fillMethod = static_cast<int>(FillMethod::Vertical);
	int fillStartingPoint = static_cast<int>(FillStartingPoint::Left);

	Math::Vector2 center = Math::Vector2(0.5f, 0.5f);	// 中心座標
	float fillAmount = 1.0f;				// 塗りつぶし量
	float innerRadius = 0.0f;				// 内半径
	float outerRadius = 1.0f;				// 外半径
	float startAngle = 0.0f;				// 開始角度
	float arcRange = kPI;					// 弧の最大角度
	int clockwise = 0;						// 回転方向

	uint32_t windowWidth = 1280;			// 画面の横幅
	uint32_t windowHeight = 720;			// 画面の縦幅

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("transform", transform)
			.Add("uvTransform", uvTransform)
			.Add("textureName", textureName)
			.Add("color", color)
			.Add("drawRange", drawRange)
			.Add("leftTop", leftTop)
			.Add("anchorPoint", anchorPoint)
			.Add("isFlipX", isFlipX)
			.Add("isFlipY", isFlipY)
			.Add("spriteSize", spriteSize)
			.Add("fillMethod", fillMethod)
			.Add("fillStartingPoint", fillStartingPoint)
			.Add("center", center)
			.Add("fillAmount", fillAmount)
			.Add("innerRadius", innerRadius)
			.Add("outerRadius", outerRadius)
			.Add("startAngle", startAngle)
			.Add("arcRange", arcRange)
			.Add("clockwise", clockwise)
			.Add("windowWidth", windowWidth)
			.Add("windowHeight", windowHeight)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "transform", transform);
		Convert::fromJson(jsonData, "uvTransform", uvTransform);
		Convert::fromJson(jsonData, "textureName", textureName);
		Convert::fromJson(jsonData, "color", color);
		Convert::fromJson(jsonData, "drawRange", drawRange);
		Convert::fromJson(jsonData, "anchorPoint", anchorPoint);
		Convert::fromJson(jsonData, "isFlipX", isFlipX);
		Convert::fromJson(jsonData, "isFlipY", isFlipY);
		Convert::fromJson(jsonData, "spriteSize", spriteSize);
		Convert::fromJson(jsonData, "fillMethod", fillMethod);
		Convert::fromJson(jsonData, "fillStartingPoint", fillStartingPoint);
		Convert::fromJson(jsonData, "center", center);
		Convert::fromJson(jsonData, "fillAmount", fillAmount);
		Convert::fromJson(jsonData, "innerRadius", innerRadius);
		Convert::fromJson(jsonData, "outerRadius", outerRadius);
		Convert::fromJson(jsonData, "startAngle", startAngle);
		Convert::fromJson(jsonData, "arcRange", arcRange);
		Convert::fromJson(jsonData, "clockwise", clockwise);
		Convert::fromJson(jsonData, "windowWidth", windowWidth);
		Convert::fromJson(jsonData, "windowHeight", windowHeight);
	}

	void Debug_Gui() override {};
};


/// <summary>
/// 2d描画
/// </summary>
class Sprite :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// Mesh構造体
	/// </summary>
	struct TextureMesh {
		Math::Vector4 pos;
		Math::Vector2 texcoord;
		float padding[2];
	};

	/// <summary>
	/// Material構造体
	/// </summary>
	struct TextureMaterial {
		AOENGINE::Color color;
		Math::Matrix4x4 uvTransform;
		Math::Vector2 uvMinSize;		// 0~1の範囲で指定
		Math::Vector2 uvMaxSize;		// 0~1の範囲で指定
		int arcType;
		float pad[3];
	};

	/// <summary>
	/// Transform情報
	/// </summary>
	struct TextureTransformData {
		Math::Matrix4x4 wvp;
	};

public:

	Sprite();
	~Sprite();

public:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="fileName"></param>
	void Init(const std::string& fileName);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="rectRange">: 描画する範囲</param>
	/// <param name="leftTop">: 左上座標</param>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="isBackGround">: バックグラウンド描画を行うか</param>
	void Draw(const AOENGINE::Pipeline* pipeline, bool isBackGround = false);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList"></param>
	void PostDraw(ID3D12GraphicsCommandList* commandList, const AOENGINE::Pipeline* pipeline) const;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

	/// <summary>
	/// 大きさや位置を調整する
	/// </summary>
	void Resize();

public:

	AOENGINE::ScreenTransform* GetTransform() const { return transform_.get(); }

	const Math::Matrix4x4& GetMatrix() const { return transform_->GetMatrix(); }

	/// <summary>
	/// Textureをセットする
	/// </summary>
	/// <param name="fileName">: セットするTexture名</param>
	void ReSetTexture(const std::string& fileName);

	/// <summary>
	/// Textureのサイズを再設計する
	/// </summary>
	/// <param name="size"></param>
	void ReSetTextureSize(const Math::Vector2& size);

	// 描画する範囲の設定
	void SetDrawRange(const Math::Vector2& rectRange) { drawRange_ = rectRange; };
	// 描画する範囲の設定
	void SetLeftTop(const Math::Vector2& leftTop) { leftTop_ = leftTop; }
	// Pivotの位置を変更する
	void SetAnchorPoint(const Math::Vector2& point) { anchorPoint_ = point; }

	void SetScale(const Math::Vector2 scale) { transform_->SetScale(scale); }
	void SetRotate(float rotate) { transform_->SetRotateZ(rotate); }
	void SetTranslate(const Math::Vector2& _pos) { transform_->SetTranslate(_pos); }
	void SetSaveTranslate(const Math::Vector2& _pos) { saveParam_.transform.translate = Math::Vector3(_pos.x, _pos.y, 0.0f); }

	void SetColor(const AOENGINE::Color& color) { materialData_->color = color; };
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }

	/// <summary>
	/// UVを直接する
	/// </summary>
	/// <param name="range"></param>
	void SetUvMinSize(const Math::Vector2& range) { materialData_->uvMinSize = range; }

	void SetUvMaxSize(const Math::Vector2& range) { materialData_->uvMaxSize = range; }

	void FillAmount(float amount);

	const Math::Vector2 GetTranslate() const { return transform_->GetTranslate(); }
	const Math::Vector2 GetScale() const { return transform_->GetScale(); }
	const float GetRotate() const { return transform_->GetRotateZ(); }
	const Math::Vector2 GetSpriteSize() const { return spriteSize_; }
	const bool GetIsFlipX() const { return isFlipX_; }
	const bool GetIsFlipY() const { return isFlipY_; }

	const AOENGINE::Color& GetColor() const { return materialData_->color; }

	const bool GetEnable() const { return isEnable_; }
	void SetEnable(bool _isEnable) { isEnable_ = _isEnable; }

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; }
	bool GetIsDestroy() const { return isDestroy_; }

	void SetIsBackGround(bool _isBackGround) { isBackGround_ = _isBackGround; }
	bool GetIsBackGround() const { return isBackGround_; }

	void SetIsFront(bool _isFront) { isFront_ = _isFront; }

	void SetFillMethod(FillMethod _method) { fillMethod_ = _method; }
	void SetFillStartingPoint(FillStartingPoint _point) { fillStartingPoint_ = _point; }

private:

	bool isEnable_;
	bool isDestroy_;
	bool isBackGround_;
	bool isFront_;

	// -------------------
	// DirectX関連
	// -------------------

	// 定数バッファ
	ComPtr<ID3D12Resource> vertexBuffer_;
	ComPtr<ID3D12Resource> indexBuffer_;
	ComPtr<ID3D12Resource> materialBuffer_;
	ComPtr<ID3D12Resource> arcGaugeParamBuffer_;

	// view
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	// データ
	TextureMesh* vertexData_;
	uint32_t* indexData_;
	TextureMaterial* materialData_;
	AOENGINE::ArcGaugeParam* arcData_;

	// Transform情報
	std::unique_ptr<AOENGINE::ScreenTransform> transform_;
	Math::SRT* parentTransform_ = nullptr;
	Math::SRT uvTransform_;

	// -------------------
	// Sprite情報
	// -------------------

	std::string textureName_;

	// 描画する範囲
	Math::Vector2 drawRange_ = { 0.0f, 0.0f };
	// 左上座標
	Math::Vector2 leftTop_ = { 0.0f, 0.0f };

	Math::Vector2 anchorPoint_;

	bool isFlipX_ = false;	// 左右フリップ
	bool isFlipY_ = false;	// 上下フリップ

	// Textureのサイズ
	Math::Vector2 textureSize_;
	Math::Vector2 spriteSize_;

	// -------------------
	// 塗りつぶしに関する変数
	// -------------------

	FillMethod fillMethod_;
	FillStartingPoint fillStartingPoint_;	// 塗りつぶし起点

	// -------------------
	// 外部保存のための変数
	// -------------------

	AOENGINE::SpriteParameter saveParam_;
	std::string saveGroupName_;
	std::string saveKeyName_;

public:

	void ApplyParam();

	void Load(const std::string& _group, const std::string& _key);
	void Save(const std::string& _group, const std::string& _key);

};
}