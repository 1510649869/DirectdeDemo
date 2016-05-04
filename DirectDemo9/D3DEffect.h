#pragma once
#include"D3DInit.h"
class CD3DEffect
{
public:
	CD3DEffect(){}
	CD3DEffect(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CD3DEffect(void);
public:
	virtual bool LoadEffect(wchar_t* szFxFileName, char* ErrMsg); //������Ч
	virtual void BeginEffect(UINT& numPasses);                 //������Ч
	virtual void EndEffect();                                  //��ֹ��Ч
public:
	ID3DXEffect* GetEffect(){ return m_pEffect; }                //�����Чָ��
protected:
	ID3DXEffect* m_pEffect;                                    //��Чָ��
protected:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
};

