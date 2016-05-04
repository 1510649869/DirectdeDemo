#include "SkyBoxClass.h"

SkyBoxClass::SkyBoxClass(LPDIRECT3DDEVICE9 pDevice)
{
	m_pVertexBuffer = NULL;
	m_pd3dDevice = pDevice;
	for (int i = 0; i < 5; i++)
		m_pTexture[i] = NULL;
	m_length = 0.0f;
}
BOOL SkyBoxClass::InitSkyBox(float length)
{
	//顶点创建步骤
	m_length = length;
	m_pd3dDevice->CreateVertexBuffer(20 * sizeof(SKYBOXVERTEX), 0, D3DFVF_SKYBOX, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
	 //用一个结构体把顶点数据先准备好  
    SKYBOXVERTEX vertices[] =  
    {  
        //前面的四个顶点  
        { -m_length/2, 0.0f,    m_length/2, 0.0f, 1.0f, },  
        { -m_length/2, m_length/2,   m_length/2, 0.0f, 0.0f, },  
        {  m_length/2, 0.0f,    m_length/2, 1.0f, 1.0f, },  
        {  m_length/2, m_length/2,   m_length/2, 1.0f, 0.0f, },  
  
        //背面的四个顶点  
        {  m_length/2, 0.0f,   -m_length/2, 0.0f, 1.0f, },  
        {  m_length/2, m_length/2,  -m_length/2, 0.0f, 0.0f, },  
        { -m_length/2, 0.0f,   -m_length/2, 1.0f, 1.0f, },  
        { -m_length/2, m_length/2,  -m_length/2, 1.0f, 0.0f, },  
  
        //左面的四个顶点  
        { -m_length/2, 0.0f,   -m_length/2, 0.0f, 1.0f, },  
        { -m_length/2, m_length/2,  -m_length/2, 0.0f, 0.0f, },  
        { -m_length/2, 0.0f,    m_length/2, 1.0f, 1.0f, },  
        { -m_length/2, m_length/2,   m_length/2, 1.0f, 0.0f, },    
        //右面的四个顶点  
        { m_length/2, 0.0f,   m_length/2, 0.0f, 1.0f, },  
        { m_length/2, m_length/2,  m_length/2, 0.0f, 0.0f, },  
        { m_length/2, 0.0f,  -m_length/2, 1.0f, 1.0f, },  
        { m_length/2, m_length/2, -m_length/2, 1.0f, 0.0f, },    
        //上面的四个顶点  
        {  m_length/2, m_length/2, -m_length/2, 1.0f, 0.0f, },  
        {  m_length/2, m_length/2,  m_length/2, 1.0f, 1.0f, },  
        { -m_length/2, m_length/2, -m_length/2, 0.0f, 0.0f, },  
        { -m_length/2, m_length/2,  m_length/2, 0.0f, 1.0f, },  
    }; 
	void * pVerties;
	m_pVertexBuffer->Lock(0, 0, (void**)&pVerties, 0);
	memcpy(pVerties, vertices, sizeof(vertices));
	m_pVertexBuffer->Unlock();
	return TRUE;
}
BOOL SkyBoxClass::LoadSkyTextureFromFiile(wchar_t *pFrontTextureFile, wchar_t *pBackTextureFile, wchar_t *pLeftTextureFile, wchar_t *pRightTextureFile, wchar_t *pTopTextureFile)
{
	D3DXCreateTextureFromFile(m_pd3dDevice, pFrontTextureFile, &m_pTexture[0]);  //前面
	D3DXCreateTextureFromFile(m_pd3dDevice, pBackTextureFile, &m_pTexture[1]);   //后面
	D3DXCreateTextureFromFile(m_pd3dDevice, pLeftTextureFile, &m_pTexture[2]);
	D3DXCreateTextureFromFile(m_pd3dDevice, pRightTextureFile, &m_pTexture[3]);
	D3DXCreateTextureFromFile(m_pd3dDevice, pTopTextureFile, &m_pTexture[4]);
	return TRUE;
}
VOID SkyBoxClass::RenderSky(D3DXMATRIX *pMatworld, BOOL bRenderFrame)
{
	m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTransform(D3DTS_WORLD,pMatworld);
	m_pd3dDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(SKYBOXVERTEX));
	m_pd3dDevice->SetFVF(D3DFVF_SKYBOX);
	for (int i = 0; i < 5;i++)
	{
		m_pd3dDevice->SetTexture(0,m_pTexture[i]);
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,i*4,2);
	}
	if (bRenderFrame)
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		for (int i = 0; i < 5; i++)
			m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,i*4,2);
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}
SkyBoxClass::~SkyBoxClass()
{
	SAFE_RELEASE(m_pVertexBuffer);
	for (int i = 0; i < 5; i++)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}
}