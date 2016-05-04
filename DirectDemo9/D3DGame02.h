#pragma once
#include "D3DBaseGame.h"
#include "D3DCamera.h"
class D3DGame02:public D3DBaseGame
{
public:
	D3DGame02();
	~D3DGame02();

	virtual HRESULT LoadContent() override;

	virtual VOID UnLoadContent() override;

	virtual VOID Update() override;

	virtual VOID D3DRender() override;
private:
	//CameraClass* g_pCamera;
};

