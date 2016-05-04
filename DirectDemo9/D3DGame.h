#pragma once
#include "D3DBaseGame.h"
#include "DirectInputClass.h"
#include "D3DModelLoad.h"
#include "D3DCamera.h"
#include"D3DEffect.h"
#include "D3DSprite.h"
#include "Texture2D.h"
#include "D3DSkyBox.h"
#include "D3DTerrain.h"
#include"D3DWater.h"

struct VertexPosTex
{
	float x, y, z;
	float u, v;
	static const DWORD FVF;
};
class CD3DGame:public D3DBaseGame
{
public:
	CD3DGame();
	~CD3DGame();
	virtual HRESULT LoadContent() override;
	virtual VOID UnLoadContent() override;
	virtual VOID Update() override;
	virtual VOID D3DRender() override;
public:
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	VOID      BeginEffect();
	VOID      EndEffect();
	VOID      GetParameters();
	VOID      SetParameters();
	VOID      FixedRender();
	VOID      HardDeviceContent();
private:
	//注册一些变量
	DInputClass*   g_pDInput;
	ModelClass *   g_pModel;
	CameraClass*   g_pCamera;
	CD3DEffect *   g_pEffect;
	CD3DSprite *   g_pSpreite;
	CTexture2D *   g_pTexture;
	CD3DSkyBox *   g_pSkyBox;
	CD3DTerrain*   g_pTerrain;
	CD3DWater  *   g_pWater;
private:
	IDirect3DVertexBuffer9* BkGndQuad = 0; // background quad - crate
	IDirect3DTexture9*      BkGndTex = 0; // crate texture
	D3DMATERIAL9            BkGndMtrl; // background material
	LPD3DXMESH              TeaPot;
	D3DMATERIAL9            TeaPotMtrl;
};

