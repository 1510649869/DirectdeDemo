#pragma  once
#include"D3DUtil.h"
#include "D3DInit.h"

class ModelClass
{
public:
	ModelClass(LPDIRECT3DDEVICE9 pDevice);
	~ModelClass();
public:
	BOOL LoadModelFromFileX(wchar_t *pModelFileX);
	BOOL RenderModel(D3DXVECTOR3& pos);
private:
	//D3DXMATRIX					g_matWorld;          //世界矩阵
	LPD3DXMESH					m_pMesh;         // 网格的对象
	D3DMATERIAL9*				m_pMaterials;    // 网格的材质信息
	LPDIRECT3DTEXTURE9*		    m_pTextures;     // 网格的纹理信息
	DWORD						m_dwNumMtrls;    // 材质的数目
	LPDIRECT3DDEVICE9           m_pd3dDevice;
};