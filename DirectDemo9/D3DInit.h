#pragma  once
#include "D3DUtil.h"
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
HRESULT InitD3D(IDirect3D9 **ppD3D,
	IDirect3DDevice9 **ppD3DDevice,
	D3DPRESENT_PARAMETERS* d3dpp,
	HWND hWnd, BOOL ForceWindowed = FALSE,
	BOOL MultiThreaded = FALSE);
