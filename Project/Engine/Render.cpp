#include "Render.h"
#include "Engine/System/Manager/TextureManager.h"

Render::Render() {}
Render::~Render() {}

void Render::Finalize() {
	viewProjection2D_->Finalize();
	viewProjection_->Finalize();
	lightGroup_->Finalize();
	primitiveDrawer_->Finalize();
}

Render* Render::GetInstance() {
	static Render instance;
	return &instance;
}

void Render::Init(ID3D12GraphicsCommandList* commandList, ID3D12Device* device, PrimitivePipeline* primitive, RenderTarget* renderTarget) {
	assert(commandList);
	commandList_ = commandList;
	primitivePipelines_ = primitive;
	GetInstance()->renderTarget_ = renderTarget;

	viewProjection_ = std::make_unique<ViewProjection>();
	viewProjection2D_ = std::make_unique<ViewProjection>();
	primitiveDrawer_ = std::make_unique<PrimitiveDrawer>();
	lightGroup_ = std::make_unique<LightGroup>();

	viewProjection_->Init(device);
	viewProjection2D_->Init(device);

	// light
	lightGroup_->Init(device);

	primitiveDrawer_->Init(device);

	nearClip_ = 1.0f;
	farClip_ = 10000.0f;

	nearClip2D_ = 0.0f;
	farClip2D_ = 100.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Renderの更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void Render::Update() {
	lightGroup_->Update();
	primitiveDrawer_->Begin();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　LineのDrawCallを呼び出す
//////////////////////////////////////////////////////////////////////////////////////////////////

void Render::PrimitiveDrawCall() {
	primitiveDrawer_->DrawCall(commandList_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　RenderTargetを任意の物に設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Render::SetRenderTarget(const RenderTargetType& type) {
	currentRenderTarget_ = type;
	GetInstance()->renderTarget_->SetRenderTarget(commandList_, type);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Spriteの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void Render::DrawSprite(Sprite* sprite) {
	sprite->PostDraw(commandList_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　モデルの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void Render::DrawModel(Model* model, const WorldTransform* worldTransform, const std::vector<std::unique_ptr<Material>>& materials) {
	lightGroup_->Draw(commandList_, 4);
	model->Draw(commandList_, worldTransform, viewProjection_.get(), materials);
}

void Render::DrawModel(Model* model, const WorldTransform* worldTransform,
					   const D3D12_VERTEX_BUFFER_VIEW& vbv,
					   const std::vector<std::unique_ptr<Material>>& materials) {
	lightGroup_->Draw(commandList_, 4);
	model->Draw(commandList_, worldTransform, viewProjection_.get(), vbv, materials);
}

void Render::DrawEnvironmentModel(Mesh* _mesh, Material* _material, const WorldTransform* _transform) {
	lightGroup_->Draw(commandList_, 4);
	commandList_->IASetVertexBuffers(0, 1, &_mesh->GetVBV());
	commandList_->IASetIndexBuffer(&_mesh->GetIBV());
	commandList_->SetGraphicsRootConstantBufferView(0, _material->GetBufferAdress());

	_transform->BindCommandList(commandList_);
	viewProjection_->BindCommandList(commandList_);

	std::string textureName = _material->GetUseTexture();
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, textureName, 3);
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList_, skyboxTexture_, 7);

	commandList_->DrawIndexedInstanced(_mesh->GetIndexNum(), 1, 0, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　線の描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void Render::DrawLine(const Vector3& p1, const Vector3& p2, const Vector4& color, const Matrix4x4& vpMat) {
	primitiveDrawer_->Draw(p1, p2, color, vpMat);
}

void Render::DrawLine(const Vector3& p1, const Vector3& p2, const Vector4& color) {
	primitiveDrawer_->Draw(p1, p2, color, viewProjection_->GetViewProjection());
}

void Render::DrawLightGroup(const int& startIndex) {
	lightGroup_->Draw(commandList_, startIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　設定系の関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void Render::SetViewProjection(const Matrix4x4& view, const Matrix4x4& projection) {
	viewProjection_->SetViewProjection(view, projection);
}

void Render::SetViewProjection2D(const Matrix4x4& view, const Matrix4x4& projection) {
	viewProjection2D_->SetViewProjection(view, projection);
}

Matrix4x4 Render::GetViewport2D() {
	return  viewProjection2D_->GetViewMatrix();
}

Matrix4x4 Render::GetViewport3D() {
	return viewProjection_->GetViewMatrix();
}

Matrix4x4 Render::GetProjection2D() {
	return  viewProjection2D_->GetProjectionMatrix();
}

Matrix4x4 Render::GetProjection3D() {
	return viewProjection_->GetProjectionMatrix();
}

Matrix4x4 Render::GetViewProjectionMat() {
	return viewProjection_->GetViewMatrix() * viewProjection_->GetProjectionMatrix();
}

float Render::GetNearClip() {
	return nearClip_;
}

float Render::GetNearClip2D() {
	return nearClip2D_;
}

float Render::GetFarClip() {
	return farClip_;
}

float Render::GetFarClip2D() {
	return farClip2D_;
}

void Render::SetEyePos(const Vector3& eyePos) {
	eyePos_ = eyePos;
	lightGroup_->SetEyePos(eyePos);
}

const Vector3& Render::GetEyePos() {
	return eyePos_;
	// TODO: return ステートメントをここに挿入します
}

void Render::SetCameraRotate(const Quaternion& rotate) {
	cameraRotate_ = rotate;
}

Quaternion Render::GetCameraRotate() {
	return cameraRotate_;
}

const ViewProjection* Render::GetViewProjection() {
	return viewProjection_.get();
}

void Render::SetSkyboxTexture(const std::string& _name) {
	skyboxTexture_ = _name;
}
