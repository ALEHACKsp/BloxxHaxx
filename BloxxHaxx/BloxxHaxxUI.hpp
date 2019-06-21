#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <Psapi.h>
#include <vector>
#include <map>
#include <fstream>
#include <tchar.h>
#include <sstream>

#include <GL/gl.h>
#include <GL/glu.h>

#include <d3d11.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>

#include "MinHook/MinHook.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl2.h"
#include "ImGUI/imgui_impl_win32.h"

#include "FW1FontWrapper\FW1FontWrapper.h"


#include "Cheats/CheatIncludes.hpp"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")


IFW1Factory* Bloxx_FW1FontFactory;
IFW1FontWrapper* Bloxx_FW1FontWrapper;

ID3D11Device* D3D11Device;
ID3D11DeviceContext* D3D11Context;
ID3D11Texture2D* D3D11TextureTarget;
ID3D11RenderTargetView* D3D11RenderView;
IDXGISwapChain* D3D11SwapChain;

DWORD_PTR* D3D11SwapChain_VTABLE;
DWORD_PTR* D3D11Device_VTABLE;
DWORD_PTR* D3D11Context_VTABLE;



bool UIIsCurrentlyOpen;

std::hash<std::string> HashString;

HMODULE BloxxHaxxMod;

HGLRC LastRC = NULL;
std::map<int, HGLRC> PixelFormatLut;

void(*Orig_SwapBuffers)(HDC dc) = nullptr;

template <typename T>
inline MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

long ReturnRGBValue(int iR, int iG, int iB, int iA) {

	return ((iA * 256 + iR) * 256 + iG) * 256 + iB;
}

#define RGB_GET(iR, iG, iB, iA) (ReturnRGBValue(iR, iG, iB, iA))
#define RGB_GET(iR, iG, iB) (ReturnRGBValue(iR, iG, iB, 255))




void OpenGLDraw(RECT Rect) {

	auto& IO = ImGui::GetIO();
	IO.DisplaySize.x = Rect.right - Rect.left;
	IO.DisplaySize.y = Rect.bottom - Rect.top;

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplOpenGL2_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 100, 100 });

	if (ImGui::Begin("BloxxHaxx - Developed By Ringarang")) {
		ImGui::SetWindowSize(ImVec2(545, 351));

		ImGui::Text("All Cheats Are Listed Below: ");

		if (AutoClick_Enabled) {
			ImGui::TextColored(ImVec4(0, 255, 0, 255), "[*] AutoClick - F1 - Enabled [*]");
		}
		else {
			ImGui::TextColored(ImVec4(255, 0, 0, 255), "[*] AutoClick - F1 - Disabled [*]");
		}













	}

	ImGui::PopStyleVar();

	ImGui::Render();

	glClear(256);
	glMatrixMode(5889);
	glOrtho(0.0f, (int)IO.DisplaySize.x, (int)IO.DisplaySize.y, 0.0f, 0.0f, 100.0f);
	glViewport(0, 0, (int)IO.DisplaySize.x, (int)IO.DisplaySize.y);

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}


static void SwapOpenGLBuffers(HDC DC) {
	if (!DC) {
		return Orig_SwapBuffers(DC);
	}

	if (UIIsCurrentlyOpen) {
		HWND const WND = WindowFromDC(DC);
		RECT WNDRect;
		GetClientRect(WND, &WNDRect);

		OpenGLDraw(WNDRect);
	}

	return Orig_SwapBuffers(DC);
}



typedef LRESULT(CALLBACK*OWindowProc) (
	_In_ HWND   hwnd,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
OWindowProc WNDProc = nullptr;

bool TabBackAutoClick;

static LRESULT WINAPI HWNDProcHandle(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {

	DWORD Proc;
	auto& IO = ImGui::GetIO();

	if (UIIsCurrentlyOpen) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam)) {
			return TRUE;
		}
	}

	switch (Msg) {
	case WM_KEYUP: {
		if (wParam == VK_INSERT)
		{
			UIIsCurrentlyOpen =! UIIsCurrentlyOpen;
			if (UIIsCurrentlyOpen) {
				TabBackAutoClick = true;
				ReleaseCapture();
			}
			if (!UIIsCurrentlyOpen) {
				if (TabBackAutoClick) {
					//use this to auto click back into game upon UI closing so you can look around again without having to click
					INPUT Input = { 0 };

					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					SendInput(1, &Input, sizeof(INPUT));

					ZeroMemory(&Input, sizeof(INPUT));
					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					SendInput(1, &Input, sizeof(INPUT));

					TabBackAutoClick = false;
				}
			}
		}
		if (wParam == VK_F1) {
			AutoClick_Enabled =!AutoClick_Enabled;
			if (AutoClick_Enabled) {
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AutoClick_Cheat, 0, 0, 0);
			}
		}
	}
	case WM_SETCURSOR:
	{
		if (UIIsCurrentlyOpen) {
			return 0;
		}
	}
	}
ret:
	return CallWindowProc(WNDProc, hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }



void InitGUI() {
	ImGui::CreateContext();

	auto MineCraftWindow = FindWindow(_T("LWJGL"), NULL);
	ImGui_ImplWin32_Init(MineCraftWindow);
	ImGui_ImplOpenGL2_Init();


	WNDProc = (OWindowProc)SetWindowLongPtr(MineCraftWindow, GWLP_WNDPROC, (LONG_PTR)HWNDProcHandle);

	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->WindowRounding = 0;
	Style->WindowTitleAlign = ImVec2(0.01, 0.5);
	Style->GrabRounding = 1;
	Style->GrabMinSize = 20;
	Style->FrameRounding = 0;

	if (MH_Initialize() != MH_OK) {
		MessageBox(NULL, _T("Unable To Initiate BloxxHaxx UI! Error Code: 1"), _T("Fatal Error!"), MB_OK | MB_ICONERROR);
		exit(0);
	}

	if (MH_CreateHookApiEx(L"Gdi32.dll", "SwapBuffers", &SwapOpenGLBuffers, &Orig_SwapBuffers) != MH_OK) {
		MessageBox(NULL, _T("Unable To Initiate BloxxHaxx UI! Error Code: 2"), _T("Fatal Error!"), MB_OK | MB_ICONERROR);
		exit(0);
	}

	if (MH_EnableHook(MH_ALL_HOOKS)) {
		MessageBox(NULL, _T("Unable To Initiate BloxxHaxx UI! Error Code: 3"), _T("Fatal Error!"), MB_OK | MB_ICONERROR);
		exit(0);
	}
}