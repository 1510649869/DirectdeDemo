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

 

//����ȫ�ֵ��豸����
LPDIRECT3DDEVICE9   g_pD3Ddevice = NULL;
ID3DXFont * g_pfont = NULL;
float g_FPS = 0.0f;
WCHAR g_strFPS[50];

LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;    //���㻺�����  
LPDIRECT3DINDEXBUFFER9  g_pIndexBuffer = NULL;    // �����������  
LPD3DXMESH g_teapot = NULL;                       //�������  
LPD3DXMESH g_cube = NULL;                         //�����壨���ӣ�����  
LPD3DXMESH g_sphere = NULL;                       //���������  
LPD3DXMESH g_torus = NULL;                        //Բ������  
LPD3DXMESH g_cylinder = NULL;
D3DXMATRIX g_WorldMatrix[4], R;                   //����һЩȫ�ֵ��������  
D3DMATERIAL9 mater;

LPD3DXMESH          g_pMeshWall = NULL; // ǽ���������
D3DMATERIAL9        g_MaterialsWall;  // ����


LPDIRECT3DTEXTURE9      g_pTexture = NULL;   // ����ӿڶ��� ������ͼ

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT    Direct3D_Init(HWND hWnd);
HRESULT    Objects_Init(HWND hWnd);
VOID       Direct3D_Render(HWND hWnd);
VOID       Direct3D_CleanUp();
float      GetFPS();
VOID       Matrix_Set();//��װ���Ĵ�任�ĺ���
VOID       Light_Set(UINT nType);


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

	if (FAILED(D3DXCreateBox(g_pD3Ddevice, 2, 2, 2, &g_cube, NULL)))
		return E_FAIL;
	if (FAILED(D3DXCreateTeapot(g_pD3Ddevice, &g_teapot, NULL)))
		return E_FAIL;
	if (FAILED(D3DXCreateSphere(g_pD3Ddevice, 2, 100, 100, &g_sphere, NULL)))
		return E_FAIL;
	

	D3DXCreateBox(g_pD3Ddevice, 10.0f, 10.0f, 0.08f, &g_pMeshWall, NULL);
	g_MaterialsWall.Ambient  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_MaterialsWall.Diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_MaterialsWall.Specular = D3DXCOLOR(0.2f, 1.0f, 1.0f, 1.0f);

	//���ò���

	::ZeroMemory(&mater, sizeof(mater));
	mater.Ambient   =  D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
	mater.Diffuse   =  D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	mater.Specular  =  D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	mater.Emissive  =  D3DXCOLOR(0.3f, 0.1f, 0.1f, 1.0f);
	mater.Power =100; //���ò������������ǿ�ȡ�
	
	//���ù���
	Light_Set(2);
	// ������Ⱦ״̬  
	g_pD3Ddevice->SetRenderState(D3DRS_LIGHTING, true);              //�������� 
	g_pD3Ddevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pD3Ddevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);        //������������  
 
	return S_OK;
}
VOID       Matrix_Set()
{
	//--------------------------------------------------------------------------------------  
	//���Ĵ�任֮������ȡ���任���������  
	//--------------------------------------------------------------------------------------  
	D3DXMATRIX   matView; //����һ������  
	D3DXVECTOR3  vEye(10.0f, 0.0f, -15.0f);   //�������λ��  
	D3DXVECTOR3  vAt(0.0f, 0.0f, 0.0f);      //�۲���λ��  
	D3DXVECTOR3  vUp(0.0f, 1.0f, 0.0f);      //���ϵ�����  
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����  
	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����  
	//--------------------------------------------------------------------------------------  
	//���Ĵ�任֮������ͶӰ�任���������  
	//--------------------------------------------------------------------------------------  
	D3DXMATRIX matProj; //����һ������  
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //����ͶӰ�任����  
	g_pD3Ddevice->SetTransform(D3DTS_PROJECTION, &matProj);  //����ͶӰ�任����  
	//--------------------------------------------------------------------------------------  
	//���Ĵ�任֮�ġ����ӿڱ任������  
	//--------------------------------------------------------------------------------------  
	D3DVIEWPORT9 vp;           //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����  
	vp.X = 0;                  //��ʾ�ӿ�����ڴ��ڵ�X����  
	vp.Y = 0;                  //�ӿ���ԶԴ��ڵ�Y����  
	vp.Width = WINDOW_WIDTH;   //�ӿڵĿ��  
	vp.Height = WINDOW_HEIGHT; //�ӿڵĸ߶�  
	vp.MinZ = 0.0f;            //�ӿ�����Ȼ����е���С���ֵ  
	vp.MaxZ = 1.0f;            //�ӿ�����Ȼ����е�������ֵ  
	g_pD3Ddevice->SetViewport(&vp); //�ӿڵ�����  
}
VOID       Light_Set(UINT nType)
{
	//����nType��ѡ��ͬ�Ĺ���
	static D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	switch (nType)
	{
	case 1:	  //���Դ
		light.Type       = D3DLIGHT_POINT;
		light.Ambient    = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		light.Diffuse    = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular   = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
		light.Position   = D3DXVECTOR3(0.0f, 200.0f, 0.0f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range        = 300.0f;
		break;
	case 2:	  //ƽ�й�
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		break;
	case 3:	  //�۹��
		light.Type      = D3DLIGHT_SPOT;
		light.Position  = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
		light.Direction = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		light.Ambient   = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range        = 300.0f;
		light.Falloff      = 0.1f;
		light.Phi          = D3DX_PI / 3.0f;
		light.Theta        = D3DX_PI / 6.0f;
		break;
	default:
		break;
	}
	g_pD3Ddevice->SetLight(0, &light);
	g_pD3Ddevice->LightEnable(0, true);
	g_pD3Ddevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(30, 30, 30));
}
VOID       Direct3D_Render(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮һ������������
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 50, 0), 1.0f, 0);
	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮��������ʼ����
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->BeginScene();    // ��ʼ����

	Matrix_Set();
	if (::GetAsyncKeyState(0x31) & 0x8000f)
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else if (::GetAsyncKeyState(0x32)&0x8000f)
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	if (::GetAsyncKeyState('Q')  & 0x8000f)         // �������ϵİ���Q������,��Դ������Ϊ���Դ
		Light_Set(1);
	if (::GetAsyncKeyState(0x57) & 0x8000f)         // �������ϵİ���W�����£���Դ������Ϊƽ�й�Դ
		Light_Set(2);
	if (::GetAsyncKeyState(0x45) & 0x8000f)         // �������ϵİ���E�����£���Դ������Ϊ�۹��
		Light_Set(3);


	//g_pD3Ddevice->SetMaterial(&mater);
	//D3DXMatrixRotationY(&R, ::timeGetTime() / 720.0f);
	//D3DXMatrixTranslation(&g_WorldMatrix[0], 3.0f, -3.0f, 0.0f);
	//g_WorldMatrix[0] = g_WorldMatrix[0];
	//g_pD3Ddevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[0]);
	//g_cube->DrawSubset(0);

	//���в���Ļ���

	D3DXMatrixTranslation(&g_WorldMatrix[1], -3.0f, 0.0f, -2.0f);
	g_WorldMatrix[1] = g_WorldMatrix[1];
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[1]);
	g_teapot->DrawSubset(0);

	////��������Ļ���
	//D3DXMatrixTranslation(&g_WorldMatrix[2], 3.0f, 3.0f, 0.0f);
	//g_WorldMatrix[2];
	//g_pD3Ddevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[2]);
	//g_sphere->DrawSubset(0);

