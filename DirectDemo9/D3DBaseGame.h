#pragma once
#include<windows.h>
#include<d3d9.h>
#include<d3dx9.h>

#include"D3DInit.h"
//#include"D3DUtil.h"//包含宏定义

class D3DBaseGame
{
public:
	D3DBaseGame();
	virtual ~D3DBaseGame();
public:
	HRESULT         InitDirect(HINSTANCE hInstance, HWND hwnd);
	VOID            ShutDown();
	VOID            BeginRender();
	VOID            EndRender();
	//virtual HRESULT LoadDevice();
	virtual HRESULT LoadContent();
	virtual VOID    UnLoadContent();
	virtual VOID    Update()=0;  //绘图更新
	virtual VOID    D3DRender()=0;//逻辑更新
public:
	LPDIRECT3DDEVICE9     g_pd3dDevice;
	LPDIRECT3D9           g_pd3d;
	D3DPRESENT_PARAMETERS g_pd3dpp;
protected:
	HWND hWnd;
	HINSTANCE hInstance;
public:
};

