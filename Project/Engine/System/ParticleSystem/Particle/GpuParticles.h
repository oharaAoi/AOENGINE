#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/ShaderResource.h"

/// <summary>
/// GpuParticleを行うクラス
/// </summary>
class GpuParticles {
public:	// メンバ構造体

	struct Particle {
		Vector4 color;			// 色
		Vector3 scale;			// 拡縮
		Vector3 rotate;			// 回転
		Vector3 translate;		// 座標
		Vector3 velocity;		// 速度
		Vector3 acceleration;	// 加速度
		float lifeTime;			// 生存時間
		float currentTime;		// 現在の時間
		float damping;			// 減衰
		float gravity;			// 重力

		float pad;
	};

	struct PerView {
		Matrix4x4 viewProjection;
		Matrix4x4 billboardMat;
	};

	struct PerFrame {
		float time;
		float deltaTime;
	};

public:

	GpuParticles();
	~GpuParticles();

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="instanceNum">: インスタンス数</param>
	void Init(uint32_t instanceNum);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// コマンドを積む
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// Particle全体を初期化する
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParameterIndex"></param>
	void InitBindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

	/// <summary>
	/// 射出する際のコマンドを積む
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParameterIndex"></param>
	void EmitBindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

public:

	void SetViewProjection(const Matrix4x4& viewProjection);
	
private:

	uint32_t kInstanceNum_;

	std::unique_ptr<ShaderResource> particleResource_;
	std::unique_ptr<ShaderResource> freeListIndexResource_;
	std::unique_ptr<ShaderResource> freeListResource_;

	ComPtr<ID3D12Resource> perViewBuffer_;
	PerView* perView_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	std::vector<std::shared_ptr<Mesh>> meshArray_;
	std::vector<std::unique_ptr<Material>> materials_;

	std::unordered_map<std::string, ModelMaterialData> materialData_;
};

