#pragma once
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Matrix4x4.h"

namespace AOENGINE {

/// <summary>
/// 線を描画するためのクラス
/// </summary>
class PrimitiveDrawer {
public: // データ構造体

	/// <summary>
	/// Gpuに送る構造体
	/// </summary>
	struct PrimitiveData {
		Math::Vector4 pos;
		AOENGINE::Color color;
	};

	struct ThickLineData {
		Math::Vector4 start;
		Math::Vector4 end;
		AOENGINE::Color color;
		// x: 始点(0) / 終点(1), y: 線の左右(-0.5 / 0.5), z: 太さ(pixel)
		Math::Vector3 line;
		Math::Vector2 viewportSize;
	};

	// 線分の最大数
	static const UINT kMaxLineCount = 40960;
	// 線分の頂点数
	static const UINT kVertexCountLine = 2;
	static const UINT kVertexCountThickLine = 6;
	static const UINT kMaxThickLineVertexCount = 40962;

public: // コンストラクタ

	PrimitiveDrawer() {};
	~PrimitiveDrawer() {};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(ID3D12Device* device);

	/// <summary>
	/// 開始処理
	/// </summary>
	void Update();

	/// <summary>
	/// 解放
	/// </summary>
	void Finalize();

	/// <summary>
	/// 描画のコマンドを積む
	/// </summary>
	/// <param name="p1">: 始点</param>
	/// <param name="p2">: 終点</param>
	/// <param name="color">: 色</param>
	/// <param name="wvpMat">: 透視投影行列</param>
	void Draw(const Math::Vector3& p1, const Math::Vector3& p2, const AOENGINE::Color& color, const Math::Matrix4x4& wvpMat);

	/// <summary>
	/// 画面上で指定したピクセル幅を持つ線分を描画キューへ追加します。
	/// </summary>
	void DrawThick(const Math::Vector3& p1, const Math::Vector3& p2, const AOENGINE::Color& color,
		float thickness, const Math::Matrix4x4& vpMat);

	/// <summary>
	/// 描画コマンドを積む
	/// </summary>
	/// <param name="commandList">: commandList</param>
	void DrawCall(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 使用中のindexを設定する
	/// </summary>
	/// <param name="count"></param>
	void SetUseIndex(const uint32_t& count) { useIndex_ = count; }

private:

	// 定数バッファ
	ComPtr<ID3D12Resource> vertexBuffer_;
	ComPtr<ID3D12Resource> indexBuffer_;
	ComPtr<ID3D12Resource> wvpBuffer_;
	ComPtr<ID3D12Resource> thickLineVertexBuffer_;

	// バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};
	D3D12_VERTEX_BUFFER_VIEW thickLineVertexBufferView_ = {};

	DescriptorHandles wvpSRV_;

	// データポインタ
	PrimitiveData* primitiveData_;
	uint32_t* indexData_ = nullptr;
	Math::Matrix4x4* wvpData_;
	ThickLineData* thickLineData_ = nullptr;

	// 使用している線の頂点の数
	uint32_t useIndex_ = 0;

	uint32_t preUseIndex_;
	uint32_t thickLineVertexCount_ = 0;

};

}
