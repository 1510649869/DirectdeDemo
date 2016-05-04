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
#include <dinput.h>
#include "DirectInputClass.h"
#include "CameraClass.h"
#include "TerrainClass.h"
#include "SkyBoxClass.h"
#include "ModelClass.h"

#pragma comment(lib,"winmm.lib")  //����PlaySound����������ļ�
#pragma comment(lib, "dinput8.lib") 

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_CLASS L"WndClass"
#define WINDOW_TITLE L"DirectWnd"

#define SAFE_RELEASE(p)  {if(p){(p)->Release();(p)=NULL;}}
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }


//����ȫ�ֵ��豸����
LPDIRECT3DDEVICE9    g_pD3Ddevice = NULL;
ID3DXFont *          g_pfont = NULL;
LPD3DXFONT           g_pTextAdaperName = NULL;  // �Կ���Ϣ��2D�ı� 
LPD3DXFONT           g_pTextHelper = NULL;
float g_FPS = 0.0f;
WCHAR g_strFPS[50];
WCHAR g_strAdapterName[60] = { 0 };//�����Կ����Ƶ��ַ�����

////DirectInput8��һЩ����
//LPDIRECTINPUT8        g_pDirectInput = NULL;
//LPDIRECTINPUTDEVICE8  g_pMouseDevice = NULL;
//DIMOUSESTATE          g_diMouseState = { 0 };
//LPDIRECTINPUTDEVICE8  g_pKeyboardDevice = NULL;
//char                  g_pKeyStateBuffer[256] = { 0 };
DInputClass * g_pDInput=NULL;
CameraClass * g_pCamera=NULL;
TerrainClass* g_pTerrain=NULL;
SkyBoxClass * g_pSkyBox=NULL;
ModelClass  * g_pModel = NULL;

LPDIRECT3DVERTEXBUFFER9     g_pVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9      g_pIndexBuffer = NULL;
LPDIRECT3DTEXTURE9          g_pTexture = NULL;

//������ļ���ȡ��Ϣ����
D3DXMATRIX					g_matWorld;          //�������
LPD3DXMESH					g_pMesh = NULL;      // ����Ķ���
D3DMATERIAL9*				g_pMaterials = NULL; // ����Ĳ�����Ϣ
LPDIRECT3DTEXTURE9*		    g_pTextures = NULL;  // �����������Ϣ
DWORD						g_dwNumMtrls = 0;    // ���ʵ���Ŀ

LRESULT                     CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL                        Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize);
HRESULT                     Direct3D_Init(HWND hWnd, HINSTANCE hInstance);
HRESULT                     Objects_Init(HWND hWnd);
VOID                        Direct3D_Render(HWND hWnd);
VOID                        Direct3D_Update(HWND hwnd);
VOID                        Direct3D_CleanUp();
VOID                        Matrix_Set();      //��װ���Ĵ�任�ĺ���
float                       GetFPS();

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

	if (S_OK != Direct3D_Init(hwnd, hInstance))
	{
		MessageBox(hwnd, L"Direct3D��ʼ��ʧ��~��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����  
	}

	//PlaySound(L"Music\\Eternal Love .wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow(hwnd, nShowCmd);                                         //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);

	g_pDInput = new DInputClass();
	g_pDInput->Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//PlaySound(L"�����=�ѥ륹�Υ륷.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
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
			Direct3D_Update(hwnd);
			Direct3D_Render(hwnd);
		}
	}
	UnregisterClass(WINDOW_CLASS, hInstance);
	return 0;
}

