#include "D3DCamera.h"
  
//Name:CameraClass::CameraClass()
//Desc:构造函数
CameraClass::CameraClass(IDirect3DDevice9 *pd3dDevice)
{
	m_pd3dDevice      = pd3dDevice;
	m_vRightVector    = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vUpVector       = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vLookVector     = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, -250.0f);   // 默认摄像机坐标为(0.0f, 0.0f, -250.0f)  
	m_vTargetPostion  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);      //默认观察目标位置为(0.0f, 0.0f, 0.0f); 
}
//Name:CameraClass::~CameraClass();
//Des:设置观察矩阵
CameraClass::~CameraClass()
{
	m_pd3dDevice = NULL;
}
//Name:CameraClass::CaculateViewMatrix()
//Desc:根据给出的矩阵求出取景变换矩阵
VOID CameraClass::CalculateViewMatrix(D3DXMATRIX* pMatrix)
{
	//1、创建一组正交矩阵
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);            //规范化观察向量
	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector); //使得上向量与观察向量垂直
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);                // 规范化上向量
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector); //右向量与上向量垂直
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
	//2、创建出取景变换矩阵
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
	m_matFlect = *pMatrix;
}
//Name:
//Des:设置观察位置
VOID CameraClass::SetTargetPostion(D3DXVECTOR3 *pLookAt /* = NULL */)
{
	if (pLookAt)m_vTargetPostion = (*pLookAt);
	else m_vTargetPostion = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vLookVector = m_vTargetPostion - m_vCameraPosition;
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);

	//正交规范化由向量和上向量
	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
}
//Name:
//Des:设置相机位置
VOID CameraClass::SetCameraPostion(D3DXVECTOR3 *pCameraPos /* = NULL */)
{
	D3DXVECTOR3 V = D3DXVECTOR3(0.0f, 0.0f, -250.0f);
	m_vCameraPosition = pCameraPos ? (*pCameraPos) : V;
}
//Name:
//Des:设置取景矩阵
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
//Des:设置投影矩阵
VOID CameraClass::SetProjMatrix(D3DXMATRIX* pMatrix /* = NULL */)
{
	if (pMatrix)m_matProj = (*pMatrix);
	else 
		D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4.0f, (float)((double)WINDOW_WIDTH / WINDOW_HEIGHT), 1.0f, 30000.0f);
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
}
//Name:
//Des:沿观察方向移动
VOID CameraClass::MoveAlongLookVec(FLOAT fUnits)
{
	m_vCameraPosition += fUnits*m_vLookVector;
	m_vTargetPostion += fUnits*m_vLookVector;
}
//Name
//Des:沿水平分量移动
VOID CameraClass::MoveAlongRightVec(FLOAT fUnits)
{
	m_vCameraPosition += fUnits*m_vRightVector;
	m_vTargetPostion += fUnits*m_vRightVector;
}
//Name:
//Des:沿垂直方向移动
VOID CameraClass::MoveAlongUpVec(FLOAT fUnits)
{
	m_vCameraPosition += fUnits*m_vUpVector;
	m_vTargetPostion += fUnits*m_vUpVector;
}
//Name:
//Des:沿观察向量方向旋转
VOID CameraClass::RotationLookVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vLookVector, fAngle);                    //创建出绕m_vLookVector旋转fAngle个角度的R矩阵  
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);          //让m_vRightVector向量绕m_vLookVector旋转fAngle个角度  
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);                //让m_vUpVector向量绕m_vLookVector旋转fAngle个角度  
	m_vTargetPostion = m_vLookVector * D3DXVec3Length(&m_vCameraPosition); //更新一下观察点的新位置（方向乘以模=向量）
}
//Name:
//Des:沿水平向量方向旋转
VOID CameraClass::RotationRightVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	//D3DXVECTOR3 right(1, 0, 0);
	D3DXMatrixRotationAxis(&R, &m_vRightVector, fAngle);
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
	m_vTargetPostion = m_vLookVector*D3DXVec3Length(&m_vCameraPosition);
}
//Name:
//Des:沿上向量方向旋转
VOID CameraClass::RotationUpVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixRotationAxis(&R, &up, fAngle);
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);
	m_vTargetPostion = m_vLookVector*D3DXVec3Length(&m_vCameraPosition);

}