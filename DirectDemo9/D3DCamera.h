//Direct3D 摄像机类的封装
#pragma once
#include "D3DInit.h"
#include "D3DUtil.h"

class CameraClass
{
public:
	//构造方法
	CameraClass(IDirect3DDevice9 *pd3dDevice);
	//析构方法
	~CameraClass();
public:
	VOID CalculateViewMatrix(D3DXMATRIX* pMatrix);//根据给出的矩阵求出取景变换矩阵
	VOID GetProjMatrix(D3DXMATRIX* pMatrix){ *pMatrix = m_matProj; }             //返回当前的投影矩阵
	VOID GetCameraPostion(D3DXVECTOR3* pVector){ *pVector = m_vCameraPosition; } //返回相机当前位置
	VOID GetLookVector(D3DXVECTOR3* pVector){ *pVector = m_vLookVector; }	//返回观察向量
	D3DXMATRIX GetReflectMatrix(){ return -m_matFlect; }
public:
	VOID SetTargetPostion(D3DXVECTOR3 *pLookAt = NULL);
	VOID SetCameraPostion(D3DXVECTOR3 *pCameraPos = NULL);
	VOID SetViewMatrix(D3DXMATRIX* pMatrix = NULL);
	VOID SetProjMatrix(D3DXMATRIX* pMatrix = NULL);
public:
	VOID MoveAlongRightVec(FLOAT fUnits);
	VOID MoveAlongUpVec(FLOAT fUnits);
	VOID MoveAlongLookVec(FLOAT fUnits);
	VOID RotationRightVec(FLOAT fAngles);
	VOID RotationUpVec(FLOAT fAngles);
	VOID RotationLookVec(FLOAT fAngles);

private:
	D3DXVECTOR3  m_vRightVector;  //右分量向量
	D3DXVECTOR3  m_vUpVector;     //上分量向量
	D3DXVECTOR3  m_vLookVector;   //观察方向向量
private:
	D3DXVECTOR3  m_vCameraPosition;
	D3DXVECTOR3  m_vTargetPostion;
private:
	D3DXMATRIX   m_matView;  //取景变换矩阵
	D3DXMATRIX   m_matProj;  //投影变换矩阵
	D3DXMATRIX   m_matFlect;
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
};
