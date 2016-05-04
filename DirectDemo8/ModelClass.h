#pragma  once
#include"D3DUtil.h"
#include <d3dx9.h>
#include <d3d9.h>

class ModelClass
{
public:
	ModelClass(LPDIRECT3DDEVICE9 pDevice);
	~ModelClass();
public:
	BOOL LoadModelFromFileX(wchar_t *pModelFileX);
	BOOL RenderModel(D3DXMATRIX* pMatWorld);
private:
	//D3DXMATRIX					g_matWorld;          //�������
	LPD3DXMESH					m_pMesh;         // ����Ķ���
	D3DMATERIAL9*				m_pMaterials;    // ����Ĳ�����Ϣ
	LPDIRECT3DTEXTURE9*		    m_pTextures;     // �����������Ϣ
	DWORD						m_dwNumMtrls;    // ���ʵ���Ŀ
	LPDIRECT3DDEVICE9           m_pd3dDevice;
};