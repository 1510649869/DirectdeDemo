#include "CD3DEffect.h"

CD3DEffect::CD3DEffect(LPDIRECT3DDEVICE9 pDevice)
{
	m_pd3dDevice = pDevice;
	m_pEffect = NULL;
}
BOOL CD3DEffect::LoadEffect(wchar_t* szFxFile, char* ErrMsg)
{
	HRESULT hr = 0;
	ID3DXBuffer* errorBuffer = 0;
	hr = D3DXCreateEffectFromFile(
		m_pd3dDevice,     // D3D设备
		szFxFile,         // 特效文件
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_DEBUG, // 编译标志
		0,                // don't share parameters
		&m_pEffect,       // 特效对象
		&errorBuffer      // 错误缓冲
		);
	if (errorBuffer)
	{
		ErrMsg = (char*)errorBuffer->GetBufferPointer();
		return FALSE;
	}
	if (FAILED(hr))
	{
		ErrMsg = "D3DXCreateEffectFromFile() - FAILED";
		return FALSE;
	}
	return TRUE;
}
void CD3DEffect::BegainEffect(UINT& numPassess)
{
	m_pEffect->Begin(&numPassess, 0);
}
void CD3DEffect::EndEffect()
{
	m_pEffect->End();
}
void CD3DEffect::Release()
{
	SAFE_RELEASE(m_pEffect);
}