#pragma once
#include "Texture2D.h"
class CD3DSprite
{
public:
	CD3DSprite(LPDIRECT3DDEVICE9 pDevice);
	~CD3DSprite();
public:
	void Begin(DWORD flags);
	void End();                              // 结束Sprite绘制
	void Draw(                               // 绘制Sprite
		CTexture2D* pTexture,                // 2D纹理
		const POINT& Position,               // 位置
		D3DCOLOR Color                       // 色相
		);
	void Draw(
		CTexture2D* pTexture,
		const POINT& Position,
		const RECT& SurRect,                 // 指定绘制纹理区域
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const POINT& Pos,
		const D3DXMATRIX& TransMatrix,       // 变换矩阵
		const RECT& SurRect,
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const POINT& Pos,
		const POINT& Size,                   // 绘制尺寸
		const RECT& SurRect,
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const RECT& DesRect,                 // 指定绘制目标区域
		const RECT& SurRect,
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const RECT& DesRect,
		D3DCOLOR Color
		);
public:
	ID3DXSprite* GetSprite(){ return m_pSprite; }   // 获得Sprite指针
private:
	ID3DXSprite* m_pSprite;                  // Sprite指针
	D3DXMATRIX   m_orgMatrix;                // 原始矩阵
};

