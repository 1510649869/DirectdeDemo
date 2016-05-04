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

#pragma comment(lib,"winmm.lib")  //调用PlaySound函数所需库文件

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WINDOW_CLASS L"WndClass"
#define WINDOW_TITLE L"DirectWnd"

#define SAFE_RELEASE(p)  {if(p){(p)->Release();(p)=NULL;}}

 

//定义全局的设备驱动
LPDIRECT3DDEVICE9   g_pD3Ddevice = NULL;
ID3DXFont * g_pfont = NULL;
float g_FPS = 0.0f;
WCHAR g_strFPS[50];

LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;    //顶点缓存对象  
LPDIRECT3DINDEXBUFFER9  g_pIndexBuffer = NULL;    // 索引缓存对象  
LPD3DXMESH g_teapot = NULL;                       //茶壶对象  
LPD3DXMESH g_cube = NULL;                         //立方体（盒子）对象  
LPD3DXMESH g_sphere = NULL;                       //球面体对象  
LPD3DXMESH g_torus = NULL;                        //圆环对象  
LPD3DXMESH g_cylinder = NULL;
D3DXMATRIX g_WorldMatrix[4], R;                   //定义一些全局的世界矩阵  
D3DMATERIAL9 mater;

LPD3DXMESH          g_pMeshWall = NULL; // 墙面网格对象
D3DMATERIAL9        g_MaterialsWall;  // 材质


