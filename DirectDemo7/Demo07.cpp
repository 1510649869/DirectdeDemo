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
float g_FPS = 0.0f;
WCHAR g_strFPS[50];
WCHAR g_strAdapterName[60] = { 0 };//包含显卡名称的字符数组

//DirectInput8的一些变量
LPDIRECTINPUT8        g_pDirectInput           = NULL;
LPDIRECTINPUTDEVICE8  g_pMouseDevice           = NULL;
DIMOUSESTATE          g_diMouseState           = { 0 };
LPDIRECTINPUTDEVICE8  g_pKeyboardDevice        = NULL;
char                  g_pKeyStateBuffer[256]   = { 0 };


LPDIRECT3DVERTEXBUFFER9     g_pVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9      g_pIndexBuffer = NULL;
LPDIRECT3DTEXTURE9          g_pTexture = NULL;

//从外界文件读取信息保存
D3DXMATRIX					g_matWorld;          //世界矩阵
LPD3DXMESH					g_pMesh = NULL;      // 网格的对象
D3DMATERIAL9*				g_pMaterials = NULL; // 网格的材质信息
LPDIRECT3DTEXTURE9*		    g_pTextures = NULL;  // 网格的纹理信息
DWORD						g_dwNumMtrls = 0;    // 材质的数目

