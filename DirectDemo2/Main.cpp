#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <tchar.h>

#include "atlimage.h" 

#pragma comment(lib,"winmm.lib")  //����PlaySound����������ļ�

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_CLASS L"WndClass"
#define WINDOW_TITLE L"DirectWnd"

#define SAFE_RELEASE(p)  {if(p){(p)->Release();(p)=NULL;}}

//����ȫ�ֵ��豸����
LPDIRECT3DDEVICE9   g_pD3Ddevice = NULL;
ID3DXFont * g_pfont = NULL;
float g_FPS = 0.0f;
WCHAR g_strFPS[50];

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HRESULT    Direct3D_Init(HWND hWnd);
HRESULT    Objects_Init(HWND hWnd);
VOID       Direct3D_Render(HWND hWnd);
VOID       Direct3D_CleanUp();
float      GetFPS();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass={0};
	wndClass.cbSize = sizeof(WNDCLASSEX);			                 //���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	                     //���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					                 //����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;								         //������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							             //���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						             //ָ���������ڹ��̵ĳ����ʵ�������
	//wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);                  //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);     //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						              //��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = WINDOW_CLASS;		                      //��һ���Կ���ֹ���ַ�����ָ������������֡�

	if (!RegisterClassEx(&wndClass))
		return -1;
	HWND hwnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (S_OK == Direct3D_Init(hwnd))
	{
		MessageBox(hwnd, L"Direct3D��ʼ�����~��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����  
	}	
	else
	{
		MessageBox(hwnd, L"Direct3D��ʼ��ʧ��~��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����  
	}

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow(hwnd, nShowCmd);                                         //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);		//�ú����ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			Direct3D_Render(hwnd);
		}
	}
	UnregisterClass(WINDOW_CLASS, hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case  WM_PAINT:
		Direct3D_Render(hwnd);                 //����Direct3D��Ⱦ����
		ValidateRect(hwnd, NULL);		       // ���¿ͻ�������ʾ
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);		      // ���ٴ���, ������һ��WM_DESTROY��Ϣ
		break;
	case WM_DESTROY:
		Direct3D_CleanUp();
		PostQuitMessage(0);
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
}

HRESULT    Direct3D_Init(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ��һ�������ӿڡ�������Direct3D�ӿڶ���, �Ա��ø�Direct3D���󴴽�Direct3D�豸����
	//--------------------------------------------------------------------------------------
	LPDIRECT3D9  pD3D = NULL; //Direct3D�ӿڶ���Ĵ���
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION))) //��ʼ��Direct3D�ӿڶ��󣬲�����DirectX�汾Э��
		return E_FAIL;
	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮��,ȡ��Ϣ������ȡӲ���豸��Ϣ
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{											
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //֧��Ӳ���������㣬���ǾͲ���Ӳ���������� 
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING; //��֧��Ӳ���������㣬����ֻ�ò��������������
	//���������Ĵ�����
	//DXGI_SWAP_CHAIN_DESC swapChainDesc;
	
	
	
	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮���������ݡ������D3DPRESENT_PARAMETERS�ṹ��
	//--------------------------------------------------------------------------------------
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;	//��Ⱦ��̨����ĸ�ʽ
	d3dpp.BackBufferCount = 1;				    //��Ҫ������Ⱦ�ĺ�̨��������
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //������Ч��
	d3dpp.hDeviceWindow = hwnd;				  //WinMain�д����Ĵ��ھ��
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	//--------------------------------------------------------------------------------------
	// ��Direct3D��ʼ���Ĳ���֮�ģ����豸��������Direct3D�豸�ӿ�
	//--------------------------------------------------------------------------------------
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, vp, &d3dpp, &g_pD3Ddevice)))
		return E_FAIL;
	SAFE_RELEASE(pD3D) //LPDIRECT3D9�ӿڶ����ʹ����ɣ����ǽ����ͷŵ�
	if (!(S_OK == Objects_Init(hwnd))) return E_FAIL;     //����һ��Objects_Init��������Ⱦ��Դ�ĳ�ʼ��
	return S_OK;
}
HRESULT    Objects_Init(HWND hWnd)
{
	//��������
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pfont)))
		return E_FAIL;
	srand(timeGetTime());      //��ϵͳʱ���ʼ��������� 
	return S_OK;
}
VOID       Direct3D_Render(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮һ������������
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮��������ʼ����
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->BeginScene();                     // ��ʼ����
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮��������ʽ���ƣ�����������д���Ķ�����
	//--------------------------------------------------------------------------------------
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.3f"), GetFPS());
	g_pfont->DrawText(NULL, g_strFPS, charCount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 0, 255));
	//��������100����д��һ������
	formatRect.top = 100;//ָ�����ֵ�������
	g_pfont->DrawText(0, _T("������������Ϩ�����Ϸ�������롿"), -1, &formatRect, DT_CENTER,
		D3DCOLOR_XRGB(68, 139, 256));
	formatRect.top = 200;
	g_pfont->DrawText(NULL, _T("HELLO WORLD"), -1, &formatRect, DT_CENTER, D3DCOLOR_XRGB(200, rand() % 255, 20));
	formatRect.top = 400;
	g_pfont->DrawText(0, _T("�������˰�"), -1, &formatRect, DT_CENTER,
		D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256));
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�ġ�����������
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->EndScene();                       // ��������
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�塿����ʾ��ת
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Present(NULL, NULL, NULL, NULL);  // ��ת����ʾ
}
VOID       Direct3D_CleanUp()
{
	//�ͷ�COM����
	SAFE_RELEASE(g_pfont);
	SAFE_RELEASE(g_pD3Ddevice);
}
float      GetFPS()
{
	static float fps = 0;
	static float frameCount = 0;
	static float currentTime = 0.0f;
	static float lastTime = 0.0f;
	frameCount++;//ÿ����һ��Get_FPS()������֡������1
	currentTime = timeGetTime()*0.001f;
	if (currentTime - lastTime > 1.0f)
	{
		fps = (float)frameCount;
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}