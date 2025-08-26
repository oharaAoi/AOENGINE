#pragma once
#include <unordered_map>
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/DirectX/RTV/RenderTarget.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/2d/PrimitiveDrawer.h"
#include "Engine/Module/Components/Light/LightGroup.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/DirectX/Pipeline/PipelineGroup/PrimitivePipeline.h"
#include "Engine/Render/ShadowMap.h"

/// <summary>
/// GameObjectの描画を行う
/// </summary>
class Render {
public:

	Render();
	~Render();
	Render(const Render&) = delete;
	const Render& operator=(const Render&) = delete;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Render* GetInstance();

	void Finalize();
	void Init(ID3D12GraphicsCommandList* commandList, ID3D12Device* device, PrimitivePipeline* primitive, RenderTarget* renderTarget);
	static void Update();

	static void PrimitiveDrawCall();
	static void SetRenderTarget(const std::vector<RenderTargetType>& renderTypes, const DescriptorHandles& depthHandle);

	static void SetShadowMap();
	static void ChangeShadowMap();
	static void ResetShadowMap();

	static LightGroup* GetLightGroup();
	static ShadowMap* GetShadowMap();

public:

	/// <summary>
	/// Spriteの描画
	/// </summary>
	/// <param name="sprite">: spriteのポインタ</param>
	static void DrawSprite(Sprite* sprite, const Pipeline* pipeline);

	/// <summary>
	/// モデルの描画
	/// </summary>
	/// <param name="model">: モデルのポインタ</param>
	/// <param name="worldTransform">: worldTransform</param>
	static void DrawModel(const Pipeline* pipeline, Model* model, const WorldTransform* worldTransform,
						  const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials);

	static void DrawModel(const Pipeline* pipeline, Mesh* mesh, const WorldTransform* worldTransform,
						  const D3D12_VERTEX_BUFFER_VIEW& vbv, const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials);

	static void DrawEnvironmentModel(const Pipeline* pipeline, Mesh* _mesh, BaseMaterial* _material, const WorldTransform* _transform);

	/// <summary>
	/// 線の描画
	/// </summary>
	/// <param name="p1">: 始点</param>
	/// <param name="p2">: 終点</param>
	/// <param name="color">: 色</param>
	/// <param name="vpMat">: viewProjection</param>
	static void DrawLine(const Vector3& p1, const Vector3& p2, const Vector4& color, const Matrix4x4& vpMat);

	/// <summary>
	/// 線の描画
	/// </summary>
	/// <param name="p1">: 始点</param>
	/// <param name="p2">: 終点</param>
	/// <param name="color">: 色</param>
	static void DrawLine(const Vector3& p1, const Vector3& p2, const Vector4& color);

	static void DrawLightGroup(Pipeline* pipeline);

	static void SetShadowMesh(const Pipeline* pipeline, Mesh* mesh, const WorldTransform* worldTransform, const D3D12_VERTEX_BUFFER_VIEW& vbv);

	//==================================================================================
	// ↓　設定系
	//==================================================================================

	/// <summary>
	/// 画面に移す範囲をここで設定する
	/// </summary>
	/// <param name="view"></param>
	/// <param name="projection"></param>
	static void SetViewProjection(const Matrix4x4& view, const Matrix4x4& projection);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="view"></param>
	/// <param name="projection"></param>
	static void SetViewProjection2D(const Matrix4x4& view, const Matrix4x4& projection);

	static Matrix4x4 GetViewport2D();
	static Matrix4x4 GetProjection2D();
	static Matrix4x4 GetViewport3D();
	static Matrix4x4 GetProjection3D();
	static Matrix4x4 GetViewProjectionMat();

	static float GetNearClip();
	static float GetNearClip2D();
	static float GetFarClip();
	static float GetFarClip2D();

	static void SetEyePos(const Vector3& eyePos);
	static const Vector3& GetEyePos();

	static void SetCameraRotate(const Quaternion& rotate);
	static Quaternion GetCameraRotate();

	static Matrix4x4 GetBillBordMat();

	static const ViewProjection* GetViewProjection();

	static void SetSkyboxTexture(const std::string& _name);

	static ID3D12Resource* GetShadowDepth();

private:

	RenderTarget* renderTarget_ = nullptr;

};

namespace {
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	std::unique_ptr<ViewProjection> viewProjection_ = nullptr;
	std::unique_ptr<ViewProjection> viewProjection2D_ = nullptr;

	std::unique_ptr<PrimitiveDrawer> primitiveDrawer_ = nullptr;
	PrimitivePipeline* primitivePipelines_ = nullptr;

	std::unique_ptr<ShadowMap> shadowMap_ = nullptr;

	float nearClip_;
	float farClip_;
	float nearClip2D_;
	float farClip2D_;

	RenderTargetType currentRenderTarget_;

	Quaternion cameraRotate_;
	Vector3 eyePos_;

	std::string skyboxTexture_;
}
