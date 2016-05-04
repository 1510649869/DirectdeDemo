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
float g_FPS = 0.0f;
WCHAR g_strFPS[50];
WCHAR g_strAdapterName[60] = { 0 };//�����Կ����Ƶ��ַ�����

//DirectInput8��һЩ����
LPDIRECTINPUT8        g_pDirectInput           = NULL;
LPDIRECTINPUTDEVICE8  g_pMouseDevice           = NULL;
DIMOUSESTATE          g_diMouseState           = { 0 };
LPDIRECTINPUTDEVICE8  g_pKeyboardDevice        = NULL;
char                  g_pKeyStateBuffer[256]   = { 0 };


LPDIRECT3DVERTEXBUFFER9     g_pVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9      g_pIndexBuffer = NULL;
LPDIRECT3DTEXTURE9          g_pTexture = NULL;

//������ļ���ȡ��Ϣ����
D3DXMATRIX					g_matWorld;          //�������
LPD3DXMESH					g_pMesh = NULL;      // ����Ķ���
D3DMATERIAL9*				g_pMaterials = NULL; // ����Ĳ�����Ϣ
LPDIRECT3DTEXTURE9*		    g_pTextures = NULL;  // �����������Ϣ
DWORD						g_dwNumMtrls = 0;    // ���ʵ���Ŀ

