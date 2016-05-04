#pragma once
#include"D3DInit.h"
class CTexture2D
{
public:
	CTexture2D(LPDIRECT3DDEVICE9 pDevice);
	CTexture2D();
	~CTexture2D();
public:
	BOOL LoadTexture(wchar_t* szFxFileName,
		UINT SizeX = D3DX_DEFAULT_NONPOW2,
		UINT SizeY = D3DX_DEFAULT_NONPOW2);
public:
	UINT GetWidth(){ return m_Width; }
	UINT GetHeight(){ return m_Height; }
	RECT GetRect(){ return m_SurfRect; }
	IDirect3DTexture9* GetTexture(){ return m_pTexture; }
	D3DXIMAGE_INFO GetTextureInfo(){ return m_pTextureInfo; }
private:
	IDirect3DTexture9* m_pTexture;
	D3DXIMAGE_INFO     m_pTextureInfo;
	RECT               m_SurfRect;
	UINT               m_Width;
	UINT               m_Height;
private:
	LPDIRECT3DDEVICE9 g_pD3DDevice;
};

