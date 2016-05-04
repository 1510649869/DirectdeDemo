/*���û�������Ҫ���ļ�
d3d9.lib
d3dx9d.lib
d3dx10d.lib
winmm.lib
comctl32.lib
DxErr.lib
dxguid.lib
*/

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <tchar.h>

#pragma comment(lib,"winmm.lib")  //����PlaySound����������ļ�

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_CLASS L"WndClass"
#define WINDOW_TITLE L"DirectWnd"

#define SAFE_RELEASE(p)  {if(p){(p)->Release();(p)=NULL;}}

//��ƶ����ʽ
struct CUSTOMVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};
//FVF  �����ʽ
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)  //FVF�����ʽ

//����ȫ�ֵ��豸����
LPDIRECT3DDEVICE9   g_pD3Ddevice = NULL;
ID3DXFont * g_pfont = NULL;
float g_FPS = 0.0f;
WCHAR g_strFPS[50];
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT    Direct3D_Init(HWND hWnd);
HRESULT    Objects_Init(HWND hWnd);
VOID       Direct3D_Render(HWND hWnd);
VOID       Direct3D_CleanUp();
float      GetFPS();



int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };
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
		//MessageBox(hwnd, L"Direct3D��ʼ�����~��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����  
	}
	else
	{
		MessageBox(hwnd, L"Direct3D��ʼ��ʧ��~��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����  
	}

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow(hwnd, nShowCmd);                                         //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);

	PlaySound(L"�����=�ѥ륹�Υ륷.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

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
	//D3DDISPLAYMODE d3ddm;
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
	srand(timeGetTime());      //��ϵͳʱ���ʼ��������� 
	//��������
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pfont)))
		return E_FAIL;
	//�������㻺��
	if (FAILED(g_pD3Ddevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX)* 18, 0,
		D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, 
		&g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}
	//������������
	if (FAILED(g_pD3Ddevice->CreateIndexBuffer(sizeof(WORD)* 48, 0,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT,
		&g_pIndexBuffer, NULL)))
	{
		return E_FAIL;
	}

	//���ʶ�������
	CUSTOMVERTEX Vertices[17];
	Vertices[0].x = 400;
	Vertices[0].y = 300;
	Vertices[0].z = 0.0f;
	Vertices[0].rhw = 1.0f;
	Vertices[0].color = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	for (int i = 0; i < 16; i++)
	{
		Vertices[i + 1].x = (float)(250 * sin(i*3.14159 / 8.0)) + 400;
		Vertices[i + 1].y = -(float)(250 * cos(i*3.14159 / 8.0)) + 300;
		Vertices[i + 1].z = 0.0f;
		Vertices[i + 1].rhw = 1.0f;
		Vertices[i + 1].color = D3DCOLOR_XRGB(rand() % 256, rand() % 256, rand() % 256);
	}


	VOID * pVertex;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(Vertices), (VOID**)&pVertex, 0)))
		return E_FAIL;
	memcpy(pVertex, Vertices, sizeof(Vertices));  //���ݿ�������Ҫ�������ڴ����ȥ
	g_pVertexBuffer->Unlock();
	
	//�������������
	WORD Indices[] = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 9, 0, 9, 10, 0, 10, 11, 0, 11, 12, 0, 12, 13, 0, 13, 14, 0, 14, 15, 0, 15, 16, 0, 16, 1 };

	//�����������
	WORD *pIndices = NULL;
	g_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);
	memcpy(pIndices, Indices, sizeof(Indices));
	g_pIndexBuffer->Unlock();

	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, FALSE);	//�ص����������������Ƿ�˳ʱ�룬������Ǹ������ζ�����ʾ�� 

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
	g_pD3Ddevice->BeginScene();    // ��ʼ����

	g_pD3Ddevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	//��ʽ����
	g_pD3Ddevice->SetStreamSource(0,g_pVertexBuffer,0,sizeof(CUSTOMVERTEX));
	g_pD3Ddevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	g_pD3Ddevice->SetIndices(g_pIndexBuffer);
	g_pD3Ddevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 17, 0, 16);
	//g_pD3Ddevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);

	int charcount=swprintf_s(g_strFPS, 20, L"FPS:%0.3f", GetFPS());
	g_pfont->DrawText(0, g_strFPS, charcount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 0));
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
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
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