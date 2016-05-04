//Direct3D �������ķ�װ
#pragma once
#include <d3dx9.h>
#include <d3d9.h>
class CameraClass
{
public:
	//���췽��
	CameraClass(IDirect3DDevice9 *pd3dDevice);
	//��������
	~CameraClass();
public:
	VOID CalculateViewMatrix(D3DXMATRIX* pMatrix);//���ݸ����ľ������ȡ���任����
	
	VOID GetProjMatrix      (D3DXMATRIX* pMatrix){ *pMatrix = m_matProj; }             //���ص�ǰ��ͶӰ����
	VOID GetCameraPostion   (D3DXVECTOR3* pVector){ *pVector = m_vCameraPosition; } //���������ǰλ��
	VOID GetLookVector      (D3DXVECTOR3* pVector){ *pVector = m_vLookVector; }		 //���ع۲�����
public:
	VOID SetTargetPostion   (D3DXVECTOR3 *pLookAt = NULL);
	VOID SetCameraPostion   (D3DXVECTOR3 *pCameraPos = NULL);
	VOID SetViewMatrix      (D3DXMATRIX* pMatrix = NULL);
	VOID SetProjMatrix      (D3DXMATRIX* pMatrix = NULL);
public:
	VOID MoveAlongRightVec  (FLOAT fUnits);
	VOID MoveAlongUpVec     (FLOAT fUnits);
	VOID MoveAlongLookVec   (FLOAT fUnits);
	VOID RotationRightVec   (FLOAT fAngles);
	VOID RotationUpVec      (FLOAT fAngles);
	VOID RotationLookVec    (FLOAT fAngles);

private:
	D3DXVECTOR3  m_vRightVector;  //�ҷ�������
	D3DXVECTOR3  m_vUpVector;     //�Ϸ�������
	D3DXVECTOR3  m_vLookVector;   //�۲췽������
private:
	D3DXVECTOR3  m_vCameraPosition;
	D3DXVECTOR3  m_vTargetPostion;
private:
	D3DXMATRIX   m_matView;  //ȡ���任����
	D3DXMATRIX   m_matProj;  //ͶӰ�任����
private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;
};
