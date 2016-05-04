#include "D3DWater.h"

D3DXVECTOR4 Vector3To4(const D3DXVECTOR3& vector3){ return D3DXVECTOR4(vector3.x, vector3.y, vector3.z, 0.0f); }
CD3DWater::CD3DWater(LPDIRECT3DDEVICE9 pDevice,
	CameraClass*      pCamera,
	D3DPRESENT_PARAMETERS pD3PP) : m_pWaterEffect(NULL),
m_pWaterWavTexture(NULL),
m_pReflectTexture(NULL),
m_pRefractTexture(NULL),
m_pReflectSurface(NULL),
m_pRefractSurface(NULL),
m_pOriginSurface(NULL),
m_pVB(NULL)
{
	m_pWaterEffect = new CD3DEffect;
	m_pD3PP = pD3PP;
	m_pDevice = pDevice;
	m_pCamera = pCamera;
}
CD3DWater::~CD3DWater()
{
	Release();
}
bool CD3DWater::Create(float iSizeX, float iSizeY,
	float iPosX ,                       // 横坐标
	float iPosY ,                       // 纵坐标
	float iHeight )
{
	if (!LoadContent())
		return false;
	FetchSurfaces();//获得折射反射渲染表面
	if (FAILED(m_pDevice->CreateVertexBuffer(6 * sizeof(VertexPositionTex),
		D3DUSAGE_WRITEONLY,
		VertexPositionTex::FVF,
		D3DPOOL_DEFAULT,
		&m_pVB,
		0)))
		return false;
	VertexPositionTex* pVertices;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = VertexPositionTex{ iPosX, iHeight, iPosY + iSizeY, 0, 0 };
	pVertices[1] = VertexPositionTex{ iPosX + iSizeX, iHeight, iPosY + iSizeY, 1, 0 };
	pVertices[2] = VertexPositionTex{ iPosX, iHeight, iPosY, 0, 1 };

	pVertices[3] = VertexPositionTex{ iPosX + iSizeX, iHeight, iPosY + iSizeY, 1, 0 };
	pVertices[4] = VertexPositionTex{ iPosX + iSizeX, iHeight, iPosY, 1, 1 };
	pVertices[5] = VertexPositionTex{ iPosX, iHeight, iPosY, 0, 1 };
	m_pVB->Unlock();

	//设置摄像机反射面

	//创建折射 反射横切面
	D3DXPlaneFromPointNormal(&m_waterPlane, &D3DXVECTOR3(iPosX, iHeight, iPosY), &D3DXVECTOR3(0,1,0));
	D3DXPlaneNormalize(&m_waterPlane, &m_waterPlane);
	return true;
}
bool CD3DWater::LoadContent()
{
	char* errage=NULL;
	if (!m_pWaterEffect->LoadEffect(L"Water.fx", errage))
		return false;
	HRESULT hr = D3DXCreateTextureFromFile(m_pDevice, L"Water\\r_WaterWav.jpg", &m_pWaterWavTexture);
	if (FAILED(hr))
		return false;
	GetParamHandles();
	ReSetParamValues();
	return true;

}
void CD3DWater::FetchSurfaces()
{
	// 创建反射纹理并获得其渲染表面
	D3DXCreateTexture(m_pDevice, m_pD3PP.BackBufferWidth, m_pD3PP.BackBufferWidth,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pReflectTexture);
	m_pReflectTexture->GetSurfaceLevel(0, &m_pReflectSurface);

	// 创建折射纹理并获得其渲染表面
	D3DXCreateTexture(m_pDevice, m_pD3PP.BackBufferWidth, m_pD3PP.BackBufferHeight, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRefractTexture);

	m_pRefractTexture->GetSurfaceLevel(0, &m_pRefractSurface);
}

