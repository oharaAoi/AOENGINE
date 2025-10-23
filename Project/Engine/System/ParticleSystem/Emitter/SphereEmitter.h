#pragma once
#include "Engine/System/ParticleSystem/Emitter/GpuEmitter.h"

class SphereEmitter : public GpuEmitter {
public:

	struct Emitter {
		float radius;			// 射出半径
	};

public:

	SphereEmitter();
	~SphereEmitter();

public:

	// 初期化
	void Init() override;

	// 更新
	void Update() override;

	// コマンドに積む
	void BindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) override;

	// 形状の描画
	void DrawShape(const Matrix4x4& viewProjectionMat) override;

	// 保存
	void Save() override;

	// 読み込み
	void Load() override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif


private:

	ComPtr<ID3D12Resource> sphereEmitterBuffer_;
	Emitter* emitter_;

};

