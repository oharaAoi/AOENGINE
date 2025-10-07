#pragma once
#include <string>
#include <memory>
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/ScreenTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Render;

enum class FillStartingPoint {
	Top,
	Bottom,
	Left,
	Right,
	TopBottom,
	LeftRight,
};

enum class FillMethod {
	Vertical,		// 垂直
	Horizontal,		// 水平
	Radial,			// 円形
	BothEnds		// 両端
};

struct ArcGaugeParam {
	Vector2 center;		// 中心座標
	float fillAmount;	// 塗りつぶし量
	float innerRadius;	// 内半径
	float outerRadius;	// 外半径
	float startAngle;	// 開始角度
	float arcRange;		// 弧の最大角度
	int clockwise;		// 回転方向
};

struct SpriteParameter : public IJsonConverter {
	SRT transform = SRT();
	SRT uvTransform = SRT();
	std::string textureName = "white.png";
	Color color = Color::white;
	Vector2 drawRange = Vector2(64.0f, 64.0f);
	Vector2 leftTop = CVector2::ZERO;
	Vector2 anchorPoint = Vector2(0.5f, 0.5f);
	bool isFlipX = false;
	bool isFlipY = false;
	Vector2 spriteSize = Vector2(64.0f, 64.0f);

	int fillMethod = static_cast<int>(FillMethod::Vertical);
	int fillStartingPoint = static_cast<int>(FillStartingPoint::Left);

	Vector2 center = Vector2(0.5f, 0.5f);	// 中心座標
	float fillAmount = 1.0f;				// 塗りつぶし量
	float innerRadius = 0.0f;				// 内半径
	float outerRadius = 1.0f;				// 外半径
	float startAngle = 0.0f;				// 開始角度
	float arcRange = kPI;					// 弧の最大角度
	int clockwise = 0;						// 回転方向

	json ToJson(const std::string& id) const override {
		json srt = transform.ToJson();
		json uvSrt = transform.ToJson();
		return JsonBuilder(id)
			.Add("transform", srt)
			.Add("uvTransform", uvSrt)
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
			.Build();
	}

	void FromJson(const json& jsonData) override {
		transform.FromJson(jsonData, "transform");
		uvTransform.FromJson(jsonData, "uvTransform");
		fromJson(jsonData, "textureName", textureName);
		fromJson(jsonData, "color", color);
		fromJson(jsonData, "drawRange", drawRange);
		fromJson(jsonData, "anchorPoint", anchorPoint);
		fromJson(jsonData, "isFlipX", isFlipX);
		fromJson(jsonData, "isFlipY", isFlipY);
		fromJson(jsonData, "spriteSize", spriteSize);
		fromJson(jsonData, "fillMethod", fillMethod);
		fromJson(jsonData, "fillStartingPoint", fillStartingPoint);
		fromJson(jsonData, "center", center);
		fromJson(jsonData, "fillAmount", fillAmount);
		fromJson(jsonData, "innerRadius", innerRadius);
		fromJson(jsonData, "outerRadius", outerRadius);
		fromJson(jsonData, "startAngle", startAngle);
		fromJson(jsonData, "arcRange", arcRange);
		fromJson(jsonData, "clockwise", clockwise);
	}

	void Debug_Gui() override {};
};

class Sprite :
	public AttributeGui {
public:

	struct TextureMesh {
		Vector4 pos;
		Vector2 texcoord;
		float padding[2];
	};

	struct TextureMaterial {
		Color color;
		Matrix4x4 uvTransform;
		Vector2 uvMinSize;		// 0~1の範囲で指定
		Vector2 uvMaxSize;		// 0~1の範囲で指定
		int arcType;
		float pad[3];
	};

	struct TextureTransformData {
		Matrix4x4 wvp;
	};

public:

	Sprite();
	~Sprite();

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
	void Draw(const Pipeline* pipeline, bool isBackGround = false);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList"></param>
	void PostDraw(ID3D12GraphicsCommandList* commandList, const Pipeline* pipeline) const;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

public:

	ScreenTransform* GetTransform() const { return transform_.get(); }

	/// <summary>
	/// Textureをセットする
	/// </summary>
	/// <param name="fileName">: セットするTexture名</param>
	void ReSetTexture(const std::string& fileName);

	/// <summary>
	/// Textureのサイズを再設計する
	/// </summary>
	/// <param name="size"></param>
	void ReSetTextureSize(const Vector2& size);

	// 描画する範囲の設定
	void SetDrawRange(const Vector2& rectRange) { drawRange_ = rectRange; };
	// 描画する範囲の設定
	void SetLeftTop(const Vector2& leftTop) { leftTop_ = leftTop; }
	// Pivotの位置を変更する
	void SetAnchorPoint(const Vector2& point) { anchorPoint_ = point; }

	void SetScale(const Vector2 scale) { transform_->SetScale(scale); }
	void SetRotate(float rotate) { transform_->SetRotateZ(rotate); }
	void SetTranslate(const Vector2& _pos) { transform_->SetTranslate(_pos); }

	void SetColor(const Color& color) { materialData_->color = color; };
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }

	/// <summary>
	/// UVを直接する
	/// </summary>
	/// <param name="range"></param>
	void SetUvMinSize(const Vector2& range) { materialData_->uvMinSize = range; }

	void SetUvMaxSize(const Vector2& range) { materialData_->uvMaxSize = range; }

	void FillAmount(float amount);

	const Vector2 GetTranslate() const { return transform_->GetTranslate(); }
	const Vector2 GetScale() const { return transform_->GetScale(); }
	const float GetRotate() const { return transform_->GetRotateZ(); }
	const Vector2 GetSpriteSize() const { return spriteSize_; }
	const bool GetIsFlipX() const { return isFlipX_; }
	const bool GetIsFlipY() const { return isFlipY_; }

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
	ArcGaugeParam* arcData_;

	// Transform情報
	std::unique_ptr<ScreenTransform> transform_;
	SRT* parentTransform_ = nullptr;
	SRT uvTransform_;
	
	// -------------------
	// Sprite情報
	// -------------------

	std::string textureName_;

	// 描画する範囲
	Vector2 drawRange_ = { 0.0f, 0.0f };
	// 左上座標
	Vector2 leftTop_ = { 0.0f, 0.0f };
	
	Vector2 anchorPoint_;

	bool isFlipX_ = false;	// 左右フリップ
	bool isFlipY_ = false;	// 上下フリップ

	// Textureのサイズ
	Vector2 textureSize_;
	Vector2 spriteSize_;

	// -------------------
	// 塗りつぶしに関する変数
	// -------------------

	FillMethod fillMethod_;
	FillStartingPoint fillStartingPoint_;	// 塗りつぶし起点

	// -------------------
	// 外部保存のための変数
	// -------------------

	SpriteParameter saveParam_;
	std::string saveGroupName_;
	std::string saveKeyName_;

public:

	void ApplyParam();

	void Load(const std::string& _group, const std::string& _key);
	void Save(const std::string& _group, const std::string& _key);

};