void CD3DWater::GetParamHandles()
{
	m_hWorldMatrix   = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWorld");
	m_hViewMatrix    = m_pWaterEffect->GetEffect()->GetAnnotationByName(0, "gView");
	m_hProjMatrix    = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gProjection");
	m_hCameraPos     = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gCameraPos");
	m_hReflectMatrix = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gReflectionView");
	m_hWaveHeight    = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWaveHeight");
	m_hWindForce     = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWindForce");
	m_hWindDirect    = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gWindDirection");
	m_hWavTexture    = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gTexture1");
	m_hWavTextureUVTile = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gTexture1UVTile");
	m_hReflectTexture   = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gReflectionMap");
	m_hRefractTexture   = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gRefractionMap");
	m_hTimeTick         = m_pWaterEffect->GetEffect()->GetParameterByName(0, "gTime");
}
void CD3DWater::SetDefultParamValues()
{
	D3DXHANDLE WaterTechnique = m_pWaterEffect->GetEffect()->GetTechniqueByName("Water");
	m_pWaterEffect->GetEffect()->SetTechnique(WaterTechnique);

	m_pWaterEffect->GetEffect()->SetTexture(m_hWavTexture, m_pWaterWavTexture);
	m_pWaterEffect->GetEffect()->SetFloat(m_hWavTextureUVTile, 10.0f);
	m_pWaterEffect->GetEffect()->SetFloat(m_hWaveHeight, 0.3f);
	m_pWaterEffect->GetEffect()->SetFloat(m_hWindForce, 0.02f);
	m_pWaterEffect->GetEffect()->SetVector(m_hWindDirect, &Vector3To4(D3DXVECTOR3(1.0f,1.0f,1.0f)));
}
void CD3DWater::ReSetParamValues()
{
	m_pDevice->GetTransform(D3DTS_WORLD, &m_worldMatrix);
	m_pWaterEffect->GetEffect()->SetMatrix(m_hWorldMatrix, &m_worldMatrix);

	D3DXMATRIX matView, mProjection;
	m_pCamera->GetProjMatrix(&mProjection);
	m_pCamera->CalculateViewMatrix(&matView);
	D3DXMATRIX worldMatrix;
	D3DXMatrixTranslation(&worldMatrix, 0.0f, -1200.0f, 0.0f);

	m_pWaterEffect->GetEffect()->SetMatrix(m_hViewMatrix, &matView);
	m_pWaterEffect->GetEffect()->SetMatrix(m_hProjMatrix, &mProjection);

	D3DXVECTOR3 cameraPos;
	m_pCamera->GetCameraPostion(&cameraPos);
	D3DXVECTOR4 vecEye = D3DXVECTOR4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);

	m_pWaterEffect->GetEffect()->SetVector(m_hCameraPos, &Vector3To4(cameraPos));
	
	m_pWaterEffect->GetEffect()->SetMatrix(m_hReflectMatrix, &m_pCamera->GetReflectMatrix());
	
	m_pWaterEffect->GetEffect()->SetTexture(m_hReflectTexture, m_pReflectTexture);
	m_pWaterEffect->GetEffect()->SetTexture(m_hRefractTexture, m_pRefractTexture);
}
void CD3DWater::Draw(float gameTick)
{
	// 重置特效参数
	ReSetParamValues();
	m_pWaterEffect->GetEffect()->SetFloat(m_hTimeTick, gameTick / 800);

	// 应用特效
	UINT numPasses;
	m_pWaterEffect->BeginEffect(numPasses);
	for (UINT i = 0; i<numPasses; i++)
	{
		m_pWaterEffect->GetEffect()->BeginPass(i);

		// 顶点绘制
		m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(VertexPositionTex));
		m_pDevice->SetFVF(VertexPositionTex::FVF);
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

		m_pWaterEffect->GetEffect()->EndPass();
	}
	m_pWaterEffect->EndEffect();
}

void CD3DWater::BeginReflect()
{
	// 设定摄影矩阵为摄影机反视矩阵
	m_pDevice->SetTransform(D3DTS_VIEW, &m_pCamera->GetReflectMatrix());
	// 设定3D设备横切面，仅反射水面之上的景象
	m_pDevice->SetClipPlane(0, m_waterPlane);
	m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	// 保留原始渲染目标
	m_pDevice->GetRenderTarget(0, &m_pOriginSurface);
	// 设定新渲染目标为反射贴图表面
	m_pDevice->SetRenderTarget(0, m_pReflectSurface);
	// 清空反射表面缓存
	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 149, 237, 255), 1.0f, 0);
}

void CD3DWater::EndReflect()
{
	// 还原渲染目标
	m_pDevice->SetRenderTarget(0, m_pOriginSurface);
	// 禁用横切面
	m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	// 还原摄影矩阵
	D3DXMATRIX viewMatrix;
	m_pCamera->CalculateViewMatrix(&viewMatrix);
	m_pDevice->SetTransform(D3DTS_VIEW, &viewMatrix);
}

void CD3DWater::BeginRefract()
{
	// 设定3D设备横切面，仅折射水面以下的景象
	m_pDevice->SetClipPlane(0, -m_waterPlane);
	m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	// 保留原始渲染目标
	m_pDevice->GetRenderTarget(0, &m_pOriginSurface);
	// 设定新渲染目标为折射贴图表面
	m_pDevice->SetRenderTarget(0, m_pRefractSurface);
	// 清空折射表面缓存
	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 149, 237, 255), 1.0f, 0);
}

void CD3DWater::EndRefract()
{
	// 还原渲染目标
	m_pDevice->SetRenderTarget(0, m_pOriginSurface);
	// 禁用横切面
	m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
}

void CD3DWater::Release()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pReflectSurface);
	SAFE_RELEASE(m_pRefractSurface);
	SAFE_RELEASE(m_pReflectTexture);
	SAFE_RELEASE(m_pRefractTexture);
	SAFE_RELEASE(m_pWaterWavTexture);
	SAFE_DELETE (m_pWaterEffect);
	m_pCamera = NULL;
	m_pDevice = NULL;
}
