#pragma once
#include "D3DInit.h"
#include "D3DEffect.h"
#include"D3DCamera.h"

struct VertexPositionTex
{
	float x, y, z;
	float u, v;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
};

class CD3DWater
{
public:
	CD3DWater(LPDIRECT3DDEVICE9 pDevice,
	CameraClass*      pCamera,
	D3DPRESENT_PARAMETERS pD3PP);
	~CD3DWater();
	bool  Create(                                 // ����ˮ��
		float iSizeX,                             // ˮ�泤
		float iSizeY,                             // ˮ���
		float iPosX = 0.0f,                       // ������
		float iPosY = 0.0f,                       // ������
		float iHeight = 0.0f                      // ˮ��߶�
		);
	void  Draw(float gameTick);                   // ����ˮ��
	void  Release();                              // ��Դ�ͷ�
	void  BeginReflect();                         // �����������
	void  EndReflect();                           // ֹͣ�������
	void  BeginRefract();                         // �����������
	void  EndRefract();                           // ֹͣ�������
private:
	bool  LoadContent();                          // ������Դ
	void  CreateWaterVertices();                  // ���ɶ���
	void  GetParamHandles();                      // ��ȡ��Ч�������
	void  SetDefultParamValues();                 // ������Ч����Ĭ��ֵ
	void  ReSetParamValues();                     // ������Ч����ֵ
	void  FetchSurfaces();                        // ��ȡ���䡢�������

private:
	CD3DEffect*         m_pWaterEffect;           // ˮ����Ч
	IDirect3DTexture9*  m_pWaterWavTexture;       // ��������
	IDirect3DTexture9*  m_pReflectTexture;        // ��������
	IDirect3DTexture9*  m_pRefractTexture;        // ��������
	IDirect3DSurface9*  m_pReflectSurface;        // �������
	IDirect3DSurface9*  m_pRefractSurface;        // �������
	IDirect3DSurface9*  m_pOriginSurface;         // ԭʼ����
	IDirect3DVertexBuffer9*  m_pVB;               // ���㻺��
	D3DXPLANE           m_waterPlane;             // ˮƽ��
	D3DXPLANE           m_refWaterPlane;          // ����ˮƽ��
	D3DXMATRIX          m_worldMatrix;            // ԭʼ��������

private:
	D3DXHANDLE          m_hWorldMatrix;           // �������
	D3DXHANDLE          m_hViewMatrix;            // ��Ӱ����
	D3DXHANDLE          m_hProjMatrix;            // ͶӰ����
	D3DXHANDLE          m_hCameraPos;             // ��Ӱ��λ��
	D3DXHANDLE          m_hReflectMatrix;         // �������
	D3DXHANDLE          m_hWaveHeight;            // ˮ�����
	D3DXHANDLE          m_hWindForce;             // ����(ˮ������)
	D3DXHANDLE          m_hWindDirect;            // ����
	D3DXHANDLE          m_hWavTexture;            // ˮ������
	D3DXHANDLE          m_hWavTextureUVTile;      // ˮ������ƽ�̴���
	D3DXHANDLE          m_hReflectTexture;        // ��������
	D3DXHANDLE          m_hRefractTexture;        // ��������
	D3DXHANDLE          m_hTimeTick;              // ȫ��ʱ��
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	CameraClass*      m_pCamera;
	D3DPRESENT_PARAMETERS m_pD3PP;
};

