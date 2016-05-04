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
	bool  Create(                                 // 构建水面
		float iSizeX,                             // 水面长
		float iSizeY,                             // 水面宽
		float iPosX = 0.0f,                       // 横坐标
		float iPosY = 0.0f,                       // 纵坐标
		float iHeight = 0.0f                      // 水面高度
		);
	void  Draw(float gameTick);                   // 绘制水面
	void  Release();                              // 资源释放
	void  BeginReflect();                         // 开启反射绘制
	void  EndReflect();                           // 停止反射绘制
	void  BeginRefract();                         // 开启折射绘制
	void  EndRefract();                           // 停止折射绘制
private:
	bool  LoadContent();                          // 加载资源
	void  CreateWaterVertices();                  // 生成顶点
	void  GetParamHandles();                      // 获取特效参数句柄
	void  SetDefultParamValues();                 // 设置特效参数默认值
	void  ReSetParamValues();                     // 重设特效参数值
	void  FetchSurfaces();                        // 获取反射、折射表面

private:
	CD3DEffect*         m_pWaterEffect;           // 水面特效
	IDirect3DTexture9*  m_pWaterWavTexture;       // 波纹纹理
	IDirect3DTexture9*  m_pReflectTexture;        // 反射纹理
	IDirect3DTexture9*  m_pRefractTexture;        // 折射纹理
	IDirect3DSurface9*  m_pReflectSurface;        // 反射表面
	IDirect3DSurface9*  m_pRefractSurface;        // 折射表面
	IDirect3DSurface9*  m_pOriginSurface;         // 原始表面
	IDirect3DVertexBuffer9*  m_pVB;               // 顶点缓冲
	D3DXPLANE           m_waterPlane;             // 水平面
	D3DXPLANE           m_refWaterPlane;          // 反向水平面
	D3DXMATRIX          m_worldMatrix;            // 原始世界坐标

private:
	D3DXHANDLE          m_hWorldMatrix;           // 世界矩阵
	D3DXHANDLE          m_hViewMatrix;            // 摄影矩阵
	D3DXHANDLE          m_hProjMatrix;            // 投影矩阵
	D3DXHANDLE          m_hCameraPos;             // 摄影机位置
	D3DXHANDLE          m_hReflectMatrix;         // 反射矩阵
	D3DXHANDLE          m_hWaveHeight;            // 水波振幅
	D3DXHANDLE          m_hWindForce;             // 风力(水波流速)
	D3DXHANDLE          m_hWindDirect;            // 风向
	D3DXHANDLE          m_hWavTexture;            // 水波纹理
	D3DXHANDLE          m_hWavTextureUVTile;      // 水波纹理平铺次数
	D3DXHANDLE          m_hReflectTexture;        // 反射纹理
	D3DXHANDLE          m_hRefractTexture;        // 折射纹理
	D3DXHANDLE          m_hTimeTick;              // 全局时间
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	CameraClass*      m_pCamera;
	D3DPRESENT_PARAMETERS m_pD3PP;
};

