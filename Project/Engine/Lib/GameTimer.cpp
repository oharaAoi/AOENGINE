#include "GameTimer.h"
#include "Engine/System/Manager/TextureManager.h"
#ifdef _DEBUG
#include "Externals/ImGui/imgui.h"
#include "Engine/System/Manager/ImGuiManager.h"
#endif

float GameTimer::deletaTime_ = 0.0f;
float GameTimer::fps_ = 60.0f;
float GameTimer::timeRate_ = 0.0f;
float GameTimer::totalTime_ = 0.0f;
float GameTimer::timeScale_ = 1.0f;

GameTimer::GameTimer(const uint32_t& fps) {
	frameDuration_ = std::chrono::milliseconds(1000 / fps);
	preFrameTime_ = std::chrono::steady_clock::now();
	deletaTime_ = 1.0f / static_cast<float>(fps);
}

GameTimer::~GameTimer() {
}

void GameTimer::CalculationFrame() {
	auto currentTime = std::chrono::steady_clock::now();
	deletaTime_ = std::chrono::duration<float>(currentTime - preFrameTime_).count();
	timeRate_ = deletaTime_ / kDeltaTime_;
	totalTime_ += deletaTime_;

	preFrameTime_ = currentTime;
}

void GameTimer::Debug() {
#ifdef _DEBUG
	ImGui::Begin("GameTimer", nullptr);
	float fps = 1.0f / deletaTime_;
	ImGui::Text("%f fps", fps);
	ImGui::Text("%f m/s", deletaTime_ * 1000.0f);

	TextureManager* tex = TextureManager::GetInstance();
	ImVec2 iconSize(16, 16);
	D3D12_GPU_DESCRIPTOR_HANDLE playHandle = tex->GetDxHeapHandles("play.png").handleGPU;
	D3D12_GPU_DESCRIPTOR_HANDLE pauseHandle = tex->GetDxHeapHandles("pause.png").handleGPU;

	ImTextureID playTex = reinterpret_cast<ImTextureID>(playHandle.ptr);
	ImTextureID pauseTex = reinterpret_cast<ImTextureID>(pauseHandle.ptr);

	if (ImGui::ImageButton("play", playTex, iconSize)) {
		// 再生処理
		timeScale_ = 1.0f;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton("pause", pauseTex, iconSize)) {
		// 一時停止処理
		timeScale_ = 0.0f;
	}

	ImGui::End();
#endif
}
