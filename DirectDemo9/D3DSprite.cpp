#include "D3DSprite.h"


CD3DSprite::CD3DSprite(LPDIRECT3DDEVICE9 pDevice)
{
	D3DXCreateSprite(pDevice, &m_pSprite);
	D3DXMatrixTranslation(&m_orgMatrix, 1.0f, 1.0f, 0.0f);
}
CD3DSprite::~CD3DSprite()
{

}
void CD3DSprite::Begin(DWORD Flags)
{
	m_pSprite->Begin(Flags);
}
void CD3DSprite::End()
{
	m_pSprite->End();
}
void CD3DSprite::Draw(CTexture2D* pTexture, const RECT& DesRect, D3DCOLOR Color)
{
	Draw(pTexture, DesRect, pTexture->GetRect(), Color);
}
void CD3DSprite::Draw(CTexture2D* pTexture, const RECT& DesRect, const RECT& SurRect, D3DCOLOR Color)
{
	POINT Pos;
	Pos.x = DesRect.left;
	Pos.y = DesRect.top;
	POINT Size;
	Size.x = DesRect.right - DesRect.left;
	Size.y = DesRect.bottom - DesRect.top;
	Draw(pTexture, Pos, Size, SurRect, Color);
}
void CD3DSprite::Draw(CTexture2D* pTexture, const POINT& Pos, D3DCOLOR Color)
{
	Draw(pTexture, Pos, pTexture->GetRect(), Color);
}
void CD3DSprite::Draw(CTexture2D* pTexture,
	const POINT& Pos,					 //绘制起始位置
	const RECT& SurRect,                 // 指定绘制纹理区域
	D3DCOLOR Color
	)
{
	m_pSprite->Draw(pTexture->GetTexture(), &SurRect, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(Pos.x, Pos.y, 0.0f), Color);
}
void CD3DSprite::Draw(CTexture2D* pTexture, const POINT& Pos, const POINT& Size, const RECT& SurRect, D3DCOLOR Color)
{
	D3DXMATRIX TransMatrix;
	FLOAT ScalX, ScalY;
	ScalX = (FLOAT)Size.x / (FLOAT)(SurRect.right - SurRect.left);
	ScalY = (FLOAT)Size.y / (FLOAT)(SurRect.bottom - SurRect.top);
	D3DXMatrixScaling(&TransMatrix, ScalX, ScalY, 0.0f);
	Draw(pTexture, Pos, TransMatrix, SurRect, Color);
}
void CD3DSprite::Draw(CTexture2D* pTexture, const POINT& Pos, const D3DXMATRIX& TransMatrix, const RECT& SurRect, D3DCOLOR Color)
{
	// 设置缩放矩阵
	m_pSprite->SetTransform(&TransMatrix);
	Draw(pTexture, Pos, SurRect, Color);
	// 还原缩放矩阵
	m_pSprite->SetTransform(&m_orgMatrix);
}

