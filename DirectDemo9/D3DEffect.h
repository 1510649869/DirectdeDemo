#pragma once
#include"D3DInit.h"
class CD3DEffect
{
public:
	CD3DEffect(){}
	CD3DEffect(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CD3DEffect(void);
public:
	virtual bool LoadEffect(wchar_t* szFxFileName, char* ErrMsg); //加载特效
	virtual void BeginEffect(UINT& numPasses);                 //开启特效
	virtual void EndEffect();                                  //终止特效
public:
	ID3DXEffect* GetEffect(){ return m_pEffect; }                //获得特效指针
protected:
	ID3DXEffect* m_pEffect;                                    //特效指针
protected:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
};

