#include "Texture2D.h"
#include "D3DGame.h"

CTexture2D::CTexture2D(LPDIRECT3DDEVICE9 pDevice) :m_pTexture(NULL), g_pD3DDevice(pDevice)
{
}
CTexture2D::~CTexture2D()
{
	g_pD3DDevice = NULL;
	SAFE_RELEASE(m_pTexture);
}
BOOL CTexture2D::LoadTexture(wchar_t* szFxFileName, UINT SizeX /* = D3DX_DEFAULT_NONPOW2 */, UINT SizeY /* = D3DX_DEFAULT_NONPOW2 */)
{
	if (FAILED(D3DXCreateTextureFromFileEx(
		g_pD3DDevice,
		szFxFileName,
		SizeX,
		SizeY,
		D3DX_FROM_FILE,
		D3DPOOL_DEFAULT,
		D3DFMT_FROM_FILE,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f),
		&m_pTextureInfo,
		NULL,
		&m_pTexture
		)))
		return false;
	if (SizeX == (UINT)-2)
		m_Width = m_pTextureInfo.Width;
	else
		m_Width = SizeX;
	if (SizeY == (UINT)-2)
		m_Height = m_pTextureInfo.Height;
	else
		m_Height = SizeY;
	m_SurfRect.top = 0;
	m_SurfRect.left = 0;
	m_SurfRect.right = m_Width;
	m_SurfRect.bottom = m_Height;
	return true;
}
