#include "Pch.h"
#include "Common.h"
#include "resource.h"

vec2i g_view_size(1280, 720);
float g_view_aspect = 1.0f;

HWND gMainWnd;
extern const wchar_t* gAppName;

IDirect3D9* gD3d;
IDirect3DDevice9* gDevice;

bool gHasFocus;
int gKey;
DWORD gKeyDown[KEY_MAX];
vec2i gMousePos;
int gMouseButtons;

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
	RECT rc;
	GetClientRect(gMainWnd, &rc);

	g_view_size.x = max<int>(rc.right - rc.left, 16);
	g_view_size.y = max<int>(rc.bottom - rc.top, 16);
	g_view_aspect = (float)g_view_size.x / (float)g_view_size.y;

	pp->Windowed				= TRUE;
	pp->SwapEffect				= D3DSWAPEFFECT_DISCARD;
	pp->BackBufferWidth			= g_view_size.x;
	pp->BackBufferHeight		= g_view_size.y;
	pp->BackBufferFormat		= D3DFMT_A8R8G8B8;
	pp->hDeviceWindow			= gMainWnd;
	pp->PresentationInterval	= D3DPRESENT_INTERVAL_ONE;
}

void ResetDevice() {
	if (gDevice) {
		D3DPRESENT_PARAMETERS pp = { };
		GetPresentParams(&pp);
		gDevice->Reset(&pp);
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

void DoFrame() {
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

	if (gDevice) {
		gDevice->BeginScene();

		void RenderPreUpdate();
		RenderPreUpdate();

		void game_update();
		game_update();

		void game_render();
		game_render();

		void frame_render();
		frame_render();

		gKey = 0;

		gDevice->EndScene();
		gDevice->Present(0, 0, 0, 0);
	}
}

int which_key(int c, bool shifted) {
	switch(c) {
		case VK_UP:		return KEY_UP;
		case VK_DOWN:	return KEY_DOWN;
		case VK_LEFT:	return KEY_LEFT;
		case VK_RIGHT:	return KEY_RIGHT;
		case VK_NUMPAD8:return KEY_UP;
		case VK_NUMPAD2:return KEY_DOWN;
		case VK_NUMPAD4:return KEY_LEFT;
		case VK_NUMPAD6:return KEY_RIGHT;
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

	if (c == g_LocKeyW) return KEY_UP;
	if (c == g_LocKeyS) return KEY_DOWN;
	if (c == g_LocKeyA) return KEY_LEFT;
	if (c == g_LocKeyD) return KEY_RIGHT;

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
			PostQuitMessage(0);
		return 0;

		case WM_SIZE:
			RenderShutdown();
			GpuShutdown();

			ResetDevice();

			GpuInit();
			RenderInit();

			DoFrame();

			if (gHasFocus) {
				RECT rc;
				GetClientRect(hwnd, &rc);
				MapWindowPoints(hwnd, 0, (POINT*)&rc, 2);
				ClipCursor(&rc);
			}
		return 0;

		case WM_ACTIVATE:
			gHasFocus = wparam != WA_INACTIVE;

			if (!gHasFocus) {
				memset(gKeyDown, 0, sizeof(gKeyDown));
			}

			if (gHasFocus) {
				RECT rc;
				GetClientRect(hwnd, &rc);
				MapWindowPoints(hwnd, 0, (POINT*)&rc, 2);
				ClipCursor(&rc);
			}
		break;

		// Keys

		case WM_KEYDOWN: {
			int got_key = which_key(LOWORD(wparam), (GetKeyState(VK_SHIFT) & 0x8000) != 0);

			if (got_key) {
				if (!gKeyDown[got_key]) {
					gKey = got_key;
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
				PostQuitMessage(0);
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
	RECT	rcWin	= { 0, 0, g_view_size.x, g_view_size.y };

	RECT rcDesk;
	GetClientRect(GetDesktopWindow(), &rcDesk);

	AdjustWindowRectEx(&rcWin, style, FALSE, styleEx);
	OffsetRect(&rcWin, ((rcDesk.right - rcDesk.left) - g_view_size.x) / 2, ((rcDesk.bottom - rcDesk.top) - g_view_size.y) / 2);

	gMainWnd = CreateWindowEx(styleEx, wc.lpszClassName, gAppName, style, rcWin.left, rcWin.top, rcWin.right - rcWin.left, rcWin.bottom - rcWin.top, 0, 0, 0, 0);

	// Graphics

	if ((gD3d = Direct3DCreate9(D3D_SDK_VERSION)) == 0) {
		panic("D3DCreate failed - do you have D3D9 installed?");
	}

	D3DPRESENT_PARAMETERS pp = { };

	GetPresentParams(&pp);

	if (FAILED(gD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, gMainWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &gDevice))) {
		panic("D3D CreateDevice failed - do you have D3D9 installed?");
	}

	GpuInit();

	// Keys

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

	// Main

	RenderInit();

	void game_init();
	game_init();

	ShowWindow(gMainWnd, SW_SHOWNORMAL);

	// Audio
	
	SoundInit();

	for(;;) {
		MSG msg;
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			DoFrame();
			if (!gHasFocus) Sleep(250);
		}
	}

	ShowWindow(gMainWnd, SW_HIDE);
	ExitProcess(0);
	
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