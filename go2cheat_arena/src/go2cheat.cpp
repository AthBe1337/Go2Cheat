#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <algorithm>
#include <mutex>

#pragma comment(lib, "d3d11.lib")
#include "../tools/imgui/imgui.h"
#include "../tools/imgui/imgui_impl_dx11.h"
#include "../tools/imgui/imgui_impl_win32.h"

#include "../tools/utils/process.h"
#include "../tools/driver.h"
#include "../tools/utils/Players.h"
#include "../tools/utils/memory.h"

#include "../features/esp.h"
#include "../features/radarHack.h"
#include "../features/triggerbot.h"

#include "../tools/Coutput.h"

std::atomic<bool> keep_running(true);
std::mutex players_mutex;

void refresh_thread()
{
	while (keep_running)
	{
		{
			std::lock_guard<std::mutex> lock(players_mutex);
			utils::Players::refresh_player_list();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void triggerbot()
{
	while (keep_running)
	{
		if (GetAsyncKeyState(VK_LMENU))
		{
			std::lock_guard<std::mutex>lock(players_mutex);
			features::triggerbot::trigger();
		}
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
	{
		return 0L;
	}


	if (message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, w_param, l_param);
}

#ifdef EXECUTABLE
INT WINAPI WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show)
{
	RedirectToNewConsole();

	normal_print("正在初始化...");

	DWORD pid = utils::process::get_process_id(L"cs2.exe");
	if (pid == 0) {
		error_print("游戏未运行！");
		return 1;
	}

	debug_print("cs2.exe PID: %d", pid);

	driver::initialize();

	if (!driver::attach_to_process(driver_handle, pid))
	{
		error_print("驱动连接失败！");
		driver::close();
		return 1;
	}

	debug_print("Driver attached to cs2.exe");

	//client_dll_base = utils::process::get_module_base(pid, L"client.dll");
	client_dll_base = utils::memory::FindPattern("E8 ? ? ? ? 4C 8D 0D ? ? ? ? C6 44 24 ? ? 4C 8D 45 6F", 16) - client_offset;

	if (!client_dll_base) {
		driver::close();
		return 1;
	}

	debug_print("client_dll_base: 0x%p", client_dll_base);
	normal_print("初始化已完成！");

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"External Overlay Class";


	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		wc.lpszClassName,
		L"ESP",
		WS_POPUP,
		0,
		0,
		screenW,
		screenH,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);


	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins
		{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;

	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;


	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else
	{
		return 1;
	}


	ShowWindow(window, cmd_show);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simhei.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	normal_print("字体已加载: simhei.ttf");

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);
	normal_print("ImGui已初始化");

	std::thread refresh_thread_instance(refresh_thread);
	std::thread triggerbot_thread_instance(triggerbot);
	normal_print("线程已创建");

	bool running = true;
	bool rendering = false;
	bool radar_hack = false;
	bool render_friend = false;
	int players_count = 0;

	normal_print(FOREGROUND_GREEN,"去作弊Go2Cheat已准备完成");
	while (running && utils::process::is_process_running(pid))
	{
		{
			std::lock_guard<std::mutex> lock(players_mutex);
			if (players_count != utils::Players::players.size())
			{
				players_count = utils::Players::players.size();
				normal_print(FOREGROUND_RED | FOREGROUND_GREEN,"玩家数量: %d", players_count);
			}
		}
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			if(!rendering)
			{
				rendering = !rendering;
			}
			else if (!render_friend) {
				render_friend = !render_friend;
			}
			else {
				rendering = !rendering;
				render_friend = !render_friend;
			}
			normal_print("ESP渲染已%s, 队友渲染已%s", rendering ? "开启" : "关闭", render_friend ? "开启" : "关闭");
		}
		if (GetAsyncKeyState(VK_HOME) & 1) {
			radar_hack = !radar_hack;
			normal_print("雷达透视已%s", radar_hack ? "开启" : "关闭");
		}
		if (GetAsyncKeyState(VK_END) & 1)
		{
			running = false;
		}

		if (!running)
		{
			break;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		//ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(200, 200), 50.0f, IM_COL32(255, 0, 0, 255), 0, 2.0f);

		{
			std::lock_guard<std::mutex> lock(players_mutex);

			if (radar_hack) {
				features::radarHack::set_spotted_state();
			}

			if (rendering) {
				features::esp::draw_esp(render_friend);
			}
		}

		features::esp::C4_info();

		ImGui::Render();

		constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(0U, 0U);		
	}

	error_print("正在退出...");
	keep_running = false;
	if (refresh_thread_instance.joinable())
	{
		refresh_thread_instance.join();
	}
	if (triggerbot_thread_instance.joinable())
	{
		triggerbot_thread_instance.join();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();


	if (swap_chain)
	{
		swap_chain->Release();
	}

	if (device_context)
	{
		device_context->Release();
	}

	if (render_target_view)
	{
		render_target_view->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	driver::close();
	return 0;
}
#elif defined(DLL)

#define DLL_API __declspec(dllexport)


extern "C" DLL_API int Main(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show = SW_SHOW)
{
	RedirectToNewConsole();

	normal_print("正在初始化...");

	DWORD pid = utils::process::get_process_id(L"cs2.exe");
	if (pid == 0) {
		error_print("游戏未运行！");
		FreeConsole();
		return 1;
	}

	debug_print("cs2.exe PID: %d", pid);

	driver::initialize();

	if (!driver::attach_to_process(driver_handle, pid))
	{
		driver::close();
		error_print("驱动连接失败！");
		FreeConsole();
		return 1;
	}

	debug_print("Driver attached to cs2.exe");

	//client_dll_base = utils::process::get_module_base(pid, L"client.dll");
	client_dll_base = utils::memory::FindPattern("E8 ? ? ? ? 4C 8D 0D ? ? ? ? C6 44 24 ? ? 4C 8D 45 6F", 16) - client_offset;

	if (!client_dll_base) {
		driver::close();
		return 1;
	}

	debug_print("client_dll_base: 0x%p", client_dll_base);
	normal_print("初始化已完成！");

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"External Overlay Class";


	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
		wc.lpszClassName,
		L"ESP",
		WS_POPUP,
		0,
		0,
		screenW,
		screenH,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	if (!window) {
		DWORD error = GetLastError();
		error_print("CreateWindowExW failed! Error: %lu", error);
		return 1;
	}


	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins
		{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;

	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;


	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	if (FAILED(hr)) {
		error_print("D3D11CreateDeviceAndSwapChain failed! HRESULT: 0x%X", hr);
		return 1;
	}

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else
	{
		return 1;
	}


	ShowWindow(window, SW_SHOW);
	
	if (!UpdateWindow(window))
	{
		DWORD err = GetLastError();
		error_print("UpdateWindow failed! Error: %lu", err);
	}

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simhei.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
	normal_print("字体已加载: simhei.ttf");

	if (!ImGui_ImplWin32_Init(window))
		error_print("ImGui_ImplWin32_Init failed!");
	if (!ImGui_ImplDX11_Init(device, device_context))
		error_print("ImGui_ImplDX11_Init failed!");
	normal_print("ImGui已初始化");

	std::thread refresh_thread_instance(refresh_thread);
	std::thread triggerbot_thread_instance(triggerbot);
	normal_print("线程已创建");

	bool running = true;
	bool rendering = false;
	bool radar_hack = false;
	bool render_friend = false;
	int players_count = 0;

	normal_print(FOREGROUND_GREEN, "去作弊Go2Cheat已准备完成");
	while (running && utils::process::is_process_running(pid))
	{
		{
			std::lock_guard<std::mutex> lock(players_mutex);
			if (players_count != utils::Players::players.size())
			{
				players_count = utils::Players::players.size();
				normal_print(FOREGROUND_RED | FOREGROUND_GREEN, "玩家数量: %d", players_count);
			}
		}
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
		{
			if (!rendering)
			{
				rendering = !rendering;
			}
			else if (!render_friend) {
				render_friend = !render_friend;
			}
			else {
				rendering = !rendering;
				render_friend = !render_friend;
			}
			normal_print("ESP渲染已%s, 队友渲染已%s", rendering ? "开启" : "关闭", render_friend ? "开启" : "关闭");
		}
		if (GetAsyncKeyState(VK_HOME) & 1) {
			radar_hack = !radar_hack;
			normal_print("雷达透视已%s", radar_hack ? "开启" : "关闭");
		}
		if (GetAsyncKeyState(VK_END) & 1)
		{
			running = false;
		}

		if (!running)
		{
			break;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		//ImGui::GetBackgroundDrawList()->AddCircle(ImVec2(200, 200), 50.0f, IM_COL32(255, 0, 0, 255), 0, 2.0f);

		{
			std::lock_guard<std::mutex> lock(players_mutex);

			if (radar_hack) {
				features::radarHack::set_spotted_state();
			}

			if (rendering) {
				features::esp::draw_esp(render_friend);
			}
		}

		features::esp::C4_info();

		ImGui::Render();

		constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(0U, 0U);
	}

	error_print("正在退出...");
	keep_running = false;
	if (refresh_thread_instance.joinable())
	{
		refresh_thread_instance.join();
	}
	if (triggerbot_thread_instance.joinable())
	{
		triggerbot_thread_instance.join();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();


	if (swap_chain)
	{
		swap_chain->Release();
	}

	if (device_context)
	{
		device_context->Release();
	}

	if (render_target_view)
	{
		render_target_view->Release();
	}

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	driver::close();
	error_print("已退出，请关闭本窗口");
	FreeConsole();
	//FreeLibraryAndExitThread(instance, 0);
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hinstDLL);
		
		//CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Main, hinstDLL, 0, nullptr);
	}
	return TRUE;
}

#endif