///���ƾ���
	D3DXMATRIX matWorld;
	D3DXMATRIX R; 	
	D3DXMatrixRotationY(&R, D3DX_PI / 4);
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &(matWorld));
	g_pD3Ddevice->SetMaterial(&g_MaterialsWall);
	g_pD3Ddevice->SetTexture(0, 0);
	g_pMeshWall ->DrawSubset(0);

	//3. ����ģ�建�棬�Լ�����صĻ���״̬�������á�
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILENABLE, true);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); 
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// 4.�����ںϲ������Լ���ֹ����Ȼ���ͺ�̨����д����
	g_pD3Ddevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	g_pD3Ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pD3Ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	g_pD3Ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//D3DXMatrixRotationY(&R, D3DX_PI / 4);
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &(matWorld));
	g_pD3Ddevice->SetMaterial(&g_MaterialsWall);
	g_pMeshWall->DrawSubset(0);

	g_pD3Ddevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	g_pD3Ddevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	g_pD3Ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	g_pD3Ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);


	D3DXMATRIX matReflect;
	D3DXPLANE planeXY(0.0f, 0.0f, 1.0f, 0.0f);
	D3DXMatrixReflect(&matReflect, &planeXY);
	matWorld = matReflect*g_WorldMatrix[1];
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &matWorld);//����ģ�͵��������Ϊ������׼��
	g_teapot->DrawSubset(0);

	g_pD3Ddevice->SetRenderState(D3DRS_STENCILPASS, false);
	//��ʾFPS
	int charcount = swprintf_s(g_strFPS, 10, L"FPS:%0.3f", GetFPS());
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
	SAFE_RELEASE(g_cube);
	SAFE_RELEASE(g_teapot);
	SAFE_RELEASE(g_sphere);
	SAFE_RELEASE(g_pD3Ddevice);
}
float      GetFPS()
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
