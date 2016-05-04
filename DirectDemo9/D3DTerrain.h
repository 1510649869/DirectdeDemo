#pragma once
#include"D3DInit.h"
#include<vector>
#include <fstream>
class CD3DTerrain
{
public:
	CD3DTerrain(IDirect3DDevice9* pDevice);
	~CD3DTerrain();
public:
	BOOL LoadTerrainFromFile(wchar_t *pRawFileName, wchar_t* pTextureFile);
	BOOL InitTerrain(INT nRows, INT nCols, FLOAT fSpace, FLOAT fScale);
	BOOL InitTerrainMyself();
	BOOL Draw(BOOL bDrawFrame = false);
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
private:
	int                         m_nCellsPerRow;     // ÿ�еĵ�Ԫ����  
	int                         m_nCellsPerCol;     // ÿ�еĵ�Ԫ����  
	int                         m_nVertsPerRow;     // ÿ�еĶ�����  
	int                         m_nVertsPerCol;     // ÿ�еĶ�����  
	int                         m_nNumVertices;     // ��������  
	FLOAT                       m_fTerrainWidth;    // ���εĿ��  
	FLOAT                       m_fTerrainDepth;    // ���ε����  
	FLOAT                       m_fCellSpacing;     // ��Ԫ��ļ��  
	FLOAT                       m_fHeightScale;      // �߶�����ϵ��  
	std::vector<FLOAT>          m_vHeightInfo;      // ���ڴ�Ÿ߶���Ϣ
private:
	struct TERRAINVERTEX
	{
		FLOAT _x, _y, _z;
		FLOAT _u, _v;
		TERRAINVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v) :_x(x)
			, _y(y), _z(z), _u(u), _v(v){
		}
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};
};

