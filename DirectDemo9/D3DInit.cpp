#include "D3DInit.h"

//用于初始化Direct3D设备
HRESULT InitD3D(IDirect3D9 **ppD3D, IDirect3DDevice9 **ppD3DDevice, D3DPRESENT_PARAMETERS* d3pp, HWND hWnd, BOOL ForceWindowed /* = FALSE */, BOOL MultiThreaded /* = FALSE */)
{
	LPDIRECT3D9 pD3D=NULL;
	LPDIRECT3DDEVICE9 pD3Ddevice = NULL;
	HRESULT hr;
	if (!ppD3D || !ppD3DDevice || !hWnd)
		return E_FAIL;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;
	*ppD3D = pD3D;
	DWORD Mode;
	if (ForceWindowed == TRUE)
		Mode = IDNO;
	else
		Mode = MessageBox(hWnd, L"Use fullscreen mode? (640x480x16)", L"Initialize D3D", MB_YESNO | MB_ICONQUESTION);
	D3DPRESENT_PARAMETERS d3dpp;
	::ZeroMemory(&d3dpp,sizeof(d3dpp));
	if (Mode == IDYES)
	{
		D3DFORMAT Format = D3DFMT_R5G6B5;
		d3dpp.BackBufferWidth = WINDOW_WIDTH;
		d3dpp.BackBufferHeight = WINDOW_HEIGHT;
		d3dpp.BackBufferFormat = Format;
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		d3dpp.Windowed = FALSE;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}
	else
	{
		RECT ClientRect, WndRect;
		GetClientRect(hWnd, &ClientRect);
		GetWindowRect(hWnd, &WndRect);
		DWORD Width = (WndRect.right - WndRect.left) + (WINDOW_WIDTH - ClientRect.right);
		DWORD Height = (WndRect.bottom - WndRect.top) + (WINDOW_HEIGHT - ClientRect.bottom);
		MoveWindow(hWnd, WndRect.left, WndRect.top,Width, Height, TRUE);
		D3DDISPLAYMODE d3ddm;
		pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
		d3dpp.BackBufferHeight = WINDOW_HEIGHT;
		d3dpp.BackBufferWidth = WINDOW_WIDTH;
		d3dpp.BackBufferFormat = d3ddm.Format;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.Windowed = TRUE;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.PresentationInterval = D3DPRESENT_RATE_DEFAULT;
	}
	d3pp = &d3dpp;
	DWORD Flags;
	D3DCAPS9 caps;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
		return E_FAIL;
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		Flags= D3DCREATE_HARDWARE_VERTEXPROCESSING;//支持硬件顶点运算
	else
		Flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;//不支持硬件顶点运算
	if (MultiThreaded == TRUE)
		Flags |= D3DCREATE_MULTITHREADED;
	if (FAILED(hr = pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL, hWnd, Flags,
		&d3dpp, &pD3Ddevice)))
		return hr;
	*ppD3DDevice = pD3Ddevice;
    //进行其他的设置	 //投影矩阵的设置
	float Aspect = (float)d3dpp.BackBufferWidth / (float)d3dpp.BackBufferHeight;
	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f, Aspect, 1.0f, 10000.0f);
	pD3Ddevice->SetTransform(D3DTS_PROJECTION,&matProjection);
	//设置默认渲染状态
	pD3Ddevice->SetRenderState(D3DRS_LIGHTING, false);
	pD3Ddevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	pD3Ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pD3Ddevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//设置材质
	return S_OK;
}