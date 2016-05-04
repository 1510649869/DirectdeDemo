#include "D3DGame.h"

//---HLSL全局变量句柄
D3DXHANDLE   g_CurrentTechHandle = NULL;
D3DXHANDLE   g_matWorldViewProj = NULL;
D3DXHANDLE   g_matWorld = NULL;
D3DXHANDLE   g_vecEye = NULL;
D3DXHANDLE   g_vecLightDir = NULL;
D3DXHANDLE   g_vDiffuseColor = NULL;
D3DXHANDLE   g_vSpecularColor = NULL;
D3DXHANDLE   g_vAmbient = NULL;
D3DXMATRIX   g_matProjection;
static float fogstart = 120;
static float fogend = 1000;


const DWORD VertexPosTex::FVF = (D3DFVF_XYZ | D3DFVF_TEX0);

CD3DGame::CD3DGame()
{
	g_pDInput  = NULL;
	g_pModel   = NULL;
	g_pDInput  = NULL;
	g_pEffect  = NULL;
	g_pSpreite = NULL;
	g_pTexture = NULL;
	g_pSkyBox  = NULL;
	g_pTerrain = NULL;
	g_pWater   = NULL;
}
//卸载设备
VOID CD3DGame::UnLoadContent()
{
	SAFE_DELETE(g_pDInput);
	SAFE_DELETE(g_pModel);
	SAFE_DELETE(g_pCamera);
	SAFE_DELETE(g_pEffect);
	SAFE_DELETE(g_pSpreite);
	SAFE_DELETE(g_pTexture);
	SAFE_DELETE(g_pSkyBox);
	SAFE_DELETE(g_pTerrain);
	SAFE_DELETE(g_pWater);

	SAFE_RELEASE(BkGndQuad);
	SAFE_RELEASE(BkGndTex);
	SAFE_RELEASE(TeaPot);
}
void CD3DGame::GetParameters()
{
	g_CurrentTechHandle =  g_pEffect->GetEffect()->GetTechniqueByName("SpecularLight");
	g_matWorldViewProj  =  g_pEffect->GetEffect()->GetParameterByName(0,"matWorldViewProj");
	g_matWorld          =  g_pEffect->GetEffect()->GetParameterByName(0, "matWorld");
	g_vecEye            =  g_pEffect->GetEffect()->GetParameterByName(0, "vecEye");
	g_vecLightDir       =  g_pEffect->GetEffect()->GetParameterByName(0, "vecLightDir");
	g_vDiffuseColor     =  g_pEffect->GetEffect()->GetParameterByName(0, "vDiffuseColor");
	g_vSpecularColor    =  g_pEffect->GetEffect()->GetParameterByName(0, "vSpecularColor");
	g_vAmbient          =  g_pEffect->GetEffect()->GetParameterByName(0, "vAmbient");
}
void CD3DGame::SetParameters()
{
	g_pEffect->GetEffect()->SetTechnique(g_CurrentTechHandle);
	
	D3DXMATRIX matView,mProjection;
	g_pCamera->GetProjMatrix(&mProjection);
	g_pCamera->CalculateViewMatrix(&matView);
	D3DXMATRIX worldMatrix;
	D3DXMatrixTranslation(&worldMatrix, 0.0f, -1200.0f, 0.0f);
	g_pEffect->GetEffect()->SetMatrix(g_matWorldViewProj, &(worldMatrix*matView*mProjection));
	g_pEffect->GetEffect()->SetMatrix(g_matWorld, &worldMatrix);
	
	D3DXVECTOR3 cameraPos;
	g_pCamera->GetCameraPostion(&cameraPos);
	D3DXVECTOR4 vecEye = D3DXVECTOR4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	g_pEffect->GetEffect()->SetVector(g_vecEye, &vecEye);
	
	
	D3DXVECTOR4 vLightDirection = D3DXVECTOR4(0.0f, 0.0f, -1.0f, 1.0f);
	g_pEffect->GetEffect()->SetVector(g_vecLightDir, &vLightDirection);
	
	D3DXVECTOR4 vColorDiffuse  = D3DXVECTOR4(0.8f, 0.0f, 0.0f, 1.0f);
	D3DXVECTOR4 vColorSpecular = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXVECTOR4 vColorAmbient  = D3DXVECTOR4(0.1f, 0.1f, 0.1f, 1.0f);
	g_pEffect->GetEffect()->SetVector(g_vDiffuseColor, &vColorDiffuse);
	//g_pEffect->GetEffect()->SetVector(g_vSpecularColor, &vColorSpecular);
	g_pEffect->GetEffect()->SetVector(g_vAmbient, &vColorAmbient);
}
CD3DGame::~CD3DGame()
{
}