LRESULT    CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL       Device_Read     (IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize);
HRESULT    Direct3D_Init   (HWND hWnd,HINSTANCE hInstance);
HRESULT    Objects_Init    (HWND hWnd);
VOID       Direct3D_Render (HWND hWnd);
VOID       Direct3D_Update (HWND hwnd);
VOID       Direct3D_CleanUp();
VOID       Matrix_Set      ();      //��װ���Ĵ�任�ĺ���
float      GetFPS          ();

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

	if (S_OK == Direct3D_Init(hwnd,hInstance))
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
HRESULT     Direct3D_Init(HWND hwnd,HINSTANCE hInstance)
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
	wcscpy_s(g_strAdapterName,TeamName);

							  //���
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pMouseDevice, NULL);

	// �������ݸ�ʽ��Э������  
	g_pDirectInput->CreateDevice(GUID_SysMouse, &g_pMouseDevice, NULL);
	g_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	//��ȡ�豸����Ȩ
	g_pMouseDevice->Acquire();

	// ����DirectInput�ӿں��豸  ����
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pKeyboardDevice, NULL);
	// �������ݸ�ʽ��Э������  
	g_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	g_pKeyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//��ȡ�豸����Ȩ  
	g_pKeyboardDevice->Acquire();

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
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("΢���ź�"),&g_pTextAdaperName)))
		return E_FAIL;
	//����������ͼ
	if (FAILED(D3DXCreateTextureFromFile(g_pD3Ddevice, L"pal5q.jpg", &g_pTexture)))
	{
		return E_FAIL;
	}
	//���ò���
	D3DMATERIAL9 mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient   =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Diffuse   =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Specular  =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_pD3Ddevice->SetMaterial(&mtrl);

	// ��X�ļ��м�����������
	LPD3DXBUFFER pAdjBuffer  = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(L"loli.x", D3DXMESH_MANAGED, g_pD3Ddevice,
		&pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh);
	// ��ȡ���ʺ���������
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	g_pMaterials = new D3DMATERIAL9[g_dwNumMtrls];
	g_pTextures = new LPDIRECT3DTEXTURE9[g_dwNumMtrls];
	for (DWORD i = 0; i < g_dwNumMtrls; i++)
	{
		g_pMaterials[i] = pMtrls[i].MatD3D;
		g_pMaterials[i].Ambient = g_pMaterials[i].Diffuse;
		g_pTextures [i] = NULL;
		D3DXCreateTextureFromFileA(g_pD3Ddevice, pMtrls[i].pTextureFilename, &g_pTextures[i]);
	}
	pAdjBuffer->Release();
	pMtrlBuffer->Release();
	//������Ⱦ״̬
	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //������������  
	g_pD3Ddevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //���û����� 

	return S_OK;
}
VOID        Matrix_Set()
{
	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮һ��������任���������
	//--------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮������ȡ���任���������
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //����һ������
	D3DXVECTOR3 vEye(0.0f, 0, -250.0f);  //�������λ��
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //�۲���λ��
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//���ϵ�����
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //�����ȡ���任����
	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView); //Ӧ��ȡ���任����

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮������ͶӰ�任���������	  //͸��ͶӰ
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj;                                                        //����һ������
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //����ͶӰ�任����
	g_pD3Ddevice->SetTransform(D3DTS_PROJECTION, &matProj);                    //����ͶӰ�任����

	//--------------------------------------------------------------------------------------
	//���Ĵ�任֮�ġ����ӿڱ任������
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //ʵ����һ��D3DVIEWPORT9�ṹ�壬Ȼ��������������������ֵ�Ϳ�����
	vp.X = 0;		//��ʾ�ӿ�����ڴ��ڵ�X����
	vp.Y = 0;		//�ӿ���ԶԴ��ڵ�Y����
	vp.Width = WINDOW_WIDTH;	//�ӿڵĿ��
	vp.Height = WINDOW_HEIGHT; //�ӿڵĸ߶�
	vp.MinZ = 0.0f; //�ӿ�����Ȼ����е���С���ֵ
	vp.MaxZ = 1.0f;	//�ӿ�����Ȼ����е�������ֵ
	g_pD3Ddevice->SetViewport(&vp); //�ӿڵ�����
}
VOID        Direct3D_Update(HWND hwnd)
{
	// ��ȡ������Ϣ������������Ӧ�����ģʽ    
	if (g_pKeyStateBuffer[DIK_1] & 0x80)         // �����ּ�1�����£�����ʵ�����    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (g_pKeyStateBuffer[DIK_2] & 0x80)         // �����ּ�2�����£������߿����    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// ��ȡ�������  
	::ZeroMemory(&g_diMouseState, sizeof(g_diMouseState));
	Device_Read(g_pMouseDevice, (LPVOID)&g_diMouseState, sizeof(g_diMouseState));

	// ��ȡ��������  
	::ZeroMemory(g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));
	Device_Read(g_pKeyboardDevice, (LPVOID)g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));


	// ��ס���������϶���Ϊƽ�Ʋ���  
	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
	if (g_diMouseState.rgbButtons[0] & 0x80)
	{
		fPosX += g_diMouseState.lX *  0.08f;
		fPosY += g_diMouseState.lY * -0.08f;
	}

	//�����֣�Ϊ�۲����������  
	fPosZ += g_diMouseState.lZ * -0.02f;

	// ƽ������  
	if (g_pKeyStateBuffer[DIK_A] & 0x80) fPosX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_D] & 0x80) fPosX += 0.005f;
	if (g_pKeyStateBuffer[DIK_W] & 0x80) fPosY += 0.005f;
	if (g_pKeyStateBuffer[DIK_S] & 0x80) fPosY -= 0.005f;


	D3DXMatrixTranslation(&g_matWorld, fPosX, fPosY, fPosZ);


	// ��ס����Ҽ����϶���Ϊ��ת����  
	static float fAngleX = 0.15f, fAngleY = -(float)D3DX_PI;
	if (g_diMouseState.rgbButtons[1] & 0x80)
	{
		//fAngleX += g_diMouseState.lY * -0.01f;
		fAngleY += g_diMouseState.lX * -0.01f;
	}
	// ��ת����  
	if (g_pKeyStateBuffer[DIK_UP] & 0x80) fAngleX += 0.005f;
	if (g_pKeyStateBuffer[DIK_DOWN] & 0x80) fAngleX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_LEFT] & 0x80) fAngleY -= 0.005f;
	if (g_pKeyStateBuffer[DIK_RIGHT] & 0x80) fAngleY += 0.005f;


	D3DXMATRIX Rx, Ry;
	D3DXMatrixRotationX(&Rx, fAngleX);
	D3DXMatrixRotationY(&Ry, fAngleY);

	g_matWorld = Rx * Ry * g_matWorld;
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &g_matWorld);
	Matrix_Set();
}
VOID        Direct3D_Render(HWND hwnd)
{
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮һ������������
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//����һ�����Σ����ڻ�ȡ�����ھ���
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮��������ʼ����
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->BeginScene();    // ��ʼ����

	if (::GetAsyncKeyState(0x31) & 0x8000f)         // �����ּ�1�����£������߿����
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (::GetAsyncKeyState(0x32) & 0x8000f)         // �����ּ�2�����£�����ʵ�����
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	for (DWORD i = 0; i < g_dwNumMtrls; i++)
	{
		g_pD3Ddevice->SetMaterial(&g_pMaterials[i]);
		///if (i % 2 == 0)
		//{
		g_pD3Ddevice->SetTexture(0, g_pTextures[i]);
		//}
		g_pMesh->DrawSubset(i);
	}
	//��ʾFPS
	int charcount = swprintf_s(g_strFPS, 20, L"FPS:%0.3f", GetFPS());
	g_pfont->DrawText(0, g_strFPS, charcount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 0));

	g_pTextAdaperName->DrawText(NULL, g_strAdapterName, -1, &formatRect,
		DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�ġ�����������
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->EndScene();                       // ��������
	//--------------------------------------------------------------------------------------
	// ��Direct3D��Ⱦ�岽��֮�塿����ʾ��ת
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Present(NULL, NULL, NULL, NULL);  // ��ת����ʾ
}
VOID        Direct3D_CleanUp()
{
	//�ͷ�COM����
	SAFE_RELEASE(g_pfont);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pTexture);
	for (DWORD i = 0; i < g_dwNumMtrls; i++)
		SAFE_RELEASE(g_pTextures[i]);
	SAFE_DELETE(g_pTextures)
		SAFE_DELETE(g_pMaterials)
		SAFE_RELEASE(g_pMesh)
		SAFE_RELEASE(g_pD3Ddevice);
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
