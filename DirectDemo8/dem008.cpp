/*配置环境所需要的文件
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

#pragma comment(lib,"winmm.lib")  //调用PlaySound函数所需库文件
#pragma comment(lib, "dinput8.lib") 

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_CLASS L"WndClass"
#define WINDOW_TITLE L"DirectWnd"

#define SAFE_RELEASE(p)  {if(p){(p)->Release();(p)=NULL;}}
#define SAFE_DELETE(p)  { if(p) { delete (p); (p)=NULL; } }


//定义全局的设备驱动
LPDIRECT3DDEVICE9    g_pD3Ddevice = NULL;
ID3DXFont *          g_pfont = NULL;
LPD3DXFONT           g_pTextAdaperName = NULL;  // 显卡信息的2D文本 
LPD3DXFONT           g_pTextHelper = NULL;
float g_FPS = 0.0f;
WCHAR g_strFPS[50];
WCHAR g_strAdapterName[60] = { 0 };//包含显卡名称的字符数组

////DirectInput8的一些变量
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

//从外界文件读取信息保存
D3DXMATRIX					g_matWorld;          //世界矩阵
LPD3DXMESH					g_pMesh = NULL;      // 网格的对象
D3DMATERIAL9*				g_pMaterials = NULL; // 网格的材质信息
LPDIRECT3DTEXTURE9*		    g_pTextures = NULL;  // 网格的纹理信息
DWORD						g_dwNumMtrls = 0;    // 材质的数目

LRESULT                     CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL                        Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize);
HRESULT                     Direct3D_Init(HWND hWnd, HINSTANCE hInstance);
HRESULT                     Objects_Init(HWND hWnd);
VOID                        Direct3D_Render(HWND hWnd);
VOID                        Direct3D_Update(HWND hwnd);
VOID                        Direct3D_CleanUp();
VOID                        Matrix_Set();      //封装了四大变换的函数
float                       GetFPS();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);			                 //设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	                     //设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					                 //设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;								         //窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							             //窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						             //指定包含窗口过程的程序的实例句柄。
	//wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);                  //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);     //为hbrBackground成员指定一个白色画刷句柄	
	wndClass.lpszMenuName = NULL;						              //用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = WINDOW_CLASS;		                      //用一个以空终止的字符串，指定窗口类的名字。

	if (!RegisterClassEx(&wndClass))
		return -1;
	HWND hwnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (S_OK != Direct3D_Init(hwnd, hInstance))
	{
		MessageBox(hwnd, L"Direct3D初始化失败~！", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口  
	}

	//PlaySound(L"Music\\Eternal Love .wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow(hwnd, nShowCmd);                                         //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);

	g_pDInput = new DInputClass();
	g_pDInput->Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//PlaySound(L"グラン=パルスのルシ.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);		//该函数分发一个消息给窗口程序。
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
		Direct3D_Render(hwnd);                 //调用Direct3D渲染函数
		ValidateRect(hwnd, NULL);		       // 更新客户区的显示
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);		      // 销毁窗口, 并发送一条WM_DESTROY消息
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
	// 【Direct3D初始化一，创建接口】：创建Direct3D接口对象, 以便用该Direct3D对象创建Direct3D设备对象
	//--------------------------------------------------------------------------------------
	LPDIRECT3D9  pD3D = NULL; //Direct3D接口对象的创建
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION))) //初始化Direct3D接口对象，并进行DirectX版本协商
		return E_FAIL;
	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之二,取信息】：获取硬件设备信息
	//--------------------------------------------------------------------------------------
	D3DCAPS9 caps; int vp = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;   //支持硬件顶点运算，我们就采用硬件顶点运算 
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;   //不支持硬件顶点运算，无奈只好采用软件顶点运算
	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之三，填内容】：填充D3DPRESENT_PARAMETERS结构体
	//--------------------------------------------------------------------------------------
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
	//--------------------------------------------------------------------------------------
	// 【Direct3D初始化四步曲之四，创设备】：创建Direct3D设备接口
	//--------------------------------------------------------------------------------------
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, vp, &d3dpp, &g_pD3Ddevice)))
		return E_FAIL;

	//获取显卡信号
	WCHAR TeamName[60] = L"当前的显卡型号：";
	D3DADAPTER_IDENTIFIER9 adapter;
	pD3D->GetAdapterIdentifier(0, 0, &adapter);
	int len = MultiByteToWideChar(CP_ACP, 0, adapter.Description, -1, NULL, 0);	//显卡型号已经存放在了adapter.Description中 为char类型
	MultiByteToWideChar(CP_ACP, 0, adapter.Description, -1, g_strAdapterName, len);
	wcscat_s(TeamName, g_strAdapterName);
	wcscpy_s(g_strAdapterName, TeamName);

	SAFE_RELEASE(pD3D) //LPDIRECT3D9接口对象的使命完成，我们将其释放掉
	if (!(S_OK == Objects_Init(hwnd))) return E_FAIL;     //调用一次Objects_Init，进行渲染资源的初始化
	g_pD3Ddevice->SetRenderState(D3DRS_LIGHTING, FALSE);			  //关闭光照
	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);	//开启背面消隐
	return S_OK;
}
HRESULT     Objects_Init(HWND hWnd)
{
	srand(timeGetTime());      //用系统时间初始化随机种子 
	//创建字体
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pfont)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 25, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pTextAdaperName)))
		return E_FAIL;
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 25, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pTextHelper)))
		return E_FAIL;
	//创建纹理贴图
	if (FAILED(D3DXCreateTextureFromFile(g_pD3Ddevice, L"pal5q.jpg", &g_pTexture)))
	{
		return E_FAIL;
	}
	//设置材质
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
	//设置渲染状态
	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐  
	//g_pD3Ddevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //设置环境光 

	return S_OK;
}
//VOID        Matrix_Set()
//{
//	//--------------------------------------------------------------------------------------
//	//【四大变换之一】：世界变换矩阵的设置
//	//--------------------------------------------------------------------------------------
//
//
//	//--------------------------------------------------------------------------------------
//	//【四大变换之二】：取景变换矩阵的设置
//	//--------------------------------------------------------------------------------------
//	D3DXMATRIX matView; //定义一个矩阵
//	D3DXVECTOR3 vEye(0.0f, 0, -250.0f);  //摄像机的位置
//	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
//	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
//	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
//	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵
//
//	//--------------------------------------------------------------------------------------
//	//【四大变换之三】：投影变换矩阵的设置	  //透视投影
//	//--------------------------------------------------------------------------------------
//	D3DXMATRIX matProj;                                                        //定义一个矩阵
//	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //计算投影变换矩阵
//	g_pD3Ddevice->SetTransform(D3DTS_PROJECTION, &matProj);                    //设置投影变换矩阵
//
//	//--------------------------------------------------------------------------------------
//	//【四大变换之四】：视口变换的设置
//	//--------------------------------------------------------------------------------------
//	D3DVIEWPORT9 vp; //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了
//	vp.X = 0;		//表示视口相对于窗口的X坐标
//	vp.Y = 0;		//视口相对对窗口的Y坐标
//	vp.Width = WINDOW_WIDTH;	//视口的宽度
//	vp.Height = WINDOW_HEIGHT; //视口的高度
//	vp.MinZ = 0.0f; //视口在深度缓存中的最小深度值
//	vp.MaxZ = 1.0f;	//视口在深度缓存中的最大深度值
//	g_pD3Ddevice->SetViewport(&vp); //视口的设置
//}
VOID        Direct3D_Update(HWND hwnd)
{
	//使用DirectInput类读取数据  
	g_pDInput->GetInput();
	// 获取键盘消息并给予设置相应的填充模式    
	if (g_pDInput->IsKeyDown(DIK_1))         // 若数字键1被按下，进行实体填充    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (g_pDInput->IsKeyDown(DIK_2))         // 若数字键2被按下，进行线框填充    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	// 沿摄像机各分量移动视角  
	if (g_pDInput->IsKeyDown(DIK_A))      g_pCamera->MoveAlongRightVec(-0.3f);
	if (g_pDInput->IsKeyDown(DIK_D))      g_pCamera->MoveAlongRightVec(0.3f);
	if (g_pDInput->IsKeyDown(DIK_W))      g_pCamera->MoveAlongLookVec(3.0f);
	if (g_pDInput->IsKeyDown(DIK_S))      g_pCamera->MoveAlongLookVec(-3.0f);
	if (g_pDInput->IsKeyDown(DIK_R))      g_pCamera->MoveAlongUpVec(0.3f);
	if (g_pDInput->IsKeyDown(DIK_F))      g_pCamera->MoveAlongUpVec(-0.3f);

	if (g_pDInput->IsKeyDown(DIK_Q))      g_pCamera->RotationLookVec(0.0003f);
	if (g_pDInput->IsKeyDown(DIK_E))      g_pCamera->RotationLookVec(-0.0003f);
	//沿摄像机各分量旋转视角  
	if (g_pDInput->IsKeyDown(DIK_LEFT))   g_pCamera->RotationUpVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_RIGHT))  g_pCamera->RotationUpVec(0.003f);
	if (g_pDInput->IsKeyDown(DIK_UP))     g_pCamera->RotationRightVec(-0.003f);
	if (g_pDInput->IsKeyDown(DIK_DOWN))   g_pCamera->RotationRightVec(0.003f);
 
	//鼠标控制右向量和上向量的旋转  
	if (g_pDInput->IsMouseButtonDown(0))
	{

	    g_pCamera->RotationRightVec(g_pDInput->MouseDY() * 0.001f);
		//if (g_pDInput->MouseDX() < g_pDInput->MouseDX())		
	}
	if (g_pDInput->IsMouseButtonDown(1))
		g_pCamera->RotationUpVec(g_pDInput->MouseDX()* 0.001f);

	//鼠标滚轮控制观察点收缩操作  
	static FLOAT fPosZ = 0.0f;
	fPosZ += g_pDInput->MouseDZ()*0.03f;
	//计算并设置取景变换矩阵  
	D3DXMATRIX matView;
	g_pCamera->CalculateViewMatrix(&matView);
	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView);

	////把正确的世界变换矩阵存到g_matWorld中 
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
	//将lt和rb的窗口坐标转换为屏幕坐标  
	ClientToScreen(hwnd, &lt);
	ClientToScreen(hwnd, &rb);
	//以屏幕坐标重新设定矩形区域  
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	//限制鼠标光标移动区域  
	ClipCursor(&rect);
	ShowCursor(false);
}
VOID        Direct3D_Render(HWND hwnd)
{
	g_pD3Ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	g_pD3Ddevice->BeginScene();    // 开始绘制
	D3DXMATRIX mScal, mRot2, mTrans, mFinal;   
	D3DXMatrixTranslation(&mTrans, 0.0f, 600.0f, 200.0f);
	D3DXMatrixScaling(&mScal, 3.0f, 3.0f, 3.0f);
	D3DXMatrixRotationY(&mRot2,D3DX_PI);
	mFinal = mScal*mTrans*mRot2;
	g_pModel->RenderModel(&mFinal);

	D3DXMATRIX pTerrMatWorld;
	D3DXMatrixIdentity(&pTerrMatWorld);
	D3DXMatrixTranslation(&pTerrMatWorld, 0, 0, 0);//设置位置
	g_pTerrain->RenderTerrain(&pTerrMatWorld, FALSE);

	//绘制天空  
	D3DXMATRIX matSky, matTransSky, matRotSky;
	D3DXMatrixTranslation(&matTransSky, 0.0f, -3500.0f, 0.0f);
	D3DXMatrixRotationY(&matRotSky, -0.000005f*timeGetTime());   //旋转天空网格, 简单模拟云彩运动效果  
	matSky = matTransSky*matRotSky;
	g_pSkyBox->RenderSky(&matSky,FALSE);

	//显示FPS
	int charcount = swprintf_s(g_strFPS, 20, L"FPS:%0.3f", GetFPS());	
	g_pfont->DrawText(0, g_strFPS, charcount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 0));
	g_pTextAdaperName->DrawText(NULL, g_strAdapterName, -1, &formatRect,
		DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));
	// 输出绘制信息
	formatRect.top = 30;
	static wchar_t strInfo[256] = { 0 };
	swprintf_s(strInfo, -1, L"模型坐标: (%.2f, %.2f, %.2f)", g_matWorld._41, g_matWorld._42, g_matWorld._43);
	g_pTextHelper->DrawText(NULL, strInfo, -1, &formatRect, DT_SINGLELINE | DT_NOCLIP | DT_LEFT, D3DCOLOR_RGBA(135, 239, 136, 255));
	
	g_pD3Ddevice->EndScene();                       // 结束绘制
	g_pD3Ddevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
}
VOID        Direct3D_CleanUp()
{
	//释放COM对象
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
	frameCount++;//每调用一次Get_FPS()函数，帧数自增1
	currentTime = timeGetTime()*0.001f;		   //单位s
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
		pDIDevice->Poll();              // 轮询设备  
		pDIDevice->Acquire();           // 获取设备的控制权  
		if (SUCCEEDED(hr = pDIDevice->GetDeviceState(lSize, pBuffer))) break;
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED) return FALSE;
		if (FAILED(pDIDevice->Acquire())) return FALSE;
	}
	return TRUE;
}
