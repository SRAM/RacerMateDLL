// Camera.h: interface for the Camera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__E3107324_1ECE_11D5_8405_0050DA826027__INCLUDED_)
#define AFX_CAMERA_H__E3107324_1ECE_11D5_8405_0050DA826027__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DIRECTINPUT_VERSION 0x0700

#include "dx8_supp.h"

class Camera{
		D3DXMATRIX m_VMat;
		D3DXMATRIX m_IVMat;
		D3DXMATRIX m_PMat;
		float m_Aspect;
		bool m_UpdateP;
		bool m_UpdateIV;
		float m_Near;
		float m_Far;
		float m_Angle;

		float m_HZAdj;
		float m_WZAdj;

		float m_Yaw;
		float m_Pitch;

		D3DXVECTOR3 m_Lookat;
		D3DXVECTOR3 m_Eye;
		D3DXVECTOR3 m_XVec;
		D3DXVECTOR3 m_YVec;
		D3DXVECTOR3 m_ZVec;

		DWORD m_x;
		DWORD m_y;
		DWORD m_width;
		DWORD m_height;
		int m_screenwidth;
		int m_screenheight;
		D3DVIEWPORT7 m_VP;

		D3DXVECTOR3 m_CornerArr[4];
		void updateProjection();
		Camera(const Camera&);						// unimplemented
		Camera &operator = (const Camera&);		// unimplemented

	public:
		Camera();
		virtual ~Camera();

		void SetViewPort(DWORD x, DWORD y, DWORD width, DWORD height, float minz = 0.0f,
							  float maxz = 1.0f);

		void SetNear(float nearf)  {
			m_Near = nearf; m_UpdateP = true;
		}
		void SetFar(float farf)  {
			m_Far = farf; m_UpdateP = true;
		}

		const D3DXMATRIX *LookAt(const D3DXVECTOR3 &eye, const D3DXVECTOR3 &lookat,
										 const D3DXVECTOR3 *vup = NULL);


		const D3DXMATRIX * GetViewMatrix() const  {
			return &m_VMat;
		}
		const D3DXMATRIX *GetInverseViewMatrix();
		const D3DXMATRIX * GetProjectionMatrix()  {
			if (m_UpdateP)
				updateProjection();
			return &m_PMat;
		}
		const D3DVIEWPORT7 *GetViewport();

		bool SphereInFulcrum(const D3DXVECTOR3 &pt, float radius);
		bool SphereInFulcrum2D(const D3DXVECTOR3 &pt, float radius);
		int Side(const D3DXVECTOR3 &pt) const;		// -1 left 0 center 1 right

		float GetPitch() const  {
			return m_Pitch;
		}
		float GetYaw() const  {
			return m_Yaw;
		}

		void SetAngleOfView(float angle);
		float GetAngleOfView() const  {
			return D3DXToDegree(m_Angle);
		}

		const D3DXVECTOR3 & GetEye() const  {
			return m_Eye;
		}
		const D3DXVECTOR3 & GetLookat() const  {
			return m_Lookat;
		}

		const D3DXVECTOR3 & GetXVec() const  {
			return *(D3DXVECTOR3 *) &m_VMat.m00;
		}
		const D3DXVECTOR3 & GetYVec() const  {
			return *(D3DXVECTOR3 *) &m_VMat.m10;
		}
		const D3DXVECTOR3 & GetZVec() const  {
			return *(D3DXVECTOR3 *) &m_VMat.m20;
		}

		void UseCamera(LPDIRECT3DDEVICE7 pdevice);

		void GetFulcrumBox(D3DXVECTOR3 &minv, D3DXVECTOR3 &maxv);


		float FindDistanceFromX(float x) const;
		float FindDistanceFromY(float y) const;
};

#endif // !defined(AFX_CAMERA_H__E3107324_1ECE_11D5_8405_0050DA826027__INCLUDED_)
