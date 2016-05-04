#include "D3DTerrain.h"

CD3DTerrain::CD3DTerrain(IDirect3DDevice9* pDevice)
{
	//给各个成员变量赋初值  
	m_pd3dDevice = pDevice;
	m_pTexture = NULL;
	m_pIndexBuffer = NULL;
	m_pVertexBuffer = NULL;
	m_nCellsPerRow = 0;
	m_nCellsPerCol = 0;
	m_nVertsPerRow = 0;
	m_nVertsPerCol = 0;
	m_nNumVertices = 0;
	m_fTerrainWidth = 0.0f;
	m_fTerrainDepth = 0.0f;
	m_fCellSpacing = 0.0f;
	m_fHeightScale = 0.0f;
}
BOOL CD3DTerrain::LoadTerrainFromFile(wchar_t *pRawFileName, wchar_t* pTextureFile)
{
	std::ifstream infile;
	infile.open(pRawFileName, std::ios::binary);
	infile.seekg(0, std::ios::end);
	std::vector<BYTE>inData(infile.tellg());
	infile.seekg(std::ios::beg);
	infile.read((char*)&inData[0], inData.size());
	infile.close();
	m_vHeightInfo.resize(inData.size());
	for (unsigned int i = 0; i < inData.size(); i++)
	{
		m_vHeightInfo[i] = inData[i];
	}
	if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice, pTextureFile, &m_pTexture)))
		return FALSE;
	return TRUE;
}
BOOL CD3DTerrain::InitTerrain(INT nRows, INT nCols, FLOAT fSpace, FLOAT fScale)
{
	m_nCellsPerRow = nRows;               //每行的单元格数目  
	m_nCellsPerCol = nCols;               //每列的单元格数目  
	m_fCellSpacing = fSpace;              //单元格间的间距  
	m_fHeightScale = fScale;              //高度缩放系数  
	m_fTerrainWidth = nRows * fSpace;     //地形的宽度  
	m_fTerrainDepth = nCols * fSpace;     //地形的深度  
	m_nVertsPerRow = m_nCellsPerCol + 1;  //每行的顶点数  
	m_nVertsPerCol = m_nCellsPerRow + 1;  //每列的顶点数  
	m_nNumVertices = m_nVertsPerRow * m_nVertsPerCol;  //顶点总数
	// 通过一个for循环，逐个把地形原始高度乘以缩放系数，得到缩放后的高度  
	for (unsigned int i = 0; i < m_vHeightInfo.size(); i++)
		m_vHeightInfo[i] *= m_fHeightScale;
	//1，创建顶点缓存  
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(m_nNumVertices *sizeof(TERRAINVERTEX),
		D3DUSAGE_WRITEONLY, TERRAINVERTEX::FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, 0)))
		return FALSE;
	//2，加锁  
	TERRAINVERTEX *pVertices = NULL;
	m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	//3，访问，赋值  
	FLOAT fStartX = -m_fTerrainWidth / 2.0f, fEndX = m_fTerrainWidth / 2.0f;  //指定起始点和结束点的X坐标值  
	FLOAT fStartZ = m_fTerrainDepth / 2.0f, fEndZ = -m_fTerrainDepth / 2.0f;  //指定起始点和结束点的Z坐标值  
	FLOAT fCoordU = 3.0f / (FLOAT)m_nCellsPerRow;                             //指定纹理的横坐标值  
	FLOAT fCoordV = 3.0f / (FLOAT)m_nCellsPerCol;                             //指定纹理的纵坐标值  

	int nIndex = 0, i = 0, j = 0;
	for (float z = fStartZ; z > fEndZ; z -= m_fCellSpacing, i++)          //Z坐标方向上起始顶点到结束顶点行间的遍历  
	{
		j = 0;
		for (float x = fStartX; x < fEndX; x += m_fCellSpacing, j++)       //X坐标方向上起始顶点到结束顶点行间的遍历  
		{
			nIndex = i * m_nCellsPerRow + j;         //指定当前顶点在顶点缓存中的位置  
			pVertices[nIndex] = TERRAINVERTEX(x, m_vHeightInfo[nIndex], z, j*fCoordU, i*fCoordV); //把顶点位置索引在高度图中对应的各个顶点参数以及纹理坐标赋值给赋给当前的顶点  
			nIndex++;                                                                          //索引数自加1  
		}
	}
	//4，解锁  
	m_pVertexBuffer->Unlock();

	//1.创建索引缓存  
	if (FAILED(m_pd3dDevice->CreateIndexBuffer(m_nNumVertices * 6 * sizeof(WORD),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, 0)))
		return FALSE;
	//2.加锁  
	WORD* pIndices = NULL;
	m_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);
	//3.访问，赋值  
	nIndex = 0;
	for (int row = 0; row < m_nCellsPerRow - 1; row++)   //遍历每行  
	{
		for (int col = 0; col < m_nCellsPerCol - 1; col++)  //遍历每列  
		{
			//三角形ABC的三个顶点  
			pIndices[nIndex] = row* m_nCellsPerRow + col;                 //顶点A  
			pIndices[nIndex + 1] = row * m_nCellsPerRow + col + 1;        //顶点B  
			pIndices[nIndex + 2] = (row + 1) *m_nCellsPerRow + col;       //顶点C  
			//三角形CBD的三个顶点  
			pIndices[nIndex + 3] = (row + 1) *m_nCellsPerRow + col;       //顶点C  
			pIndices[nIndex + 4] = row * m_nCellsPerRow + col + 1;        //顶点B  
			pIndices[nIndex + 5] = (row + 1) *m_nCellsPerRow + col + 1;   //顶点D  
			//处理完一个单元格，索引加上6  
			nIndex += 6;  //索引自加6  
		}
	}
	//4、解锁  
	m_pIndexBuffer->Unlock();

	return TRUE;
}
BOOL CD3DTerrain::InitTerrainMyself()
{
	m_pd3dDevice->CreateVertexBuffer(sizeof(TERRAINVERTEX)* 4, 0, TERRAINVERTEX::FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
	TERRAINVERTEX *pVertices = NULL;
	m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = TERRAINVERTEX(-5000.0f, 0.0f, -5000.0f, 0.0f, 30.0f);
	pVertices[1] = TERRAINVERTEX(-5000.0f, 0.0f, 5000.0f, 0.0f, 0.0f);
	pVertices[2] = TERRAINVERTEX(5000.0f, 0.0f, -5000.0f, 30.0f, 30.0f);
	pVertices[3] = TERRAINVERTEX(5000.0f, 0.0f, 5000.0f, 30.0f, 0.0f);
	m_pVertexBuffer->Unlock();
	return TRUE;
}
BOOL CD3DTerrain::Draw(BOOL bDrawFrame /* = false */)
{
	/*D3DXMATRIX posMatrix;
	D3DXMatrixTranslation(&posMatrix, pos.x, pos.y, pos.z);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &posMatrix);*/

	m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(TERRAINVERTEX));
	m_pd3dDevice->SetFVF(TERRAINVERTEX::FVF);
	m_pd3dDevice->SetIndices(m_pIndexBuffer);
	m_pd3dDevice->SetTexture(0, m_pTexture);
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);     //关闭光照  

	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_nNumVertices, 0, m_nNumVertices * 2);              //绘制顶点 
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pd3dDevice->SetTexture(0, 0);

	if (bDrawFrame)  //如果要渲染出线框的话  
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); //把填充模式设为线框填充  
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			m_nNumVertices, 0, m_nNumVertices * 2);       //绘制顶点   
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);    //把填充模式调回实体填充  
	}
	//还原世界矩阵；
	/*D3DXMatrixTranslation(&posMatrix, 0, 0, 0);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &posMatrix);*/
	return TRUE;
}
//析构函数 释放资源
CD3DTerrain::~CD3DTerrain()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	m_pd3dDevice = NULL;
}