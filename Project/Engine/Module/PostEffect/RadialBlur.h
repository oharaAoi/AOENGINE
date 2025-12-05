#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

/// <summary>
/// 放射ブラー
/// </summary>
class RadialBlur :
	public IPostEffect {
public:

	struct BlurSetting {
		Math::Vector2 blurCenter = {0.5f, 0.5f};	// 放射ブラーの中心（通常はfloat2(0.5, 0.5)）
		float blurStrength = 0.00f;			// ブラーの強度（例: 0.02）
		float blurStart = 0.2f;				// ブラーの開始距離
		int sampleCount = 16;				// サンプル数（例: 16）
	};

public:

	RadialBlur() = default;
	~RadialBlur() override;

public:

	// 初期化
	void Init() override;
	// コマンドを積む
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;
	// チェックボックスの表示
	void CheckBox() override;
	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// 開始処理
	/// </summary>
	/// <param name="_strength">: 強さ</param>
	/// <param name="_startTime">: 時間</param>
	/// <param name="_continuation">: 継続するか</param>
	void Start(float _strength, float _startTime, bool _continuation);

	/// <summary>
	/// 徐々に止める
	/// </summary>
	/// <param name="time"></param>
	void SlowDown(float time);

	// 止める
	void Stop();

public:

	void SetStrength(float strength) { setting_->blurStrength = strength; }

private:

	AOENGINE::DxResource* blurSettingBuffer_;
	BlurSetting* setting_;

	bool run_ = false;
	bool continuation_ = false; // 継続するか
	VectorTween<float> blurStrengthTween_;
};

