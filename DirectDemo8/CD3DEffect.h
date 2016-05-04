#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <wtypes.h>
#include "D3DUtil.h"
class CD3DEffect
{
public:
	CD3DEffect(LPDIRECT3DDEVICE9 pDevice);
	~CD3DEffect();
public:
	 BOOL LoadEffect(wchar_t* szFxFile, char* ErrMsg);
	 void BegainEffect(UINT& numPassess);
	 void EndEffect();
	 void Release();
private:
	ID3DXEffect* GetEffect(){ return m_pEffect; }
private:
	ID3DXEffect* m_pEffect;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
};