LRESULT    CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL       Device_Read     (IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize);
HRESULT    Direct3D_Init   (HWND hWnd,HINSTANCE hInstance);
HRESULT    Objects_Init    (HWND hWnd);
VOID       Direct3D_Render (HWND hWnd);
VOID       Direct3D_Update (HWND hwnd);
VOID       Direct3D_CleanUp();
VOID       Matrix_Set      ();      //封装了四大变换的函数
float      GetFPS          ();

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

	if (S_OK == Direct3D_Init(hwnd,hInstance))
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
HRESULT     Direct3D_Init(HWND hwnd,HINSTANCE hInstance)
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
	wcscpy_s(g_strAdapterName,TeamName);

							  //鼠标
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pMouseDevice, NULL);

	// 设置数据格式和协作级别  
	g_pDirectInput->CreateDevice(GUID_SysMouse, &g_pMouseDevice, NULL);
	g_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	//获取设备控制权
	g_pMouseDevice->Acquire();

	// 创建DirectInput接口和设备  键盘
	DirectInput8Create(hInstance, 0x0800, IID_IDirectInput8, (void**)&g_pDirectInput, NULL);
	g_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_pKeyboardDevice, NULL);
	// 设置数据格式和协作级别  
	g_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	g_pKeyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	//获取设备控制权  
	g_pKeyboardDevice->Acquire();

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
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"),&g_pTextAdaperName)))
		return E_FAIL;
	//创建纹理贴图
	if (FAILED(D3DXCreateTextureFromFile(g_pD3Ddevice, L"pal5q.jpg", &g_pTexture)))
	{
		return E_FAIL;
	}
	//设置材质
	D3DMATERIAL9 mtrl;
	::ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Ambient   =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Diffuse   =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Specular  =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_pD3Ddevice->SetMaterial(&mtrl);

	// 从X文件中加载网格数据
	LPD3DXBUFFER pAdjBuffer  = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(L"loli.x", D3DXMESH_MANAGED, g_pD3Ddevice,
		&pAdjBuffer, &pMtrlBuffer, NULL, &g_dwNumMtrls, &g_pMesh);
	// 读取材质和纹理数据
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
	//设置渲染状态
	g_pD3Ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);   //开启背面消隐  
	g_pD3Ddevice->SetRenderState(D3DRS_AMBIENT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)); //设置环境光 

	return S_OK;
}
VOID        Matrix_Set()
{
	//--------------------------------------------------------------------------------------
	//【四大变换之一】：世界变换矩阵的设置
	//--------------------------------------------------------------------------------------


	//--------------------------------------------------------------------------------------
	//【四大变换之二】：取景变换矩阵的设置
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matView; //定义一个矩阵
	D3DXVECTOR3 vEye(0.0f, 0, -250.0f);  //摄像机的位置
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f); //观察点的位置
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);//向上的向量
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp); //计算出取景变换矩阵
	g_pD3Ddevice->SetTransform(D3DTS_VIEW, &matView); //应用取景变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之三】：投影变换矩阵的设置	  //透视投影
	//--------------------------------------------------------------------------------------
	D3DXMATRIX matProj;                                                        //定义一个矩阵
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 1000.0f); //计算投影变换矩阵
	g_pD3Ddevice->SetTransform(D3DTS_PROJECTION, &matProj);                    //设置投影变换矩阵

	//--------------------------------------------------------------------------------------
	//【四大变换之四】：视口变换的设置
	//--------------------------------------------------------------------------------------
	D3DVIEWPORT9 vp; //实例化一个D3DVIEWPORT9结构体，然后做填空题给各个参数赋值就可以了
	vp.X = 0;		//表示视口相对于窗口的X坐标
	vp.Y = 0;		//视口相对对窗口的Y坐标
	vp.Width = WINDOW_WIDTH;	//视口的宽度
	vp.Height = WINDOW_HEIGHT; //视口的高度
	vp.MinZ = 0.0f; //视口在深度缓存中的最小深度值
	vp.MaxZ = 1.0f;	//视口在深度缓存中的最大深度值
	g_pD3Ddevice->SetViewport(&vp); //视口的设置
}
VOID        Direct3D_Update(HWND hwnd)
{
	// 获取键盘消息并给予设置相应的填充模式    
	if (g_pKeyStateBuffer[DIK_1] & 0x80)         // 若数字键1被按下，进行实体填充    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (g_pKeyStateBuffer[DIK_2] & 0x80)         // 若数字键2被按下，进行线框填充    
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// 读取鼠标输入  
	::ZeroMemory(&g_diMouseState, sizeof(g_diMouseState));
	Device_Read(g_pMouseDevice, (LPVOID)&g_diMouseState, sizeof(g_diMouseState));

	// 读取键盘输入  
	::ZeroMemory(g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));
	Device_Read(g_pKeyboardDevice, (LPVOID)g_pKeyStateBuffer, sizeof(g_pKeyStateBuffer));


	// 按住鼠标左键并拖动，为平移操作  
	static FLOAT fPosX = 0.0f, fPosY = 30.0f, fPosZ = 0.0f;
	if (g_diMouseState.rgbButtons[0] & 0x80)
	{
		fPosX += g_diMouseState.lX *  0.08f;
		fPosY += g_diMouseState.lY * -0.08f;
	}

	//鼠标滚轮，为观察点收缩操作  
	fPosZ += g_diMouseState.lZ * -0.02f;

	// 平移物体  
	if (g_pKeyStateBuffer[DIK_A] & 0x80) fPosX -= 0.005f;
	if (g_pKeyStateBuffer[DIK_D] & 0x80) fPosX += 0.005f;
	if (g_pKeyStateBuffer[DIK_W] & 0x80) fPosY += 0.005f;
	if (g_pKeyStateBuffer[DIK_S] & 0x80) fPosY -= 0.005f;


	D3DXMatrixTranslation(&g_matWorld, fPosX, fPosY, fPosZ);


	// 按住鼠标右键并拖动，为旋转操作  
	static float fAngleX = 0.15f, fAngleY = -(float)D3DX_PI;
	if (g_diMouseState.rgbButtons[1] & 0x80)
	{
		//fAngleX += g_diMouseState.lY * -0.01f;
		fAngleY += g_diMouseState.lX * -0.01f;
	}
	// 旋转物体  
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
	// 【Direct3D渲染五步曲之一】：清屏操作
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//定义一个矩形，用于获取主窗口矩形
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之二】：开始绘制
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->BeginScene();    // 开始绘制

	if (::GetAsyncKeyState(0x31) & 0x8000f)         // 若数字键1被按下，进行线框填充
		g_pD3Ddevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	if (::GetAsyncKeyState(0x32) & 0x8000f)         // 若数字键2被按下，进行实体填充
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
	//显示FPS
	int charcount = swprintf_s(g_strFPS, 20, L"FPS:%0.3f", GetFPS());
	g_pfont->DrawText(0, g_strFPS, charcount, &formatRect, DT_TOP | DT_RIGHT, D3DCOLOR_XRGB(255, 255, 0));

	g_pTextAdaperName->DrawText(NULL, g_strAdapterName, -1, &formatRect,
		DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f));
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之四】：结束绘制
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->EndScene();                       // 结束绘制
	//--------------------------------------------------------------------------------------
	// 【Direct3D渲染五步曲之五】：显示翻转
	//--------------------------------------------------------------------------------------
	g_pD3Ddevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
}
VOID        Direct3D_CleanUp()
{
	//释放COM对象
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
