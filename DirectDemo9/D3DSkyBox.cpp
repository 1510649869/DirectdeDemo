#include "D3DSkyBox.h"

// 顶点格式的初始化放到.cpp里，避免重定义错误
const DWORD VertexSkybox::FVF = (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);

CD3DSkyBox::CD3DSkyBox(CameraClass* m_pCamera, LPDIRECT3DDEVICE9 m_pd3dDevice):m_ppTextureArray(NULL),m_pVertex(NULL),
m_pIndices(NULL)
{
	this->m_pCamera = m_pCamera;
	this->m_pd3dDevice = m_pd3dDevice;
	g_pVertexBuffer = NULL;
	g_pIndexBuffer  = NULL;
}
CD3DSkyBox::~CD3DSkyBox()
{
	Release();
}
BOOL CD3DSkyBox::LoadSkyTextureFromFiile(wchar_t *pFrontTextureFile, wchar_t *pBackTextureFile, wchar_t *pLeftTextureFile, wchar_t *pRightTextureFile, wchar_t *pTopTextureFile, wchar_t *pBottomTexFile)
{
	InitVerTices();
	// 初始化天空盒纹理
	m_ppTextureArray = new CTexture2D*[6];
	for (int ti = 0; ti < 6; ti++)
	{
		m_ppTextureArray[ti] = new CTexture2D(m_pd3dDevice);
	}
	if (!m_ppTextureArray[0]->LoadTexture(pFrontTextureFile) ||
		!m_ppTextureArray[1]->LoadTexture(pBackTextureFile) ||
		!m_ppTextureArray[2]->LoadTexture(pLeftTextureFile) ||
		!m_ppTextureArray[3]->LoadTexture(pRightTextureFile) ||
		!m_ppTextureArray[4]->LoadTexture(pTopTextureFile) ||
		!m_ppTextureArray[5]->LoadTexture(pBottomTexFile))
	{
		Release();
		return false;
	}
	return true;
}
void CD3DSkyBox::InitVerTices()
{
	m_pIndices    = new UINT16[6];
	m_pIndices[0] = 0;
	m_pIndices[1] = 1;
	m_pIndices[2] = 2;
	m_pIndices[3] = 0;
	m_pIndices[4] = 2;
	m_pIndices[5] = 3;
	const float t = 1.0f;
	const float o = 0.0f;

	m_pVertex     = new VertexSkybox[24];
	// 前
	m_pVertex[0]  = VertexSkybox(-1, -1, -1, 0, 0, 1, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f), t, t);
	m_pVertex[1]  = VertexSkybox(1, -1, -1, 0, 0, 1, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, t);
	m_pVertex[2]  = VertexSkybox(1, 1, -1, 0, 0, 1, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, o);
	m_pVertex[3]  = VertexSkybox(-1, 1, -1, 0, 0, 1, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, o);
	// 后
	m_pVertex[4]  = VertexSkybox(1, -1, 1, 0, 0, -1, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, t);
	m_pVertex[5]  = VertexSkybox(-1, -1, 1, 0, 0, -1, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, t);
	m_pVertex[6]  = VertexSkybox(-1, 1, 1, 0, 0, -1, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, o);
	m_pVertex[7]  = VertexSkybox(1, 1, 1, 0, 0, -1, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, o);
	// 左
	m_pVertex[8]  = VertexSkybox(1, -1, -1, -1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, t);
	m_pVertex[9]  = VertexSkybox(1, -1, 1, -1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, t);
	m_pVertex[10] = VertexSkybox(1, 1, 1, -1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, o);
	m_pVertex[11] = VertexSkybox(1, 1, -1, -1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, o);
	// 右
	m_pVertex[12] = VertexSkybox(-1, -1, 1, 1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, t);
	m_pVertex[13] = VertexSkybox(-1, -1, -1, 1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, t);
	m_pVertex[14] = VertexSkybox(-1, 1, -1, 1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, o);
	m_pVertex[15] = VertexSkybox(-1, 1, 1, 1, 0, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, o);

	// 上
	m_pVertex[16] = VertexSkybox(1, 1, -1, 0, -1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, t);
	m_pVertex[17] = VertexSkybox(1, 1, 1, 0, -1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, t);
	m_pVertex[18] = VertexSkybox(-1, 1, 1, 0, -1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, o);
	m_pVertex[19] = VertexSkybox(-1, 1, -1, 0, -1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, o);

	// 下
	m_pVertex[20] = VertexSkybox(1, -1, 1, 0, 1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, o);
	m_pVertex[21] = VertexSkybox(1, -1, -1, 0, 1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, o);
	m_pVertex[22] = VertexSkybox(-1, -1, -1, 0, 1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), t, t);
	m_pVertex[23] = VertexSkybox(-1, -1, 1, 0, 1, 0, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), o, t);
	for (int i = 0; i < 24; i++){
		m_pVertex[i]._x *=  1.2;
		m_pVertex[i]._y *=  1.2;
		m_pVertex[i]._z *=  1.2;
		m_pVertex[i]._nx *= 1.2;
		m_pVertex[i]._ny *= 1.2;
		m_pVertex[i]._nz *= 1.2;
	}
}
void CD3DSkyBox::Draw()
{
	D3DXVECTOR3 pos;
	m_pCamera->GetCameraPostion(&pos);
	D3DXMatrixTranslation(&m_WorldTransMatrix, pos.x,pos.y, pos.z);//更新世界矩阵
	m_pd3dDevice->GetTransform(D3DTS_WORLD, &m_OriWorldTransMatrix);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_WorldTransMatrix);

	m_pd3dDevice->SetRenderState (D3DRS_ZENABLE, D3DZB_FALSE);//禁用深度缓冲
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	for (int i = 0; i < 6; i++)
	{
		m_pd3dDevice->SetTexture(0, m_ppTextureArray[i]->GetTexture());
		m_pd3dDevice->SetFVF(VertexSkybox::FVF);
		// 绘制一个面的4个顶点
		m_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, m_pIndices,
			D3DFMT_INDEX16, &m_pVertex[i*4], sizeof(VertexSkybox));
		//m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,0,0,6,0,4);
	}
	// 还原默认采样方式
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	// 重用深度缓冲
	m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	// 还原世界矩阵
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &m_OriWorldTransMatrix);
}
void CD3DSkyBox::Release()
{
	// 释放天空盒纹理
	for (int ti = 0; ti < 6; ti++)
	{
		SAFE_DELETE(m_ppTextureArray[ti]);
	}
	delete[] m_ppTextureArray;
	// 释放索引缓冲
	delete[] m_pIndices;
	// 释放顶点缓冲
	delete[] m_pVertex;
	
	m_pCamera = NULL;
	m_pd3dDevice = NULL;
}