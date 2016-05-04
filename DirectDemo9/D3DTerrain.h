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
	int                         m_nCellsPerRow;     // 每行的单元格数  
	int                         m_nCellsPerCol;     // 每列的单元格数  
	int                         m_nVertsPerRow;     // 每行的顶点数  
	int                         m_nVertsPerCol;     // 每列的顶点数  
	int                         m_nNumVertices;     // 顶点总数  
	FLOAT                       m_fTerrainWidth;    // 地形的宽度  
	FLOAT                       m_fTerrainDepth;    // 地形的深度  
	FLOAT                       m_fCellSpacing;     // 单元格的间距  
	FLOAT                       m_fHeightScale;      // 高度缩放系数  
	std::vector<FLOAT>          m_vHeightInfo;      // 用于存放高度信息
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