VOID CD3DGame::HardDeviceContent()
{
	g_pDInput = new DInputClass();
	g_pDInput->Init(hWnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	g_pCamera = new CameraClass(g_pd3dDevice);
	g_pCamera->SetCameraPostion(&D3DXVECTOR3(0.0f, 0.0f, -200.0f));
	g_pCamera->SetTargetPostion(&D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	g_pCamera->SetViewMatrix();
	g_pCamera->SetProjMatrix();
}
//加载一些设备
HRESULT CD3DGame::LoadContent()
{
	HardDeviceContent();
	g_pModel = new ModelClass(g_pd3dDevice);
	g_pModel->LoadModelFromFileX(L"loli.X");
	
	g_pTexture = new CTexture2D(g_pd3dDevice);
	g_pTexture->LoadTexture(L"img.jpg");
	g_pSpreite = new CD3DSprite(g_pd3dDevice);

	g_pSkyBox = new CD3DSkyBox(g_pCamera, g_pd3dDevice);
	g_pSkyBox->LoadSkyTextureFromFiile(L"Skybox\\frontsnow1.jpg", 
		L"Skybox\\backsnow1.jpg",
		L"Skybox\\leftsnow1.jpg", 
		L"Skybox\\rightsnow1.jpg",
		L"Skybox\\topsnow1.jpg", 
		L"Skybox\\topsnow1.jpg");

	g_pTerrain = new CD3DTerrain(g_pd3dDevice);
	g_pTerrain->LoadTerrainFromFile(L"Terrain\\heighmap.raw", L"Terrain\\r_grass.dds");
	g_pTerrain->InitTerrain(200, 200, 100.0f, 10.0f);

	g_pWater = new CD3DWater(g_pd3dDevice, g_pCamera, g_pd3dpp);
	g_pWater->Create(200, 200, 0.0f, 0.0f, 40.0f);
	g_pEffect = new CD3DEffect(g_pd3dDevice);
	char ErrMsg[60];
	// 加载fx特效
	if (!g_pEffect->LoadEffect(L"Light.fx", ErrMsg))
		return E_FAIL;
	GetParameters();

	::ZeroMemory(&TeaPotMtrl, sizeof(TeaPotMtrl));
	TeaPotMtrl.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	TeaPotMtrl.Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
	TeaPotMtrl.Specular= D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXCreateTeapot(g_pd3dDevice, &TeaPot, 0);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, true);
	return S_OK;
}
//逻辑更新
VOID CD3DGame::Update()
{
	g_pDInput->GetInput();
	if (g_pDInput->IsKeyDown(DIK_A))      g_pCamera->MoveAlongRightVec(-3.0f);
	if (g_pDInput->IsKeyDown(DIK_D))      g_pCamera->MoveAlongRightVec(3.0f);
	if (g_pDInput->IsKeyDown(DIK_W))      g_pCamera->MoveAlongLookVec(3.0f);
	if (g_pDInput->IsKeyDown(DIK_S))      g_pCamera->MoveAlongLookVec(-3.0f);
	if (g_pDInput->IsKeyDown(DIK_R))      g_pCamera->MoveAlongUpVec(3.0f);
	if (g_pDInput->IsKeyDown(DIK_F))      g_pCamera->MoveAlongUpVec(-3.0f);
	if (g_pDInput->IsKeyDown(DIK_Q))      g_pCamera->RotationLookVec(0.0003f);
	if (g_pDInput->IsKeyDown(DIK_E))      g_pCamera->RotationLookVec(-0.0003f);
	if (g_pDInput->IsMouseButtonDown(0))
	{
		g_pCamera->RotationUpVec(g_pDInput->MouseDX() * 0.001f);
		g_pCamera->RotationRightVec(g_pDInput->MouseDY()*0.001f);
	}
	D3DXMATRIX matView;
	g_pCamera->GetProjMatrix(&g_matProjection);
    g_pCamera->CalculateViewMatrix(&matView);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	if (g_pDInput->IsKeyDown(DIK_1))
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_POINT);
	}
	if (g_pDInput->IsKeyDown(DIK_2))
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	if (g_pDInput->IsKeyDown(DIK_3))
	{
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}

//额外添加函数，用来进行固定渲染函数的测试
VOID CD3DGame::FixedRender()
{

}
//物体渲染
VOID CD3DGame::D3DRender()
{
	SetParameters();
	BeginRender();
	POINT pos;
	pos.x = 0;
	pos.y = 0;
	//添加其他的
    g_pSkyBox->Draw();
	//BeginEffect();
	D3DXMATRIX W;
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	D3DXMatrixScaling(&W, 1.5f, 1.5f, 1.5f); 
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &W);
	g_pd3dDevice->SetMaterial(&TeaPotMtrl);
	g_pd3dDevice->SetTexture(0,0);
	TeaPot->DrawSubset(0);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
	//EndEffect();

	// Sprite绘制开始
	g_pSpreite->Begin(D3DXSPRITE_ALPHABLEND);
	// Sprite绘制
	//g_pSpreite->Draw(g_pTexture, pos, g_pTexture->GetRect(), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	// Sprite绘制结束
	g_pSpreite->End();
	// 绘制水体表面
	//g_pWater->Draw(1);
	UINT numPasses;
	// 开启特效
	g_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR);
	g_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)&fogstart);
	g_pd3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)&fogend);


	g_pEffect->BeginEffect(numPasses);
	for (UINT i = 0; i<numPasses; i++)
	{
		// 开启路径
		g_pEffect-> GetEffect()->BeginPass(i);
		
		g_pModel -> RenderModel(D3DXVECTOR3(0,1, 0));
		g_pTerrain->Draw();
		// 路径结束
		g_pEffect->GetEffect()->EndPass();
	}
	// 特效结束
	g_pEffect->EndEffect();
	EndEffect();
	EndRender();
}
//固定渲染管线路线
VOID CD3DGame::BeginEffect()
{
	D3DXVECTOR3 dir(-0.0f, -1.0f, 1.0f);
	D3DXCOLOR   col(1.0f, 1.0f, 0.9f, 1.0f);
	D3DLIGHT9 light = InitDirectionalLight(&dir, &col);
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->LightEnable(0, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	//背面消去隐藏
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
VOID CD3DGame::EndEffect()
{
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
}
D3DLIGHT9 CD3DGame::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient    =  *color * 0.7f;
	light.Diffuse    =  *color * 0.5f;
	light.Specular   =  *color;		  
	light.Direction  =  *direction;
	return light;
}
