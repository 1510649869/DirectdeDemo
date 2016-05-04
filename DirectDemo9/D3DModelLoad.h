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
	//D3DXMATRIX					g_matWorld;          //�������
	LPD3DXMESH					m_pMesh;         // ����Ķ���
	D3DMATERIAL9*				m_pMaterials;    // ����Ĳ�����Ϣ
	LPDIRECT3DTEXTURE9*		    m_pTextures;     // �����������Ϣ
	DWORD						m_dwNumMtrls;    // ���ʵ���Ŀ
	LPDIRECT3DDEVICE9           m_pd3dDevice;
};