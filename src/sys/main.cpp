#include "Pch.h"
#include "Common.h"
#include "resource.h"

vec2i g_default_view_size(1280, 720);

vec2i g_view_size;
float g_view_aspect = 1.0f;

HWND gMainWnd;
extern const wchar_t* gAppName;

IDirect3D9* gD3d;
IDirect3DDevice9* gDevice;

bool g_quit;
bool g_capture_mouse = true;
bool g_mouse_is_captured;
bool g_win_in_menu;
bool g_win_in_size;

bool gHasFocus;
DWORD gKeyDown[KEY_MAX];
vec2i gMousePos;
int gMouseButtons;
int gMouseTime;
bool gUsingMouse;
vec2i gOldMousePos;

int g_LocKeyW;
int g_LocKeyS;
int g_LocKeyA;
int g_LocKeyD;

int g_LocKeyZ;
int g_LocKeyX;
int g_LocKeyC;

int g_LocKeyI;
int g_LocKeyO;
int g_LocKeyP;

vec2 gJoyLeft;
vec2 gJoyRight;
bool gJoyA;

bool is_key_down(key_press k) { return gKeyDown[k] != 0; }

void GetPresentParams(D3DPRESENT_PARAMETERS* pp) {
	pp->Windowed				= TRUE;
	pp->SwapEffect				= D3DSWAPEFFECT_DISCARD;
	pp->BackBufferWidth			= g_view_size.x;
	pp->BackBufferHeight		= g_view_size.y;
	pp->BackBufferFormat		= D3DFMT_A8R8G8B8;
	pp->hDeviceWindow			= gMainWnd;
	pp->PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT;
}

void ResetDevice() {
	debug("device reset");

	if (gDevice) {
		SendMessage(gMainWnd, WM_APP, 0, 0);
	}
}

vec2 do_stick(int ix, int iy) {
	vec2 d(ix / 32768.0f, iy / -32768.0f);
	float i = length(d);

	if (i < 0.2f)
		return vec2();

	float j = clamp((i - 0.2f) / 0.7f, 0.0f, 1.0f);

	return d * (square(j) / i);
}

bool update_win_size() {
	RECT rc;
	GetClientRect(gMainWnd, &rc);

	int w = max<int>(rc.right - rc.left, 16);
	int h = max<int>(rc.bottom - rc.top, 16);

	if ((w == g_view_size.x) && (h == g_view_size.y))
		return false;

	g_view_size = vec2i(w, h);
	g_view_aspect = (float)w / (float)h;

	return true;
}

