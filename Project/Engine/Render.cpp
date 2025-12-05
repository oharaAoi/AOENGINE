#include "Render.h"
#include "Engine/System/Manager/TextureManager.h"

using namespace AOENGINE;

namespace {
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	std::unique_ptr<LightGroup> lightGroup_ = nullptr;
	std::unique_ptr<ViewProjection> viewProjection_ = nullptr;
	std::unique_ptr<ViewProjection> viewProjection2D_ = nullptr;
	
	Matrix4x4 vpvpMatrix;
	
	std::unique_ptr<PrimitiveDrawer> primitiveDrawer_ = nullptr;
	
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

AOENGINE::Render::Render() {}
AOENGINE::Render::~Render() {}

void AOENGINE::Render::Finalize() {
	viewProjection2D_->Finalize();
	viewProjection_->Finalize();
	lightGroup_->Finalize();
	primitiveDrawer_->Finalize();
	shadowMap_.reset();
}

AOENGINE::Render* AOENGINE::Render::GetInstance() {
	static AOENGINE::Render instance;
	return &instance;
}

void AOENGINE::Render::Init(ID3D12GraphicsCommandList* commandList, ID3D12Device* device, AOENGINE::RenderTarget* renderTarget) {
	assert(commandList);
	commandList_ = commandList;
	GetInstance()->renderTarget_ = renderTarget;

	viewProjection_ = std::make_unique<ViewProjection>();
	viewProjection2D_ = std::make_unique<ViewProjection>();
	primitiveDrawer_ = std::make_unique<PrimitiveDrawer>();
	lightGroup_ = std::make_unique<LightGroup>();
	shadowMap_ = std::make_unique<ShadowMap>();

	viewProjection_->Init(device);
	viewProjection2D_->Init(device);

	// light
	lightGroup_->Init(device);

	primitiveDrawer_->Init(device);

	shadowMap_->Init();

	nearClip_ = 1.0f;
	farClip_ = 10000.0f;

	nearClip2D_ = 0.0f;
	farClip2D_ = 100.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Renderの更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Render::Update() {
	lightGroup_->Update();
	primitiveDrawer_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　LineのDrawCallを呼び出す
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Render::PrimitiveDrawCall() {
	primitiveDrawer_->DrawCall(commandList_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　AOENGINE::RenderTargetを任意の物に設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Render::SetRenderTarget(const std::vector<RenderTargetType>& renderTypes, const DescriptorHandles& depthHandle) {
	GetInstance()->renderTarget_->SetRenderTarget(commandList_, renderTypes, depthHandle);
}

void AOENGINE::Render::SetShadowMap() {
	shadowMap_->SetCommand();
}

void AOENGINE::Render::ChangeShadowMap() {
	shadowMap_->ChangeResource(commandList_);
}

void AOENGINE::Render::ResetShadowMap() {
	shadowMap_->ResetResource(commandList_);
}

LightGroup* AOENGINE::Render::GetLightGroup() {
	return lightGroup_.get();
}

ShadowMap* AOENGINE::Render::GetShadowMap() {
	return shadowMap_.get();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Spriteの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Render::DrawSprite(Sprite* sprite, const Pipeline* pipeline) {
	sprite->PostDraw(commandList_, pipeline);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　モデルの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Render::DrawModel(const Pipeline* pipeline, Model* model, const WorldTransform* worldTransform,
					   const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials) {
	lightGroup_->BindCommand(pipeline, commandList_);
	UINT index = 0;
	index = pipeline->GetRootSignatureIndex("gShadowMap");
	commandList_->SetGraphicsRootDescriptorTable(index, shadowMap_->GetDeptSrvHandle().handleGPU);

	model->Draw(commandList_, pipeline, worldTransform, viewProjection_.get(), materials);
}

void AOENGINE::Render::DrawModel(const Pipeline* pipeline, Mesh* mesh, const WorldTransform* worldTransform,
					   const D3D12_VERTEX_BUFFER_VIEW& vbv,
					   const std::unordered_map<std::string, std::unique_ptr<BaseMaterial>>& materials) {
	lightGroup_->BindCommand(pipeline, commandList_);

	UINT index = 0;
	std::string useMaterial = mesh->GetUseMaterial();
	BaseMaterial* material = materials.at(useMaterial).get();
	commandList_->IASetVertexBuffers(0, 1, &vbv);
	commandList_->IASetIndexBuffer(&mesh->GetIBV());
	index = pipeline->GetRootSignatureIndex("gMaterial");
	commandList_->SetGraphicsRootConstantBufferView(index, material->GetBufferAddress());
	index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform->BindCommandList(commandList_, index);
	index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
	viewProjection_->BindCommandList(commandList_, index);
	index = pipeline->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	viewProjection_->BindCommandListPrev(commandList_, index);

	// MaterialのShaderTypeによってバインドするものを変える
	index = pipeline->GetRootSignatureIndex("gTexture");
	if (material->GetShaderType() == MaterialShaderType::UniversalRender) {
		std::string textureName = material->GetAlbedoTexture();
		TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, textureName, index);
	} else if (material->GetShaderType() == MaterialShaderType::ShaderGraphRender) {
		AOENGINE::DxResource* dxResource = material->GetShaderGraph()->GetResource();
		if (dxResource) {
			ID3D12Resource* resource = dxResource->GetResource();
			if (resource) {
				commandList_->SetGraphicsRootDescriptorTable(index, dxResource->GetSRV().handleGPU);
			} else {
				TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, "error.png", index);
			}
		} else {
			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, "error.png", index);
		}
	}

	index = pipeline->GetRootSignatureIndex("gShadowMap");
	commandList_->SetGraphicsRootDescriptorTable(index, shadowMap_->GetDeptSrvHandle().handleGPU);

	commandList_->DrawIndexedInstanced(mesh->GetIndexNum(), 1, 0, 0, 0);
}

void AOENGINE::Render::DrawEnvironmentModel(const Pipeline* pipeline, Mesh* _mesh, BaseMaterial* _material, const WorldTransform* _transform) {
	lightGroup_->BindCommand(pipeline, commandList_);
	commandList_->IASetVertexBuffers(0, 1, &_mesh->GetVBV());
	commandList_->IASetIndexBuffer(&_mesh->GetIBV());

	UINT index = pipeline->GetRootSignatureIndex("gMaterial");
	commandList_->SetGraphicsRootConstantBufferView(index, _material->GetBufferAddress());

	index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
	_transform->BindCommandList(commandList_, index);
	index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
	viewProjection_->BindCommandList(commandList_, index);
	index = pipeline->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	viewProjection_->BindCommandListPrev(commandList_, index);

	std::string textureName = _material->GetAlbedoTexture();
	index = pipeline->GetRootSignatureIndex("gTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, textureName, index);
	index = pipeline->GetRootSignatureIndex("gEnviromentTexture");
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, skyboxTexture_, index);

	commandList_->DrawIndexedInstanced(_mesh->GetIndexNum(), 1, 0, 0, 0);
}

void AOENGINE::Render::SetShadowMesh(const Pipeline* pipeline, Mesh* mesh, const WorldTransform* worldTransform, const D3D12_VERTEX_BUFFER_VIEW& vbv) {
	UINT index = 0;
	commandList_->IASetVertexBuffers(0, 1, &vbv);
	commandList_->IASetIndexBuffer(&mesh->GetIBV());
	index = pipeline->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform->BindCommandList(commandList_, index);
	index = pipeline->GetRootSignatureIndex("gViewProjectionMatrix");
	lightGroup_->GetDirectionalLight()->ViewBindCommand(commandList_, index);
	
	commandList_->DrawIndexedInstanced(mesh->GetIndexNum(), 1, 0, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　線の描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Render::DrawLine(const Vector3& p1, const Vector3& p2, const Color& color, const Matrix4x4& vpMat) {
	primitiveDrawer_->Draw(p1, p2, color, vpMat);
}

void AOENGINE::Render::DrawLine(const Vector3& p1, const Vector3& p2, const Color& color) {
	primitiveDrawer_->Draw(p1, p2, color, viewProjection_->GetViewProjection());
}

void AOENGINE::Render::DrawLightGroup(Pipeline* pipeline) {
	lightGroup_->BindCommand(pipeline, commandList_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　設定系の関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::Render::SetViewProjection(const Matrix4x4& view, const Matrix4x4& projection) {
	viewProjection_->SetViewProjection(view, projection);
}

void AOENGINE::Render::SetViewProjection2D(const Matrix4x4& view, const Matrix4x4& projection) {
	viewProjection2D_->SetViewProjection(view, projection);
}

Matrix4x4 AOENGINE::Render::GetViewport2D() {
	return  viewProjection2D_->GetViewMatrix();
}

Matrix4x4 AOENGINE::Render::GetViewport3D() {
	return viewProjection_->GetViewMatrix();
}

Matrix4x4 AOENGINE::Render::GetProjection2D() {
	return  viewProjection2D_->GetProjectionMatrix();
}

Matrix4x4 AOENGINE::Render::GetProjection3D() {
	return viewProjection_->GetProjectionMatrix();
}

Matrix4x4 AOENGINE::Render::GetViewProjectionMat() {
	return viewProjection_->GetViewMatrix() * viewProjection_->GetProjectionMatrix();
}

void AOENGINE::Render::SetVpvpMatrix(const Matrix4x4& _mat) {
	vpvpMatrix = _mat;
}

Matrix4x4 AOENGINE::Render::GetVpvpMatrix() {
	return vpvpMatrix;
}

float AOENGINE::Render::GetNearClip() {
	return nearClip_;
}

float AOENGINE::Render::GetNearClip2D() {
	return nearClip2D_;
}

float AOENGINE::Render::GetFarClip() {
	return farClip_;
}

float AOENGINE::Render::GetFarClip2D() {
	return farClip2D_;
}

void AOENGINE::Render::SetEyePos(const Vector3& eyePos) {
	eyePos_ = eyePos;
	lightGroup_->SetEyePos(eyePos);
}

const Vector3& AOENGINE::Render::GetEyePos() {
	return eyePos_;
	// TODO: return ステートメントをここに挿入します
}

void AOENGINE::Render::SetCameraRotate(const Quaternion& rotate) {
	cameraRotate_ = rotate;
}

Quaternion AOENGINE::Render::GetCameraRotate() {
	return cameraRotate_;
}

Matrix4x4 AOENGINE::Render::GetBillBordMat() {
	Matrix4x4 billMatrix = AOENGINE::Render::GetCameraRotate().MakeMatrix();
	return Multiply(Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), billMatrix);
}

const ViewProjection* AOENGINE::Render::GetViewProjection() {
	return viewProjection_.get();
}

void AOENGINE::Render::SetSkyboxTexture(const std::string& _name) {
	skyboxTexture_ = _name;
}

ID3D12Resource* AOENGINE::Render::GetShadowDepth() {
	return shadowMap_->GetDepthResource();
}
