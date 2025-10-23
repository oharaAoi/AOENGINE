#pragma once
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/System/ParticleSystem/Emitter/GpuEmitter.h"

/// <summary>
/// 箱型のemitter
/// </summary>
class BoxEmitter : public GpuEmitter {
public:

	struct Emitter {
		Vector3 size_;
	};

public:

	BoxEmitter();
	~BoxEmitter();

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// コマンドを積む
	void BindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) override;
	// 形状の描画
	void DrawShape(const Matrix4x4& viewProjectionMat) override;
	// 保存
	void Save() override;
	// 読み込み
	void Load() override;

#ifdef _DEBUG
	// 編集処理
	void Debug_Gui() override;
#endif


private:

	ComPtr<ID3D12Resource> boxEmitterBuffer_;
	Emitter* emitter_;

	OBB obb_;
};