void DoFrame() {
	if (update_win_size()) {
		RenderShutdown();
		GpuShutdown();

		update_win_size();
		ResetDevice();

		GpuInit();
		RenderInit();

		g_mouse_is_captured = false;
	}

	if (gHasFocus && g_capture_mouse && !g_win_in_menu && !g_win_in_size) {
		if (!g_mouse_is_captured) {
			RECT rc;
			GetClientRect(gMainWnd, &rc);
			MapWindowPoints(gMainWnd, 0, (POINT*)&rc, 2);
			ClipCursor(&rc);
			g_mouse_is_captured = true;
		}
	}
	else {
		if (g_mouse_is_captured) {
			ClipCursor(0);
			g_mouse_is_captured = false;
		}
	}

	static int stick_check_time = 0;

	if (--stick_check_time <= 0) {
		XINPUT_STATE state;

		if (XInputGetState(0, &state) == ERROR_SUCCESS) {
			gJoyLeft			= do_stick(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
			gJoyRight			= do_stick(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);
			gJoyA				= (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
			stick_check_time	= 0;
		}
		else {
			gJoyLeft			= vec2();
			gJoyRight			= vec2();
			gJoyA				= false;
			stick_check_time	= 60;
		}
	}

	if (gOldMousePos == gMousePos) {
		if (++gMouseTime > 60) {
			if ((length_sq(gJoyRight) > 0.2) || is_key_down(KEY_R_LEFT) || is_key_down(KEY_R_RIGHT) || is_key_down(KEY_R_UP) || is_key_down(KEY_R_DOWN))
				gUsingMouse = false;
		}
		else
			gUsingMouse = true;
	}
	else {
		gUsingMouse = true;
		gOldMousePos = gMousePos;
		gMouseTime = 0;
	}

	if (gDevice) {
		static u64 t_begin = timer_ticks();

		gDevice->BeginScene();

		void RenderPreUpdate();
		RenderPreUpdate();

		void game_update();
		game_update();

		void game_render();
		game_render();

		void frame_render();
		frame_render();

		u64 t_mid = timer_ticks();

		gDevice->EndScene();
		gDevice->Present(0, 0, 0, 0);

		u64 t_end = timer_ticks();

		if (timer_ticks_to_secs(t_end - t_begin) > (1.1f / 60.0f)) {
			debug("%f : %f", timer_ticks_to_secs(t_end - t_begin) * 1000, timer_ticks_to_secs(t_mid - t_begin) * 1000);
		}

		t_begin = t_end;
	}
}

DWORD WINAPI GameLoop(void*) {
	while(!g_quit)
		DoFrame();

	PostMessage(gMainWnd, WM_QUIT, 0, 0);

	return 0;
}

int which_key(int c, bool shifted) {
	switch(c) {
		case VK_UP:		return KEY_R_UP;
		case VK_DOWN:	return KEY_R_DOWN;
		case VK_LEFT:	return KEY_R_LEFT;
		case VK_RIGHT:	return KEY_R_RIGHT;
		case VK_RETURN:	return KEY_FIRE;
		case VK_SPACE:	return KEY_ALT_FIRE;
		//case 'R':		return KEY_RESET;
		case VK_F1:		return KEY_MODE;
		case VK_F2:		return shifted ? KEY_CHEAT : KEY_NONE;
		case '0':		return KEY_0;
		case '1':		return KEY_1;
		case '2':		return KEY_2;
		case '3':		return KEY_3;
		case '4':		return KEY_4;
		case '5':		return KEY_5;
		case '6':		return KEY_6;
		case '7':		return KEY_7;
		case '8':		return KEY_8;
		case '9':		return KEY_9;
		case 27:		return KEY_RESET;
	}

	if (c == g_LocKeyW) return KEY_L_UP;
	if (c == g_LocKeyS) return KEY_L_DOWN;
	if (c == g_LocKeyA) return KEY_L_LEFT;
	if (c == g_LocKeyD) return KEY_L_RIGHT;

	if (c == g_LocKeyZ) return KEY_ALT_FIRE;
	if (c == g_LocKeyX) return KEY_FIRE;
	if (c == g_LocKeyC) return KEY_PLACE;

	if (c == g_LocKeyI) return KEY_ALT_FIRE;
	if (c == g_LocKeyO) return KEY_FIRE;
	if (c == g_LocKeyP) return KEY_PLACE;

	return KEY_NONE;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch(msg) {
		case WM_CLOSE:
			g_quit = true;
		return 0;

		case WM_ACTIVATE:
			gHasFocus = wparam != WA_INACTIVE;

			if (!gHasFocus) {
				memset(gKeyDown, 0, sizeof(gKeyDown));
			}
		break;

		case WM_APP: {
			D3DPRESENT_PARAMETERS pp = { };
			GetPresentParams(&pp);
			gDevice->Reset(&pp); // oh d3d9 how I hate thee
		}
		break;

		case WM_ENTERMENULOOP: g_win_in_menu = true; break;
		case WM_EXITMENULOOP: g_win_in_menu = false; break;

		case WM_ENTERSIZEMOVE: g_win_in_size = true; break;
		case WM_EXITSIZEMOVE: g_win_in_size = false; break;

		// Keys

		case WM_KEYDOWN: {
			int got_key = which_key(LOWORD(wparam), (GetKeyState(VK_SHIFT) & 0x8000) != 0);

			if (got_key) {
				if (!gKeyDown[got_key]) {
					gKeyDown[got_key] = 1;
				}
			}
		}
		break;

		case WM_KEYUP:
			gKeyDown[which_key(LOWORD(wparam), false)] = 0;
			gKeyDown[which_key(LOWORD(wparam), true)] = 0;
		break;

		case WM_SYSKEYDOWN:
			if ((GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_F4) & 0x8000)) {
				g_quit = true;
			}
		return 0;

		// Mouse

		case WM_SETCURSOR:
			if (LOWORD(lparam) == HTCLIENT) {
				SetCursor(0);
				return 0;
			}
		break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			if (!gMouseButtons)
				SetCapture(hwnd);
			gMouseButtons |= 1;
			gMousePos = vec2i(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;

		case WM_LBUTTONUP:
			gMouseButtons &= ~1;
			gMousePos = vec2i(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			if (!gMouseButtons)
				ReleaseCapture();
		return 0;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
			if (!gMouseButtons)
				SetCapture(hwnd);
			gMouseButtons |= 2;
			gMousePos = vec2i(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;

		case WM_RBUTTONUP:
			gMouseButtons &= ~2;
			gMousePos = vec2i(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			if (!gMouseButtons)
				ReleaseCapture();
		return 0;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
			if (!gMouseButtons)
				SetCapture(hwnd);
			gMouseButtons |= 4;
			gMousePos = vec2i(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;

		case WM_MBUTTONUP:
			gMouseButtons &= ~4;
			gMousePos = vec2i(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
			if (!gMouseButtons)
				ReleaseCapture();
		return 0;

		case WM_MOUSEMOVE:
			gMousePos = vec2i(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;

		case WM_CAPTURECHANGED:
			gMouseButtons = 0;
			g_win_in_menu = false;
			g_win_in_size = false;
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// Window

	WNDCLASSEX wc = { sizeof(wc) };

	wc.lpszClassName = L"MainWnd";
	wc.lpfnWndProc = MainWndProc;
	wc.hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON));
	wc.hCursor = LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));

	RegisterClassEx(&wc);

	DWORD	style	= WS_OVERLAPPEDWINDOW;
	DWORD	styleEx = WS_EX_OVERLAPPEDWINDOW;
	RECT	rcWin	= { 0, 0, g_default_view_size.x, g_default_view_size.y };

	RECT rcDesk;
	GetClientRect(GetDesktopWindow(), &rcDesk);

	AdjustWindowRectEx(&rcWin, style, FALSE, styleEx);
	OffsetRect(&rcWin, ((rcDesk.right - rcDesk.left) - (rcWin.right - rcWin.left)) / 2, ((rcDesk.bottom - rcDesk.top) - (rcWin.bottom - rcWin.top)) / 2);

	gMainWnd = CreateWindowEx(styleEx, wc.lpszClassName, gAppName, style, rcWin.left, rcWin.top, rcWin.right - rcWin.left, rcWin.bottom - rcWin.top, 0, 0, 0, 0);

	// Graphics

	if ((gD3d = Direct3DCreate9(D3D_SDK_VERSION)) == 0) {
		panic("D3DCreate failed - do you have D3D9 installed?");
	}

	D3DPRESENT_PARAMETERS pp = { };

	update_win_size();
	GetPresentParams(&pp);

	if (FAILED(gD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, gMainWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &gDevice))) {
		panic("D3D CreateDevice failed - do you have D3D9 installed?");
	}

	GpuInit();

	// input

	g_LocKeyW = MapVirtualKey(0x11, MAPVK_VSC_TO_VK);
	g_LocKeyS = MapVirtualKey(0x1F, MAPVK_VSC_TO_VK);
	g_LocKeyA = MapVirtualKey(0x1E, MAPVK_VSC_TO_VK);
	g_LocKeyD = MapVirtualKey(0x20, MAPVK_VSC_TO_VK);

	g_LocKeyZ = MapVirtualKey(44, MAPVK_VSC_TO_VK);
	g_LocKeyX = MapVirtualKey(45, MAPVK_VSC_TO_VK);
	g_LocKeyC = MapVirtualKey(46, MAPVK_VSC_TO_VK);

	g_LocKeyI = MapVirtualKey(23, MAPVK_VSC_TO_VK);
	g_LocKeyO = MapVirtualKey(24, MAPVK_VSC_TO_VK);
	g_LocKeyP = MapVirtualKey(25, MAPVK_VSC_TO_VK);

	XInputEnable(TRUE);

	// game/render

	RenderInit();

	void game_init();
	game_init();

	ShowWindow(gMainWnd, SW_SHOWNORMAL);

	// game thread

	HANDLE hgl = CreateThread(0, 0, GameLoop, 0, 0, 0);

	// audio
	
	SoundInit();

	// main loop

	for(MSG msg; GetMessage(&msg, 0, 0, 0); ) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// make sure gameloop thread has stopped

	g_quit = true;
	WaitForSingleObject(hgl, INFINITE);
	CloseHandle(hgl);

	// shutdown

	ShowWindow(gMainWnd, SW_HIDE);

	gDevice->SetVertexDeclaration(0);
	gDevice->SetVertexShader(0);
	gDevice->SetPixelShader(0);
	gDevice->SetTexture(0, 0);

	SoundShutdown();

	RenderShutdown();

	gDevice->Release();
	gD3d->Release();

	DestroyWindow(gMainWnd);

	return 0;
}