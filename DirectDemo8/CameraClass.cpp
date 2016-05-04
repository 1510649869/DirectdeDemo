#include "CameraClass.h"
#ifndef SCREEN_WIDTH  
#define SCREEN_WIDTH    800                     //Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��  
#define SCREEN_HEIGHT   600                    //Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�  
#endif  
//Name:CameraClass::CameraClass()
//Desc:���캯��
CameraClass::CameraClass(IDirect3DDevice9 *pd3dDevice)
{
	m_pd3dDevice      = pd3dDevice;
	m_vRightVector    = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vUpVector       = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vLookVector     = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, -250.0f);   // Ĭ�����������Ϊ(0.0f, 0.0f, -250.0f)  
	m_vTargetPostion  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);      //Ĭ�Ϲ۲�Ŀ��λ��Ϊ(0.0f, 0.0f, 0.0f); 
}
//Name:CameraClass::~CameraClass();
//Des:���ù۲����
CameraClass::~CameraClass()
{
	//~;
}
//Name:CameraClass::CaculateViewMatrix()
//Desc:���ݸ����ľ������ȡ���任����
VOID CameraClass::CalculateViewMatrix(D3DXMATRIX* pMatrix)
{
	//1������һ����������
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);            //�淶���۲�����
	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector); //ʹ����������۲�������ֱ
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);                // �淶��������
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector); //����������������ֱ
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
	//2��������ȡ���任����
	pMatrix->_11 = m_vRightVector.x;
	pMatrix->_21 = m_vRightVector.y;
	pMatrix->_31 = m_vRightVector.z;
	pMatrix->_41 = -D3DXVec3Dot(&m_vRightVector, &m_vCameraPosition);    // -P*R  

	pMatrix->_12 = m_vUpVector.x;
	pMatrix->_22 = m_vUpVector.y;
	pMatrix->_32 = m_vUpVector.z;
	pMatrix->_42 = -D3DXVec3Dot(&m_vUpVector, &m_vCameraPosition);

	pMatrix->_13 = m_vLookVector.x;
	pMatrix->_23 = m_vLookVector.y;
	pMatrix->_33 = m_vLookVector.z;
	pMatrix->_43 = -D3DXVec3Dot(&m_vLookVector, &m_vCameraPosition);

	pMatrix->_14 = 0.0f;
	pMatrix->_24 = 0.0f;
	pMatrix->_34 = 0.0f;
	pMatrix->_44 = 1.0f;
}
//Name:
//Des:���ù۲�λ��
VOID CameraClass::SetTargetPostion(D3DXVECTOR3 *pLookAt /* = NULL */)
{
	if (pLookAt)m_vTargetPostion = (*pLookAt);
	else m_vTargetPostion = D3DXVECTOR3(0.0f,0.0f,1.0f);
	m_vLookVector = m_vTargetPostion - m_vCameraPosition;
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);

	//�����淶����������������
	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
}
//Name:
//Des:�������λ��
VOID CameraClass::SetCameraPostion(D3DXVECTOR3 *pCameraPos /* = NULL */)
{
	D3DXVECTOR3 V = D3DXVECTOR3(0.0f, 0.0f, -250.0f);
	m_vCameraPosition = pCameraPos ? (*pCameraPos) : V;
}
//Name:
//Des:����ȡ������
VOID CameraClass::SetViewMatrix(D3DXMATRIX* pMatrix /* = NULL */)
{
	if (pMatrix)
	  m_matView = (*pMatrix);
	else
	  CalculateViewMatrix(&m_matView);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	m_vRightVector = D3DXVECTOR3(m_matView._11, m_matView._12, m_matView._13);
	m_vUpVector    = D3DXVECTOR3(m_matView._21, m_matView._22, m_matView._23);
	m_vLookVector  = D3DXVECTOR3(m_matView._31, m_matView._32, m_matView._33);
}
//Name:
//Des:����ͶӰ����
VOID CameraClass::SetProjMatrix(D3DXMATRIX* pMatrix /* = NULL */)
{
	if (pMatrix)m_matProj = (*pMatrix);
	else
		D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4.0f, (float)((double)SCREEN_WIDTH / SCREEN_HEIGHT), 1.0f, 30000.0f);
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
}
//Name:
//Des:�ع۲췽���ƶ�
VOID CameraClass::MoveAlongLookVec(FLOAT fUnits)
{
	m_vCameraPosition += fUnits*m_vLookVector;
	m_vTargetPostion  += fUnits*m_vLookVector;
}
//Name
//Des:��ˮƽ�����ƶ�
VOID CameraClass::MoveAlongRightVec(FLOAT fUnits)
{
	m_vCameraPosition += fUnits*m_vRightVector;
	m_vTargetPostion += fUnits*m_vRightVector;
}
//Name:
//Des:�ش�ֱ�����ƶ�
VOID CameraClass::MoveAlongUpVec(FLOAT fUnits)
{
	m_vCameraPosition += fUnits*m_vUpVector;
	m_vTargetPostion  += fUnits*m_vUpVector;
}
//Name:
//Des:�ع۲�����������ת
VOID CameraClass::RotationLookVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vLookVector, fAngle);                    //��������m_vLookVector��תfAngle���Ƕȵ�R����  
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);          //��m_vRightVector������m_vLookVector��תfAngle���Ƕ�  
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);                //��m_vUpVector������m_vLookVector��תfAngle���Ƕ�  
	m_vTargetPostion = m_vLookVector * D3DXVec3Length(&m_vCameraPosition); //����һ�¹۲�����λ�ã��������ģ=������
}
//Name:
//Des:��ˮƽ����������ת
VOID CameraClass::RotationRightVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vRightVector, fAngle);
	D3DXVec3TransformCoord(&m_vUpVector,   &m_vUpVector, &R);
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
	m_vTargetPostion = m_vLookVector*D3DXVec3Length(&m_vCameraPosition);
}
//Name:
//Des:��������������ת
VOID CameraClass::RotationUpVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vUpVector, fAngle);
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);
	m_vTargetPostion = m_vLookVector*D3DXVec3Length(&m_vCameraPosition);
}