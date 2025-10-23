#pragma once
#include "Engine/System/ParticleSystem/Emitter/GpuEmitter.h"

/// <summary>
/// cone形状のemitter
/// </summary>
class ConeEmitter : public GpuEmitter {
public:

	struct Emitter {
		float radius;			// 射出半径
		float angle;			// 円錐の角度
		float height;			// 円錐の高さ
	};

public:

	ConeEmitter();
	~ConeEmitter();

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

	ComPtr<ID3D12Resource> coneEmitterBuffer_;
	Emitter* emitter_;

};

