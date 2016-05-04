#pragma once
#include "Texture2D.h"
#include"D3DCamera.h"

// ��պж��㻺��ṹ����
struct VertexSkybox{
	VertexSkybox(){}
	VertexSkybox(float x, float y, float z, float nx, float ny, float nz, D3DCOLOR color, float u, float v){
		_x = x; _y = y; _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_color = color;
		_u = u; _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	D3DCOLOR _color;
	float _u, _v;
	static const DWORD FVF;
};

class CD3DSkyBox
{
public:
	CD3DSkyBox(CameraClass*       m_pCamera,
	LPDIRECT3DDEVICE9 m_pd3dDevice);
	~CD3DSkyBox();
public:
	BOOL LoadSkyTextureFromFiile(wchar_t *pFrontTextureFile,
		wchar_t *pBackTextureFile, 
		wchar_t *pLeftTextureFile, 
		wchar_t *pRightTextureFile, 
		wchar_t *pTopTextureFile,
		wchar_t *pBottomTexFile);
	void Draw();
	void Release();
private:
	void InitVerTices();
private:
	CTexture2D**            m_ppTextureArray;                   // ��պ���������
	VertexSkybox*           m_pVertex;                          // ���㻺��
	UINT16*                 m_pIndices;                         // ��������
	D3DXMATRIX              m_WorldTransMatrix;                 // ��ǰ����任����
	D3DXMATRIX              m_OriWorldTransMatrix;              // ԭʼ����任����
private:
	CameraClass*       m_pCamera;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
private:
	LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9  g_pIndexBuffer;
};

