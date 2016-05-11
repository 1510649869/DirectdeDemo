#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <tchar.h>
#pragma comment(lib,"winmm.lib")
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"QWnd"
#define WINDOW_CLASS L"WndClass"
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}
#define SAFE_DELETE(p)  {if(p){delete(p);(p)=NULL;}}

struct VERTEXPOSTEX
{
	float x, y, z;
	float u, v;
	VERTEXPOSTEX(float _x, float _y, float _z, float _u, float _v) :x(_x), y(_y), z(_z),
		u(_u), v(_v)
	{
	}
};
#define  D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
IDirect3DVertexShader9* DiffuseShader = 0;
ID3DXConstantTable*      DiffuseConstTable;
ID3DXEffect*      Deffect;
ID3DXMesh* Teapot = 0;
D3DXHANDLE ViewMatrixHandle = 0;
D3DXHANDLE ViewProjMatrixHandle = 0;
D3DXHANDLE AmbientMtrlHandle = 0;
D3DXHANDLE DiffuseMtrlHandle = 0;
D3DXHANDLE LightDirHandle = 0;
D3DXMATRIX Proj;

 
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


BOOL SetUp()
{
	HRESULT hr = 0;
	D3DXCreateTeapot(g_pd3dDevice, &Teapot, 0);
	ID3DXBuffer* shader = 0;
	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCompileShaderFromFile(
		L"diffuse.txt",
		0,
		0,
		"Main", //entry point function name 
		"vs_3_0",
		D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY|D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_DEBUG,
		&shader,
		&errorBuffer,
		&DiffuseConstTable);
	if (errorBuffer)
	{
		::MessageBox(0, L"(LPCWSTR)errorBuffer->GetBufferPointer()", 0, 0);
		SAFE_RELEASE(errorBuffer);
	}
	if (FAILED(hr)) {
		::MessageBox(0, L"D3DXCompileShaderFromFile() - FAILED", 0, 0);
		return false;
	}
	hr = g_pd3dDevice->CreateVertexShader((DWORD*)shader->GetBufferPointer(),
		&DiffuseShader);
	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexShader - FAILED", 0, 0);
		return false;
	}
	SAFE_RELEASE(shader);

	LightDirHandle       = DiffuseConstTable->GetConstantByName(0, "LightDirection");
	ViewMatrixHandle     = DiffuseConstTable->GetConstantByName(0, "ViewMatrix");
	ViewProjMatrixHandle = DiffuseConstTable->GetConstantByName(0, "ViewProjMatrix");
	AmbientMtrlHandle    = DiffuseConstTable->GetConstantByName(0, "AmbientMtrl");
	DiffuseMtrlHandle    = DiffuseConstTable->GetConstantByName(0, "DiffuseMtrl");

	D3DXVECTOR4 directionToLight(-0.57f, 0.57f, -0.57f, 0.0f);
	DiffuseConstTable->SetVector(g_pd3dDevice, LightDirHandle, &directionToLight);
	D3DXVECTOR4 ambientMtrl(0.4f, 0.0f, 0.0f, 1.0f);
	D3DXVECTOR4 diffuseMtrl(1.0f, 0.0f, 0.0f, 1.0f);
	DiffuseConstTable->SetVector(g_pd3dDevice, AmbientMtrlHandle, &ambientMtrl);
	DiffuseConstTable->SetVector(g_pd3dDevice, DiffuseMtrlHandle, &diffuseMtrl);

	DiffuseConstTable->SetDefaults(g_pd3dDevice);

	D3DXMatrixPerspectiveFovLH(&Proj, D3DX_PI*0.25f,
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 1000.0f);
	return true;
}

HRESULT D3DDeviceInit(HWND hwnd)
{
	LPDIRECT3D9 pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;
	D3DCAPS9 caps;
	int flag = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
		return E_FAIL;
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		flag = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		flag = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;	//渲染后台缓存的格式
	d3dpp.BackBufferCount = 1;				    //想要用于渲染的后台缓存总数
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //处理交换效果
	d3dpp.hDeviceWindow = hwnd;				  //WinMain中创建的窗口句柄
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, flag, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;
	SAFE_RELEASE(pD3D);//释放掉Direct3D对象
	
	return TRUE;
}


void Render(HWND hwnd)
{
	D3DXVECTOR3 vEye(0.0f, 0.0f, -7.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &vEye, &vAt, &vUp);//设置相机位置
	DiffuseConstTable->SetMatrix(g_pd3dDevice, ViewMatrixHandle, &V);
	D3DXMATRIX viewProj = V*Proj;
	DiffuseConstTable->SetMatrix(g_pd3dDevice, ViewProjMatrixHandle, &viewProj);


	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(10, 50, 30), 1.0f, 0);
	g_pd3dDevice->BeginScene();
    g_pd3dDevice->SetVertexShader(DiffuseShader);
	Teapot->DrawSubset(0);
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
void Destory()
{
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(Teapot);
	SAFE_RELEASE(DiffuseShader);
	SAFE_RELEASE(DiffuseConstTable);
}
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	WNDCLASSEX WndClass = { 0 };
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = WINDOW_CLASS;
	if (!RegisterClassEx(&WndClass))
		return -1;
	HWND hwnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
	if (E_FAIL == D3DDeviceInit(hwnd))
	{
		MessageBox(hwnd, L"ERROR:Can't Init The Device!", NULL, 1);
		return 0;
	}

	if (!SetUp())return 0;
	

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//Update(hwnd);
			Render(hwnd);
		}
	}
	UnregisterClass(WINDOW_CLASS, hInstance);
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		Destory();
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		Render(hWnd);
		ValidateRect(hWnd, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}