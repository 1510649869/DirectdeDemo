#include "ModelClass.h"

ModelClass::ModelClass(LPDIRECT3DDEVICE9 pDevice)
{
	m_pd3dDevice = pDevice;
	m_dwNumMtrls = 0;
	m_pMaterials = NULL;
	m_pTextures  = NULL;
	m_pMesh      = NULL;
}
BOOL ModelClass::LoadModelFromFileX(wchar_t *pModelFileX)
{
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(pModelFileX, D3DXMESH_MANAGED, m_pd3dDevice,
		&pAdjBuffer, &pMtrlBuffer, NULL, &m_dwNumMtrls, &m_pMesh);
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	m_pMaterials = new D3DMATERIAL9[m_dwNumMtrls];
	m_pTextures = new LPDIRECT3DTEXTURE9[m_dwNumMtrls];
	for (DWORD i = 0; i < m_dwNumMtrls; i++)
	{
		m_pMaterials[i] = pMtrls[i].MatD3D;
		m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;
		m_pTextures[i] = NULL;
		D3DXCreateTextureFromFileA(m_pd3dDevice, pMtrls[i].pTextureFilename, &m_pTextures[i]);
	}
	pAdjBuffer->Release();
	pMtrlBuffer->Release();
	return TRUE;
}
BOOL ModelClass::RenderModel(D3DXMATRIX *pMatWorld)
{
	m_pd3dDevice->SetTransform(D3DTS_WORLD, pMatWorld);//设置模型的世界矩阵，为绘制做准备 
	for (DWORD i = 0; i < m_dwNumMtrls; i++)
	{
		m_pd3dDevice->SetMaterial(&m_pMaterials[i]);
		m_pd3dDevice->SetTexture(0, m_pTextures[i]);
		m_pMesh->DrawSubset(i);
	}
	return TRUE;
}
ModelClass::~ModelClass()
{
	for (DWORD i = 0; i < m_dwNumMtrls; i++)
	{
		SAFE_RELEASE(m_pTextures[i]);
	}
	SAFE_RELEASE(m_pMesh);
	SAFE_DELETE(m_pMaterials);
}