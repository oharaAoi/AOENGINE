#include "ImGuiManager.h"
#include "ImGuizmo.h"
#include "Engine/WinApp/WinApp.h"

ImGuiManager* ImGuiManager::GetInstacne() {
	static ImGuiManager instance;
	return &instance;
}

bool ImGuiManager::ImGuiHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	return false;
}

void ImGuiManager::Init(HWND hwnd, ID3D12Device* device, uint32_t bufferCount, ID3D12DescriptorHeap* srvHeap) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	
	ImFontConfig fontConfig;
	fontConfig.PixelSnapH = false;
	fontConfig.OversampleH = 3;
	fontConfig.OversampleV = 3;
	io.Fonts->AddFontFromFileTTF("./Project/Packages/Engine/Assets/Font/Cica-Regular.ttf", 14.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	io.Fonts->Build();
	//io.Fonts->AddFontFromFileTTF("./Engine/Resources/Font/FiraMono-Medium.ttf", 80 * 10.f);
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//ImGui::GetIO().FontGlobalScale = 1.0f / 50.f;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		bufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvHeap,
		srvHeap->GetCPUDescriptorHandleForHeapStart(),
		srvHeap->GetGPUDescriptorHandleForHeapStart()
	);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;

}

void ImGuiManager::Finalize(){
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Begin(){
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	UpdateMousePosition(WinApp::GetInstance()->GetHwnd(), Vector2(float(WinApp::sWindowWidth), float(WinApp::sWindowHeight)));
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	//ImGui::SetWindowFontScale(0.1f);
}

void ImGuiManager::End() {
	
	ImGui::Render();
}

void ImGuiManager::UpdateMousePosition(HWND _winHwnd, const Vector2& _renderTargetSize) {
	POINT point;
	GetCursorPos(&point);

	ScreenToClient(_winHwnd, &point);

	RECT clientRect;
	GetClientRect(_winHwnd, &clientRect);

	Vector2 clientSize = {
		static_cast<float>(clientRect.right - clientRect.left),
		static_cast<float>(clientRect.bottom - clientRect.top)
	};

	/// 補正
	Vector2 scale = _renderTargetSize / clientSize;
	Vector2 corrected = {
		point.x * scale.x,
		point.y * scale.y
	};

	ImGui::GetIO().AddMousePosEvent(corrected.x, corrected.y);
}

void ImGuiManager::Draw(ID3D12GraphicsCommandList* commandList) {
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}