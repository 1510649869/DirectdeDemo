#include"D3DBaseGame.h";
D3DBaseGame::D3DBaseGame()
{
	g_pd3d = NULL;
	g_pd3dDevice = NULL;
}
D3DBaseGame::~D3DBaseGame()
{
	ShutDown();
}
HRESULT D3DBaseGame::InitDirect(HINSTANCE hInstance, HWND hwnd)
{
	this->hInstance = hInstance;
	this->hWnd = hwnd;
	if (S_OK == InitD3D(&g_pd3d, &g_pd3dDevice,&g_pd3dpp, hwnd,TRUE))
	   return LoadContent();
	return E_FAIL;
}
VOID    D3DBaseGame::BeginRender()
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_pd3dDevice->BeginScene();
}
VOID    D3DBaseGame::EndRender()
{
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);  // 翻转与显示
}
HRESULT D3DBaseGame::LoadContent()
{
	//进行重载
	return S_OK;
}
VOID    D3DBaseGame::UnLoadContent()
{
	//进行相关重载
}
VOID    D3DBaseGame::ShutDown()
{
	UnLoadContent();
	SAFE_RELEASE(g_pd3d);
	SAFE_RELEASE(g_pd3dDevice);
}