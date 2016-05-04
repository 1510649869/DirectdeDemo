#pragma once
#include "Texture2D.h"
class CD3DSprite
{
public:
	CD3DSprite(LPDIRECT3DDEVICE9 pDevice);
	~CD3DSprite();
public:
	void Begin(DWORD flags);
	void End();                              // ����Sprite����
	void Draw(                               // ����Sprite
		CTexture2D* pTexture,                // 2D����
		const POINT& Position,               // λ��
		D3DCOLOR Color                       // ɫ��
		);
	void Draw(
		CTexture2D* pTexture,
		const POINT& Position,
		const RECT& SurRect,                 // ָ��������������
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const POINT& Pos,
		const D3DXMATRIX& TransMatrix,       // �任����
		const RECT& SurRect,
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const POINT& Pos,
		const POINT& Size,                   // ���Ƴߴ�
		const RECT& SurRect,
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const RECT& DesRect,                 // ָ������Ŀ������
		const RECT& SurRect,
		D3DCOLOR Color
		);
	void Draw(
		CTexture2D* pTexture,
		const RECT& DesRect,
		D3DCOLOR Color
		);
public:
	ID3DXSprite* GetSprite(){ return m_pSprite; }   // ���Spriteָ��
private:
	ID3DXSprite* m_pSprite;                  // Spriteָ��
	D3DXMATRIX   m_orgMatrix;                // ԭʼ����
};