LPDIRECT3DTEXTURE9      g_pTexture = NULL;   // 纹理接口对象 用于贴图

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT    Direct3D_Init(HWND hWnd);
HRESULT    Objects_Init(HWND hWnd);
VOID       Direct3D_Render(HWND hWnd);
VOID       Direct3D_CleanUp();
float      GetFPS();
VOID       Matrix_Set();//封装了四大变换的函数
VOID       Light_Set(UINT nType);


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

	if (S_OK == Direct3D_Init(hwnd))
	{
		//MessageBox(hwnd, L"Direct3D初始化完成~！", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口  
	}
	else
	{
		MessageBox(hwnd, L"Direct3D初始化失败~！", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口  
	}

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow(hwnd, nShowCmd);                                         //调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);

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

HRESULT    Direct3D_Init(HWND hwnd)
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
	SAFE_RELEASE(pD3D) //LPDIRECT3D9接口对象的使命完成，我们将其释放掉
	if (!(S_OK == Objects_Init(hwnd))) return E_FAIL;     //调用一次Objects_Init，进行渲染资源的初始化
	
	return S_OK;
}
HRESULT    Objects_Init(HWND hWnd)
{
	srand(timeGetTime());      //用系统时间初始化随机种子 
	//创建字体
	if (FAILED(D3DXCreateFont(g_pD3Ddevice, 36, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &g_pfont)))
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

	//设置材质

	::ZeroMemory(&mater, sizeof(mater));
	mater.Ambient   =  D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
	mater.Diffuse   =  D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	mater.Specular  =  D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
	mater.Emissive  =  D3DXCOLOR(0.3f, 0.1f, 0.1f, 1.0f);
	mater.Power =100; //设置材质漫反射光照强度。
	
	//设置光照
	Light_Set(2);
	// 设置渲染状态  
	g_pD3Ddevice->SetRenderState(D3DRS_LIGHTING, true);              //开启光照 
	g_pD3Ddevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pD3Ddevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);        //开启背面消隐  
 
	return S_OK;
}
VOID       Matrix_Set()
{
	//--------------------------------------------------------------------------------------  
	//【四大变换之二】：取景变换矩阵的设置  
	//--------------------------------------------------------------------------------------  
	D3DXMATRIX   matView; //定义一个矩阵  
	D3DXVECTOR3  vEye(10.0f, 0.0f, -15.0f);   //摄像机的位置  
	D3DXVECTOR3  vAt(0.0f, 0.0f, 0.0f);      //观察点的位置  
	D3DXVECTOR3  vUp(0.0f, 1.0f, 0.0f);      //向上的向量  
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵  
	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵  
	//--------------------------------------------------------------------------------------  
	//【四大变换之三】：投影变换矩阵的设置  
	//--------------------------------------------------------------------------------------  
	D3DXMATRIX matProj; //定义一个矩阵  
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //计算投影变换矩阵  
	g_pD3Ddevice->SetTransform(D3DTS_PROJECTION, &matProj);  //设置投影变换矩阵  
	//--------------------------------------------------------------------------------------  
	//【四大变换之四】：视口变换的设置  
	//--------------------------------------------------------------------------------------  
	D3DVIEWPORT9 vp;           //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了  
	vp.X = 0;                  //表示视口相对于窗口的X坐标  
	vp.Y = 0;                  //视口相对对窗口的Y坐标  
	vp.Width = WINDOW_WIDTH;   //视口的宽度  
	vp.Height = WINDOW_HEIGHT; //视口的高度  
	vp.MinZ = 0.0f;            //视口在深度缓存中的最小深度值  
	vp.MaxZ = 1.0f;            //视口在深度缓存中的最大深度值  
	g_pD3Ddevice->SetViewport(&vp); //视口的设置  
}
VOID       Light_Set(UINT nType)
{
	//更具nType来选择不同的光照
	static D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	switch (nType)
	{
	case 1:	  //点光源
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
	case 2:	  //平行光
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		break;
	case 3:	  //聚光灯
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
	// 【Direct3D渲染五步曲之一】：清屏操作
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 50, 0), 1.0f, 0);
	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之二】：开始绘制
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->BeginScene();    // 开始绘制

	Matrix_Set();
	if (::GetAsyncKeyState(0x31) & 0x8000f)
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else if (::GetAsyncKeyState(0x32)&0x8000f)
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	if (::GetAsyncKeyState('Q')  & 0x8000f)         // 若键盘上的按键Q被按下,光源类型设为点光源
		Light_Set(1);
	if (::GetAsyncKeyState(0x57) & 0x8000f)         // 若键盘上的按键W被按下，光源类型设为平行光源
		Light_Set(2);
	if (::GetAsyncKeyState(0x45) & 0x8000f)         // 若键盘上的按键E被按下，光源类型设为聚光灯
		Light_Set(3);


	//g_pD3Ddevice->SetMaterial(&mater);
	//D3DXMatrixRotationY(&R, ::timeGetTime() / 720.0f);
	//D3DXMatrixTranslation(&g_WorldMatrix[0], 3.0f, -3.0f, 0.0f);
	//g_WorldMatrix[0] = g_WorldMatrix[0];
	//g_pD3Ddevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[0]);
	//g_cube->DrawSubset(0);

	//进行茶壶的绘制

	D3DXMatrixTranslation(&g_WorldMatrix[1], -3.0f, 0.0f, -2.0f);
	g_WorldMatrix[1] = g_WorldMatrix[1];
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[1]);
	g_teapot->DrawSubset(0);

	////进行球体的绘制
	//D3DXMatrixTranslation(&g_WorldMatrix[2], 3.0f, 3.0f, 0.0f);
	//g_WorldMatrix[2];
	//g_pD3Ddevice->SetTransform(D3DTS_WORLD, &g_WorldMatrix[2]);
	//g_sphere->DrawSubset(0);

///绘制镜面
	D3DXMATRIX matWorld;
	D3DXMATRIX R; 	
	D3DXMatrixRotationY(&R, D3DX_PI / 4);
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &(matWorld));
	g_pD3Ddevice->SetMaterial(&g_MaterialsWall);
	g_pD3Ddevice->SetTexture(0, 0);
	g_pMeshWall ->DrawSubset(0);

	//3. 启用模板缓存，以及对相关的绘制状态进行设置。
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILENABLE, true);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP); 
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	g_pD3Ddevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// 4.进行融合操作，以及禁止向深度缓存和后台缓存写数据
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
	g_pD3Ddevice->SetTransform(D3DTS_WORLD, &matWorld);//设置模型的世界矩阵，为绘制做准备
	g_teapot->DrawSubset(0);

	g_pD3Ddevice->SetRenderState(D3DRS_STENCILPASS, false);
	//显示FPS
	int charcount = swprintf_s(g_strFPS, 10, L"FPS:%0.3f", GetFPS());
	g_pfont->DrawText(0, g_strFPS, charcount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 0));
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之四】：结束绘制
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->EndScene();                       // 结束绘制
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之五】：显示翻转
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
}
VOID       Direct3D_CleanUp()
{
	//释放COM对象
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
