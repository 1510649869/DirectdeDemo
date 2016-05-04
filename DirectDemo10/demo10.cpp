#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <tchar.h>
#pragma comment(lib,"winmm.lib")
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"QWnd"
#define WINDOW_CLASS L"WndClass"
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}
#define SAFE_DELETE(p)  {if(p){delete(p);(p)=NULL;}}

struct VERTEXPOSTEX 
{
	float x, y, z;
	float u, v;
	VERTEXPOSTEX(float _x, float _y, float _z, float _u, float _v) :x(_x), y(_y), z(_z),
		u(_u), v(_v)
	{
	}
};
#define  D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
LPD3DXMESH TeaPot = 0;
D3DMATERIAL9 TeaPotMtrl;
D3DMATERIAL9 BoxMtrl;

LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
LPDIRECT3DINDEXBUFFER9  g_pIndexBuffer = NULL;
LPDIRECT3DTEXTURE9  g_pTexture=NULL;

//�������������Ϣ
LPD3DXMESH g_pMesh = NULL;
D3DMATERIAL9* g_pMaterials = NULL;
LPDIRECT3DTEXTURE9* g_pTextures = NULL;
DWORD g_NumMtrls=0;

LPD3DXMESH g_pWallMesh=NULL;
D3DMATERIAL9 g_wallMaterial;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT D3DDeviceInit(HWND hwnd)
{
	LPDIRECT3D9 pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;
	D3DCAPS9 caps;
	int flag = 0;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
		return E_FAIL;
	if (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		flag = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		flag = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;	//��Ⱦ��̨����ĸ�ʽ
	d3dpp.BackBufferCount = 1;				    //��Ҫ������Ⱦ�ĺ�̨��������
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; //������Ч��
	d3dpp.hDeviceWindow = hwnd;				  //WinMain�д����Ĵ��ھ��
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hwnd, flag, &d3dpp, &g_pd3dDevice)))
		return E_FAIL;
	SAFE_RELEASE(pD3D);//�ͷŵ�Direct3D����
	return TRUE;
}
VOID LightSetting(DWORD nType)
{
	//����nType��ѡ��ͬ�Ĺ���
	static D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	switch (nType)
	{
	case 1:	  //���Դ
		light.Type = D3DLIGHT_POINT;
		light.Ambient = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
		light.Position = D3DXVECTOR3(0.0f, 200.0f, 0.0f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range = 300.0f;
		break;
	case 2:	  //ƽ�й�
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		break;
	case 3:	  //�۹��
		light.Type      = D3DLIGHT_SPOT;
		light.Position  = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
		light.Direction = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
		light.Ambient   = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
		light.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light.Specular  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Range   = 300.0f;
		light.Falloff = 0.1f;
		light.Phi   = D3DX_PI / 3.0f;
		light.Theta = D3DX_PI / 6.0f;
		break;
	default:
		break;
	}
	UINT time = GetTickCount();
	float f = 2.f*D3DX_PI*time / 1000.f;
	light.Position = D3DXVECTOR3(20 * sinf(f), 50, 20 * cosf(f));
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, true);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(30, 30, 30));
}
VOID SetUp()
{
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); 
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}
VOID TextureSetting()
{
	
}
VOID LoadMesh()
{
	LPD3DXBUFFER pAdjBuffer = NULL;
	LPD3DXBUFFER pMtrlBuffer = NULL;
	D3DXLoadMeshFromX(L"lighting.X", D3DXMESH_MANAGED,
		g_pd3dDevice, &pAdjBuffer, &pMtrlBuffer, 
		NULL, &g_NumMtrls, &g_pMesh);
	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	g_pMaterials = new D3DMATERIAL9[g_NumMtrls];
	g_pTextures = new LPDIRECT3DTEXTURE9[g_NumMtrls];
	for (DWORD i = 0; i < g_NumMtrls; i++)
	{
		g_pMaterials[i] = pMtrls[i].MatD3D;
		g_pMaterials[i].Ambient = g_pMaterials[i].Diffuse;
		g_pTextures[i] = NULL;
		D3DXCreateTextureFromFileA(g_pd3dDevice, pMtrls[i].pTextureFilename, &g_pTextures[i]);
	}
	SAFE_RELEASE(pAdjBuffer);
	SAFE_RELEASE(pMtrlBuffer);

	//��D3DXCreateBox������һ�������ľ���
	D3DXCreateBox(g_pd3dDevice, 120.0f, 120.0f, 0.3f, &g_pWallMesh, NULL);
	g_wallMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_wallMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_wallMaterial.Specular= D3DXCOLOR(0.2f, 1.0f, 1.0f, 1.0f);

}
HRESULT CreateBox()
{
	//�������㻺��
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(8 * sizeof(VERTEXPOSTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVertexBuffer, NULL)))
	{
		return E_FAIL;
	}
	// ������������
	if (FAILED(g_pd3dDevice->CreateIndexBuffer(36 * sizeof(WORD), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIndexBuffer, NULL)))
	{
		return E_FAIL;
	}

	//��䶥�㻺��  
	VERTEXPOSTEX* pVertices;
	if (FAILED(g_pVertexBuffer->Lock(0, sizeof(VERTEXPOSTEX), (void**)&pVertices, 0)))
		return E_FAIL;

	// ���涥������  
	pVertices[0] = VERTEXPOSTEX(-10.0f, 10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[1] = VERTEXPOSTEX(10.0f, 10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[2] = VERTEXPOSTEX(10.0f, -10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[3] = VERTEXPOSTEX(-10.0f, -10.0f, -10.0f, 0.0f, 1.0f);

	// ���涥������  
	pVertices[4] = VERTEXPOSTEX(10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[5] = VERTEXPOSTEX(-10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[6] = VERTEXPOSTEX(-10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[7] = VERTEXPOSTEX(10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// ���涥������  
	pVertices[8] = VERTEXPOSTEX(-10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[9] = VERTEXPOSTEX(10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[10] = VERTEXPOSTEX(10.0f, 10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[11] = VERTEXPOSTEX(-10.0f, 10.0f, -10.0f, 0.0f, 1.0f);

	// ���涥������  
	pVertices[12] = VERTEXPOSTEX(-10.0f, -10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[13] = VERTEXPOSTEX(10.0f, -10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[14] = VERTEXPOSTEX(10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[15] = VERTEXPOSTEX(-10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// ����涥������  
	pVertices[16] = VERTEXPOSTEX(-10.0f, 10.0f, 10.0f, 0.0f, 0.0f);
	pVertices[17] = VERTEXPOSTEX(-10.0f, 10.0f, -10.0f, 1.0f, 0.0f);
	pVertices[18] = VERTEXPOSTEX(-10.0f, -10.0f, -10.0f, 1.0f, 1.0f);
	pVertices[19] = VERTEXPOSTEX(-10.0f, -10.0f, 10.0f, 0.0f, 1.0f);

	// �Ҳ��涥������  
	pVertices[20] = VERTEXPOSTEX(10.0f, 10.0f, -10.0f, 0.0f, 0.0f);
	pVertices[21] = VERTEXPOSTEX(10.0f, 10.0f, 10.0f, 1.0f, 0.0f);
	pVertices[22] = VERTEXPOSTEX(10.0f, -10.0f, 10.0f, 1.0f, 1.0f);
	pVertices[23] = VERTEXPOSTEX(10.0f, -10.0f, -10.0f, 0.0f, 1.0f);
	g_pVertexBuffer->Unlock();

	// �����������  
	WORD *pIndices = NULL;
	g_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);

	// ������������  
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
	pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

	// ������������  
	pIndices[6] = 4; pIndices[7] = 5; pIndices[8] = 6;
	pIndices[9] = 4; pIndices[10] = 6; pIndices[11] = 7;

	// ������������  
	pIndices[12] = 8; pIndices[13] = 9; pIndices[14] = 10;
	pIndices[15] = 8; pIndices[16] = 10; pIndices[17] = 11;

	// ������������  
	pIndices[18] = 12; pIndices[19] = 13; pIndices[20] = 14;
	pIndices[21] = 12; pIndices[22] = 14; pIndices[23] = 15;

	// �������������  
	pIndices[24] = 16; pIndices[25] = 17; pIndices[26] = 18;
	pIndices[27] = 16; pIndices[28] = 18; pIndices[29] = 19;
	// �Ҳ�����������  
	pIndices[30] = 20; pIndices[31] = 21; pIndices[32] = 22;
	pIndices[33] = 20; pIndices[34] = 22; pIndices[35] = 23;
	g_pIndexBuffer->Unlock();
	//����������ͼ
	if (FAILED(D3DXCreateTextureFromFileEx(
		g_pd3dDevice,
		L"pal5q.jpg",
		0, 0,
		2,//�����������м���
		0,//����ʹ�÷�ʽ һ��Ϊ0
		D3DFMT_X8R8G8B8,//�����ʽ
		D3DPOOL_MANAGED,//����λ��
		D3DX_DEFAULT,//������˷�ʽ
		D3DX_DEFAULT,//�������������й��˷�ʽ
		0, //͸��ɫ
		0,//ͼ���ļ���Ϣ��ַ
		0,//��ɫ����Ϣ��ַ
		&g_pTexture)))
	{
		return E_FAIL;
	}
}
HRESULT ObjectInit(HWND hWnd)
{
	::ZeroMemory(&TeaPotMtrl, sizeof(TeaPotMtrl));
	TeaPotMtrl.Ambient  = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	TeaPotMtrl.Diffuse  = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	TeaPotMtrl.Specular = D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f);
	TeaPotMtrl.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	TeaPotMtrl.Power = 10;
	::ZeroMemory(&BoxMtrl, sizeof(TeaPotMtrl));
	BoxMtrl.Ambient  =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	BoxMtrl.Diffuse  =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	BoxMtrl.Specular =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	BoxMtrl.Emissive =  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	BoxMtrl.Power    =  10;

	if (E_FAIL == D3DXCreateTeapot(g_pd3dDevice, &TeaPot, NULL))
		return E_FAIL;
	CreateBox();

	LoadMesh();

	SetUp();
	LightSetting(1);
	return TRUE;
}
void MatrixSetting()
{
	D3DXMATRIX matView, matProjection;
	D3DXVECTOR3 vEye(50.0f, 0.0f, -200.0f);
	D3DXVECTOR3 vAt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEye, &vAt, &vUp);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4.0f, 1.0f, 1.0f, 100000);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjection);
}
void Update(HWND hwnd)
{
	if (::GetAsyncKeyState('Q') & 0x8000f)         // �������ϵİ���Q������,��Դ������Ϊ���Դ
		LightSetting(1);
	if (::GetAsyncKeyState(0x57) & 0x8000f)         // �������ϵİ���W�����£���Դ������Ϊƽ�й�Դ
		LightSetting(2);
	if (::GetAsyncKeyState(0x45) & 0x8000f)         // �������ϵİ���E�����£���Դ������Ϊ�۹��
		LightSetting(3);
	if (::GetAsyncKeyState('1') & 0x8000f)
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	if (::GetAsyncKeyState('2') & 0x8000f)
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (::GetAsyncKeyState('3') & 0x8000f)
		TeaPotMtrl.Diffuse.a += 0.001f;
	if (::GetAsyncKeyState('4')& 0x8000f)
		TeaPotMtrl.Diffuse.a -= 0.001f;
	if (TeaPotMtrl.Diffuse.a > 1.0f)
		TeaPotMtrl.Diffuse.a = 1.0f;
	if (TeaPotMtrl.Diffuse.a < 0.0f)
		TeaPotMtrl.Diffuse.a = 0.0f;
	if (GetAsyncKeyState('A') & 0x8000)
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}
	if (::GetAsyncKeyState('Z') & 0x8000f)
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
	// set border color address mode 
	if (::GetAsyncKeyState('X') & 0x8000f)
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x000000ff);
	}
	// set clamp address mode 
	if (::GetAsyncKeyState('C') & 0x8000f)
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	// set mirror address mode 
	if (::GetAsyncKeyState('V') & 0x8000f)
	{
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
	}
	if (::GetAsyncKeyState('R') & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	}
	if (::GetAsyncKeyState('F') & 0x8000f)
	{
		g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}

}
void RenderObject()
{
	D3DXMATRIX matHero, matWorld, matRotation;   //����һЩ����	


	//����3Dģ��
	D3DXMatrixTranslation(&matHero, -20.0f, 0.0f, -65.0f);
	matHero = matHero;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matHero);//����ģ�͵��������Ϊ������׼��
	// ��һ��forѭ��������ģ�͵�����������ֵĻ���
	for (DWORD i = 0; i < g_NumMtrls; i++)
	{
		g_pd3dDevice->SetMaterial(&g_pMaterials[i]);  //���ô˲��ֵĲ���
		g_pd3dDevice->SetTexture(0, g_pTextures[i]);//���ô˲��ֵ�����
		g_pMesh->DrawSubset(i);  //���ƴ˲���
	}

	//D3DXMATRIX matWorld;   //����һЩ����	

	// ���Ƴ�����
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);//��ǽ�����������ʼ��
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  //��ǽ����������
		g_pd3dDevice->SetMaterial(&g_wallMaterial);//���ò���
	g_pWallMesh->DrawSubset(0); //����ǽ��


	//3. ����ģ�建�棬�Լ�����صĻ���״̬�������á�
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// 4.�����ںϲ������Լ���ֹ����Ȼ���ͺ�̨����д����
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// 5.���Ƴ���Ϊ���������
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetMaterial(&g_wallMaterial);
	g_pWallMesh->DrawSubset(0);

	// 6.��������һϵ����Ⱦ״̬���������뾵������ں����㣬������һ��Z����
	g_pd3dDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//7. ���㾵��任����
	D3DXMATRIX matReflect;
	D3DXPLANE planeXY(0.0f, 0.0f, 1.0f, 0.0f); // xyƽ��
	D3DXMatrixReflect(&matReflect, &planeXY);
	matWorld = matReflect * matHero;


	//8.���ƾ����е�3Dģ��
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);//����ģ�͵��������Ϊ������׼��
	// ��һ��forѭ��������ģ�͵�����������ֵĻ���
	for (DWORD i = 0; i < g_NumMtrls; i++)
	{
		g_pd3dDevice->SetMaterial(&g_pMaterials[i]);  //���ô˲��ֵĲ���
		g_pd3dDevice->SetTexture(0, g_pTextures[i]);//���ô˲��ֵ�����
		g_pMesh->DrawSubset(i);  //���ƴ˲���
	}


	// 9.�ָ���Ⱦ״̬
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
void Render(HWND hwnd)
{
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_pd3dDevice->BeginScene();
	MatrixSetting();
	RenderObject();
	//D3DXMATRIX matWorld2,matRotate;//, matScale;
	//D3DXMatrixTranslation(&matWorld2, 0.0f, 0.0f, 30.0f);
	//const D3DXVECTOR3 vec(0.0f, 1.0f, 0.0f);
	//D3DXMatrixRotationAxis(&matRotate, &vec, D3DX_PI / 4.0f);
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &(matWorld2*matRotate));
	//g_pd3dDevice->SetMaterial(&BoxMtrl);
	//g_pd3dDevice->SetTexture(0, g_pTexture);
	//g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(VERTEXPOSTEX));//�Ѱ����ļ�������Ϣ�Ķ��㻺�����Ⱦ��ˮ�������
	//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);//ָ������ʹ�õ������ʽ�ĺ�����
	//g_pd3dDevice->SetIndices(g_pIndexBuffer);//������������
	//g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);//��������������϶��㻺�����ͼ��

	//g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	//g_pd3dDevice->SetMaterial(&TeaPotMtrl);
	//g_pd3dDevice->SetTexture(0, 0);
	//D3DXMATRIX matWorld, matScale;
	//D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
	//D3DXMatrixScaling(&matScale, 10.0f, 10.0f, 10.0f);
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &(matWorld*matScale));//����ģ�͵��������Ϊ������׼��
	//TeaPot->DrawSubset(0);

	//g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	g_pd3dDevice->EndScene();
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}
void Destory()
{
	SAFE_RELEASE(g_pd3dDevice);
	SAFE_RELEASE(TeaPot);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pTexture);
}
int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	WNDCLASSEX WndClass = { 0 };
	WndClass.cbSize = sizeof(WNDCLASSEX);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = WINDOW_CLASS;
	if (!RegisterClassEx(&WndClass))
		return -1;
	HWND hwnd = CreateWindow(WINDOW_CLASS, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
	if (E_FAIL == D3DDeviceInit(hwnd))
	{
		MessageBox(hwnd, L"ERROR:Can't Init The Device!", NULL, 1);
		return 0;
	}
	if (E_FAIL == ObjectInit(hwnd))
	{
		MessageBox(hwnd, L"ERROR:Can not Init Object", NULL, 1);
		return 0;
	}
	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update(hwnd);
			Render(hwnd);
		}
	}
	UnregisterClass(WINDOW_CLASS, hInstance);
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		Destory();
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		Render(hWnd);
		ValidateRect(hWnd, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
}