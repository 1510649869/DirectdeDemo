#include "D3DUtil.h"
#include<d3d9.h>
#include <d3dx9.h>

struct SKYBOXVERTEX
{
	float x, y, z;
	float u, v;
};
#define D3DFVF_SKYBOX D3DFVF_XYZ|D3DFVF_TEX1
class SkyBoxClass
{
public:
	BOOL InitSkyBox(float length);
	BOOL LoadSkyTextureFromFiile(wchar_t *pFrontTextureFile, wchar_t *pBackTextureFile, wchar_t *pLeftTextureFile, wchar_t *pRightTextureFile, wchar_t *pTopTextureFile);
	VOID RenderSky(D3DXMATRIX *pMatworld, BOOL bRenderFrame);
private:
	LPDIRECT3DDEVICE9       m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DTEXTURE9      m_pTexture[5];
	float                   m_length;    //Ìì¿ÕºÐµÄ±ß³¤
public:
	SkyBoxClass(LPDIRECT3DDEVICE9 pDevice);
	~SkyBoxClass();
};