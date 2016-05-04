#include<d3d9.h>  
#define WINDOW_CLASS L"UGPDX"  
#define WINDOW_NAME L"Blank D3D Window"  
bool InitializeD3D(HWND hWnd, bool fullscreen);
bool InitialObjects();
void RenderScene();
void Shutdown();
LPDIRECT3D9 g_D3D = NULL;
LPDIRECT3DDEVICE9 g_D3DDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer = NULL;//顶点缓存
struct stD3DVertex 
{
	float x, y, z, rhw;
	DWORD color;
};
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE prevhInst, LPSTR cmdLine, int show)
{
	// Register the window class  
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		WINDOW_CLASS, NULL };
	RegisterClassEx(&wc);
	// Create the application's window  
	HWND hWnd = CreateWindow(WINDOW_CLASS, WINDOW_NAME, WS_OVERLAPPEDWINDOW,
		100, 100, 640, 480, GetDesktopWindow(), NULL,
		wc.hInstance, NULL);
	// Initialize Direct3D  
	if (InitializeD3D(hWnd, false))
	{
		// Show the window  
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);
		// Enter the message loop  
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				RenderScene();
		}
	}
	// Release any and all resources.  
	Shutdown();
	// Unregister our window.  
	UnregisterClass(WINDOW_CLASS, wc.hInstance);
	return 0;
}
/*
typedef struct _D3DPRESENT_PARAMETERS_
{
UINT                BackBufferWidth;
UINT                BackBufferHeight;
D3DFORMAT           BackBufferFormat;
UINT                BackBufferCount;

D3DMULTISAMPLE_TYPE MultiSampleType;
DWORD               MultiSampleQuality;

D3DSWAPEFFECT       SwapEffect;
HWND                hDeviceWindow;
BOOL                Windowed;
BOOL                EnableAutoDepthStencil;
D3DFORMAT           AutoDepthStencilFormat;
DWORD               Flags;

FullScreen_RefreshRateInHz must be zero for Windowed mode 
UINT                FullScreen_RefreshRateInHz;
UINT                PresentationInterval;	//可以显示的交换链的后台缓存的最大次数
} D3DPRESENT_PARAMETERS;
*/
/*HRESULT CreateDevice( UINT Adapter, //该参数用于指定正在使用的显卡
D3DDEVTYPE DeviceType,	//用于指定Direct3D中的渲染方式
HWND hFocusWindow,
DWORD BehaviorFlags,
D3DPRESENT_PARAMETERS *pPresentationParameters,
IDirect3DDevice9 **ppReturnedDeviceInterface
);*/
bool InitializeD3D(HWND hWnd, bool fullscreen)
{
	D3DDISPLAYMODE displayMode;//一个显示结构体
	// Create the D3D object.  
	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);  //创建一个Direct3D接口对象
	if (g_D3D == NULL) return false;
	// Get the desktop display mode.  
	if (FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)))
		return false; 
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	if (fullscreen)
	{
		d3dpp.Windowed = FALSE;
		d3dpp.BackBufferWidth = 640;
		d3dpp.BackBufferHeight = 480;
	}
	else
		d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = displayMode.Format;
	// Create the D3DDevice  
	if (FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice)))
	{
		return false;
	}
	if (!InitialObjects())return false;
	return true;
}
bool InitialObjects()
{
	DWORD col = D3DCOLOR_XRGB(255, 255, 255);
	stD3DVertex objData[] =
	{
		{ 420.0f, 150.0f, 0.5f,1.0f, col, },
		{ 420.0f, 350.0f, 0.5f,1.0f, col, },
		{ 220.0f, 150.0f, 0.5f,1.0f, col, },
		{ 220.0f, 350.0f, 0.5f,1.0f, col, },
	};
	if (FAILED(g_D3DDevice->CreateVertexBuffer(sizeof(objData), 0,
		D3DFVF_VERTEX, D3DPOOL_DEFAULT, &g_VertexBuffer,
		NULL))) return false;
	void * ptr;
	/*Lock()函数将锁定一列数据，这样就可以处理该数据了。该函数的参数包括：
	为上锁的顶点缓存增加的偏移量(0代表锁定全部缓存)、要锁定的数据量、
	指向锁定内存区的指针以及一个指定锁存数据方法的标识符*/
	if (FAILED(g_VertexBuffer->Lock(0, sizeof(objData),
		(void**)&ptr, 0))) return false;
	g_VertexBuffer->Unlock();
	memcpy(ptr, objData, sizeof(objData));
	return true;
}
void RenderScene()
{
	// Clear the backbuffer.  
	g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	// Begin the scene. Start rendering.  
	g_D3DDevice->BeginScene();
	g_D3DDevice->SetStreamSource(0, g_VertexBuffer, 0, sizeof(stD3DVertex));
	g_D3DDevice->SetFVF(D3DFVF_VERTEX);
	g_D3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, 2);
	// End the scene. Stop rendering.  
	g_D3DDevice->EndScene();
	// Display the scene.  
	g_D3DDevice->Present(NULL, NULL, NULL, NULL);
}
void Shutdown()
{
	if (g_D3DDevice != NULL) g_D3DDevice->Release();
	if (g_D3D != NULL) g_D3D->Release();
	if (g_VertexBuffer != NULL) g_VertexBuffer->Release();
	g_D3DDevice = NULL;
	g_D3D = NULL;
	g_VertexBuffer = NULL;
}