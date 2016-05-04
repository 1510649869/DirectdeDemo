#include "D3DTerrain.h"

CD3DTerrain::CD3DTerrain(IDirect3DDevice9* pDevice)
{
	//��������Ա��������ֵ  
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
	m_nCellsPerRow = nRows;               //ÿ�еĵ�Ԫ����Ŀ  
	m_nCellsPerCol = nCols;               //ÿ�еĵ�Ԫ����Ŀ  
	m_fCellSpacing = fSpace;              //��Ԫ���ļ��  
	m_fHeightScale = fScale;              //�߶�����ϵ��  
	m_fTerrainWidth = nRows * fSpace;     //���εĿ��  
	m_fTerrainDepth = nCols * fSpace;     //���ε����  
	m_nVertsPerRow = m_nCellsPerCol + 1;  //ÿ�еĶ�����  
	m_nVertsPerCol = m_nCellsPerRow + 1;  //ÿ�еĶ�����  
	m_nNumVertices = m_nVertsPerRow * m_nVertsPerCol;  //��������
	// ͨ��һ��forѭ��������ѵ���ԭʼ�߶ȳ�������ϵ�����õ����ź�ĸ߶�  
	for (unsigned int i = 0; i < m_vHeightInfo.size(); i++)
		m_vHeightInfo[i] *= m_fHeightScale;
	//1���������㻺��  
	if (FAILED(m_pd3dDevice->CreateVertexBuffer(m_nNumVertices *sizeof(TERRAINVERTEX),
		D3DUSAGE_WRITEONLY, TERRAINVERTEX::FVF, D3DPOOL_MANAGED, &m_pVertexBuffer, 0)))
		return FALSE;
	//2������  
	TERRAINVERTEX *pVertices = NULL;
	m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	//3�����ʣ���ֵ  
	FLOAT fStartX = -m_fTerrainWidth / 2.0f, fEndX = m_fTerrainWidth / 2.0f;  //ָ����ʼ��ͽ������X����ֵ  
	FLOAT fStartZ = m_fTerrainDepth / 2.0f, fEndZ = -m_fTerrainDepth / 2.0f;  //ָ����ʼ��ͽ������Z����ֵ  
	FLOAT fCoordU = 3.0f / (FLOAT)m_nCellsPerRow;                             //ָ������ĺ�����ֵ  
	FLOAT fCoordV = 3.0f / (FLOAT)m_nCellsPerCol;                             //ָ�������������ֵ  

	int nIndex = 0, i = 0, j = 0;
	for (float z = fStartZ; z > fEndZ; z -= m_fCellSpacing, i++)          //Z���귽������ʼ���㵽���������м�ı���  
	{
		j = 0;
		for (float x = fStartX; x < fEndX; x += m_fCellSpacing, j++)       //X���귽������ʼ���㵽���������м�ı���  
		{
			nIndex = i * m_nCellsPerRow + j;         //ָ����ǰ�����ڶ��㻺���е�λ��  
			pVertices[nIndex] = TERRAINVERTEX(x, m_vHeightInfo[nIndex], z, j*fCoordU, i*fCoordV); //�Ѷ���λ�������ڸ߶�ͼ�ж�Ӧ�ĸ�����������Լ��������긳ֵ��������ǰ�Ķ���  
			nIndex++;                                                                          //�������Լ�1  
		}
	}
	//4������  
	m_pVertexBuffer->Unlock();

	//1.������������  
	if (FAILED(m_pd3dDevice->CreateIndexBuffer(m_nNumVertices * 6 * sizeof(WORD),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndexBuffer, 0)))
		return FALSE;
	//2.����  
	WORD* pIndices = NULL;
	m_pIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);
	//3.���ʣ���ֵ  
	nIndex = 0;
	for (int row = 0; row < m_nCellsPerRow - 1; row++)   //����ÿ��  
	{
		for (int col = 0; col < m_nCellsPerCol - 1; col++)  //����ÿ��  
		{
			//������ABC����������  
			pIndices[nIndex] = row* m_nCellsPerRow + col;                 //����A  
			pIndices[nIndex + 1] = row * m_nCellsPerRow + col + 1;        //����B  
			pIndices[nIndex + 2] = (row + 1) *m_nCellsPerRow + col;       //����C  
			//������CBD����������  
			pIndices[nIndex + 3] = (row + 1) *m_nCellsPerRow + col;       //����C  
			pIndices[nIndex + 4] = row * m_nCellsPerRow + col + 1;        //����B  
			pIndices[nIndex + 5] = (row + 1) *m_nCellsPerRow + col + 1;   //����D  
			//������һ����Ԫ����������6  
			nIndex += 6;  //�����Լ�6  
		}
	}
	//4������  
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
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);     //�رչ���  

	m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
		m_nNumVertices, 0, m_nNumVertices * 2);              //���ƶ��� 
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pd3dDevice->SetTexture(0, 0);

	if (bDrawFrame)  //���Ҫ��Ⱦ���߿�Ļ�  
	{
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); //�����ģʽ��Ϊ�߿����  
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
			m_nNumVertices, 0, m_nNumVertices * 2);       //���ƶ���   
		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);    //�����ģʽ����ʵ�����  
	}
	//��ԭ�������
	/*D3DXMatrixTranslation(&posMatrix, 0, 0, 0);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &posMatrix);*/
	return TRUE;
}
//�������� �ͷ���Դ
CD3DTerrain::~CD3DTerrain()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexBuffer);
	m_pd3dDevice = NULL;
}