LRESULT     CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
HRESULT     Direct3D_Init(HWND hwnd, HINSTANCE hInstance)
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
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;   //��֧��Ӳ���������㣬����ֻ�ò��������������
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

	//��ȡ�Կ��ź�
	WCHAR TeamName[60] = L"��ǰ���Կ��ͺţ�";
	D3DADAPTER_IDENTIFIER9 adapter;
	pD3D->GetAdapterIdentifier(0, 0, &adapter);
	int len = MultiByteToWideChar(CP_ACP, 0, adapter.Description, -1, NULL, 0);	//�Կ��ͺ��Ѿ��������adapter.Description�� Ϊchar����
	MultiByteToWideChar(CP_ACP, 0, adapter.Description, -1, g_strAdapterName, len);
	wcscat_s(TeamName, g_strAdapterName);
	wcscpy_s(g_strAdapterName, TeamName);

	SAFE_RELEASE(pD3D) //LPDIRECT3D9�ӿڶ����ʹ����ɣ����ǽ����ͷŵ�
	if (!(S_OK == Objects_Init(hwnd))) return E_FAIL;     //����һ��Objects_Init��������Ⱦ��Դ�ĳ�ʼ��
	g_pD3Ddevice->SetRenderState(D3DRS_LIGHTING, FALSE);			  //�رչ���
	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//������������
	return S_OK;
}
HRESULT     Objects_Init(HWND hWnd)
{
	srand(timeGetTime());      //��ϵͳʱ���ʼ��������� 
	//��������
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pfont)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 25, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pTextAdaperName)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 25, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"), &g_pTextHelper)))
		return E_FAIL;
	//����������ͼ
	if (FAILED(D3DXCreateTextureFromFile(g_pD3Ddevice, L"pal5q.jpg", &g_pTexture)))
	{
		return E_FAIL;
	}
	//���ò���
	D3DMATERIAL9 mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_pD3Ddevice->SetMaterial(&mtrl);

	g_pModel = new ModelClass(g_pD3Ddevice);
	g_pModel->LoadModelFromFileX(L"loli.X");

	g_pCamera = new CameraClass(g_pD3Ddevice);
	g_pCamera->SetCameraPostion(&D3DXVECTOR3(0.0f, 1000.0f, -1200.0f));
	g_pCamera->SetTargetPostion(&D3DXVECTOR3(0.0f, 1000.0f, 0.0f));
	g_pCamera->SetViewMatrix();
	g_pCamera->SetProjMatrix();

	g_pTerrain = new TerrainClass(g_pD3Ddevice);
	g_pTerrain->LoadTerrainFromFile(L"Terrain\\heighmap.raw", L"Terrain\\wood.jpg");
	g_pTerrain->InitTerrain(300, 300, 300.0f, 1.0f);

	g_pSkyBox=new SkyBoxClass(g_pD3Ddevice);
	g_pSkyBox->LoadSkyTextureFromFiile(L"GameMedia\\frontaw2.jpg",
		L"GameMedia\\backaw2.jpg",
		L"GameMedia\\leftaw2.jpg",
		L"GameMedia\\rightaw2.jpg",
		L"GameMedia\\topaw2.jpg");
	g_pSkyBox->InitSkyBox(30000);

	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = D3DXCOLOR(0.0f, 0.5f, 0.7f, 1.0f);
	light.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular  = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	light.Direction = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	g_pD3Ddevice->SetLight(0, &light);
	g_pD3Ddevice->LightEnable(0, true);
	g_pD3Ddevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pD3Ddevice->SetRenderState(D3DRS_SPECULARENABLE,true);
	//������Ⱦ״̬
	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //������������  
	//g_pD3Ddevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //���û����� 

	return S_OK;
}
//VOID        Matrix_Set()
//{
//	//--------------------------------------------------------------------------------------
//	//���Ĵ�任֮һ��������任���������
//	//--------------------------------------------------------------------------------------
//
//
//	//--------------------------------------------------------------------------------------
//	//���Ĵ�任֮������ȡ���任���������
//	//--------------------------------------------------------------------------------------
//	D3DXMATRIX matView; //����һ������
//	D3DXVECTOR3 vEye(0.0f, 0, -250.0f);  //�������λ��
//	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //�۲���λ��
//	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//���ϵ�����
//	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����
//	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����
//
//	//--------------------------------------------------------------------------------------
//	//���Ĵ�任֮������ͶӰ�任���������	  //͸��ͶӰ
//	//--------------------------------------------------------------------------------------
//	D3DXMATRIX matProj;                                                        //����һ������
//	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //����ͶӰ�任����
//	g_pD3Ddevice->SetTransform(D3DTS_PROJECTION, &matProj);                    //����ͶӰ�任����
//
//	//--------------------------------------------------------------------------------------
//	//���Ĵ�任֮�ġ����ӿڱ任������
//	//--------------------------------------------------------------------------------------
//	D3DVIEWPORT9 vp; //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����
//	vp.X = 0;		//��ʾ�ӿ�����ڴ��ڵ�X����
//	vp.Y = 0;		//�ӿ���ԶԴ��ڵ�Y����
//	vp.Width = WINDOW_WIDTH;	//�ӿڵĿ��
//	vp.Height = WINDOW_HEIGHT; //�ӿڵĸ߶�
//	vp.MinZ = 0.0f; //�ӿ�����Ȼ����е���С���ֵ
//	vp.MaxZ = 1.0f;	//�ӿ�����Ȼ����е�������ֵ
//	g_pD3Ddevice->SetViewport(&vp); //�ӿڵ�����
//}
VOID        Direct3D_Update(HWND hwnd)
{
	//ʹ��DirectInput���ȡ����  
	g_pDInput->GetInput();
	// ��ȡ������Ϣ������������Ӧ�����ģʽ    
	if (g_pDInput->IsKeyDown(DIK_1))         // �����ּ�1�����£�����ʵ�����    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (g_pDInput->IsKeyDown(DIK_2))         // �����ּ�2�����£������߿����    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	// ��������������ƶ��ӽ�  
	if (g_pDInput->IsKeyDown(DIK_A))      g_pCamera->MoveAlongRightVec(-0.3f);
	if (g_pDInput->IsKeyDown(DIK_D))      g_pCamera->MoveAlongRightVec(0.3f);
	if (g_pDInput->IsKeyDown(DIK_W))      g_pCamera->MoveAlongLookVec(3.0f);
	if (g_pDInput->IsKeyDown(DIK_S))      g_pCamera->MoveAlongLookVec(-3.0f);
	if (g_pDInput->IsKeyDown(DIK_R))      g_pCamera->MoveAlongUpVec(0.3f);
	if (g_pDInput->IsKeyDown(DIK_F))      g_pCamera->MoveAlongUpVec(-0.3f);

	if (g_pDInput->IsKeyDown(DIK_Q))      g_pCamera->RotationLookVec(0.0003f);
	if (g_pDInput->IsKeyDown(DIK_E))      g_pCamera->RotationLookVec(-0.0003f);
	//���������������ת�ӽ�  
	if (g_pDInput->IsKeyDown(DIK_LEFT))   g_pCamera->RotationUpVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_RIGHT))  g_pCamera->RotationUpVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_UP))     g_pCamera->RotationRightVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_DOWN))   g_pCamera->RotationRightVec(0.003f);
 
	//������������������������ת  
	if (g_pDInput->IsMouseButtonDown(0))
	{

	    g_pCamera->RotationRightVec(g_pDInput->MouseDY() * 0.001f);
		//if (g_pDInput->MouseDX() < g_pDInput->MouseDX())		
	}
	if (g_pDInput->IsMouseButtonDown(1))
		g_pCamera->RotationUpVec(g_pDInput->MouseDX()* 0.001f);

	//�����ֿ��ƹ۲����������  
	static FLOAT fPosZ = 0.0f;
	fPosZ += g_pDInput->MouseDZ()*0.03f;
	//���㲢����ȡ���任����  
	D3DXMATRIX matView;
	g_pCamera->CalculateViewMatrix(&matView);
	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView);

	////����ȷ������任����浽g_matWorld�� 
	D3DXVECTOR3 lookat;
	g_pCamera->GetLookVector(&lookat);
	D3DXMatrixTranslation(&g_matWorld, 0, 1000, 0);

	POINT lt, rb;
	RECT rect;
	GetClientRect(hwnd,&rect);
	lt.x = rect.left;
	lt.y = rect.top;
	rb.x = rect.right;
	rb.y = rect.bottom;
	//��lt��rb�Ĵ�������ת��Ϊ��Ļ����  
	ClientToScreen(hwnd, &lt);
	ClientToScreen(hwnd, &rb);
	//����Ļ���������趨��������  
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	//����������ƶ�����  
	ClipCursor(&rect);
	ShowCursor(false);
}
VOID        Direct3D_Render(HWND hwnd)
{
	g_pD3Ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	g_pD3Ddevice->BeginScene();    // ��ʼ����
	D3DXMATRIX mScal, mRot2, mTrans, mFinal;   
	D3DXMatrixTranslation(&mTrans, 0.0f, 600.0f, 200.0f);
	D3DXMatrixScaling(&mScal, 3.0f, 3.0f, 3.0f);
	D3DXMatrixRotationY(&mRot2,D3DX_PI);
	mFinal = mScal*mTrans*mRot2;
	g_pModel->RenderModel(&mFinal);

	D3DXMATRIX pTerrMatWorld;
	D3DXMatrixIdentity(&pTerrMatWorld);
	D3DXMatrixTranslation(&pTerrMatWorld, 0, 0, 0);//����λ��
	g_pTerrain->RenderTerrain(&pTerrMatWorld, FALSE);

	//�������  
	D3DXMATRIX matSky, matTransSky, matRotSky;
	D3DXMatrixTranslation(&matTransSky, 0.0f, -3500.0f, 0.0f);
	D3DXMatrixRotationY(&matRotSky, -0.000005f*timeGetTime());   //��ת�������, ��ģ���Ʋ��˶�Ч��  
	matSky = matTransSky*matRotSky;
	g_pSkyBox->RenderSky(&matSky,FALSE);

	//��ʾFPS
	int charcount = swprintf_s(g_strFPS, 20, L"FPS:%0.3f", GetFPS());	
	g_pfont->DrawText(0, g_strFPS, charcount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 0));
	g_pTextAdaperName->DrawText(NULL, g_strAdapterName, -1, &formatRect,
		DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));
	// ���������Ϣ
	formatRect.top = 30;
	static wchar_t strInfo[256] = { 0 };
	swprintf_s(strInfo, -1, L"ģ������: (%.2f, %.2f, %.2f)", g_matWorld._41, g_matWorld._42, g_matWorld._43);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));
	
	g_pD3Ddevice->EndScene();                       // ��������
	g_pD3Ddevice->Present(NULL, NULL, NULL, NULL);  // ��ת����ʾ
}
VOID        Direct3D_CleanUp()
{
	//�ͷ�COM����
	SAFE_RELEASE(g_pfont);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pD3Ddevice);
	delete g_pCamera;
	delete g_pDInput;
	delete g_pTerrain;
	delete g_pSkyBox;
	delete g_pModel;
}
float       GetFPS()
{
	static float fps = 0;
	static float frameCount = 0;
	static float currentTime = 0.0f;
	static float lastTime = 0.0f;
	frameCount++;//ÿ����һ��Get_FPS()������֡������1
	currentTime = timeGetTime()*0.001f;		   //��λs
	if (currentTime - lastTime > 1.0f)
	{
		fps = (float)frameCount;
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}
BOOL Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize)
{
	HRESULT hr;
	while (true)
	{
		pDIDevice->Poll();              // ��ѯ�豸  
		pDIDevice->Acquire();           // ��ȡ�豸�Ŀ���Ȩ  
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
		if (FAILED(pDIDevice->Acquire())) return FALSE;
	}
	return TRUE;
}
