// Model.h: interface for the Model class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODEL_H__B84E5E81_1966_11D5_8F41_E7E31B502761__INCLUDED_)
#define AFX_MODEL_H__B84E5E81_1966_11D5_8F41_E7E31B502761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "D3DBase.h"
#include "DX8_Supp.h"
#include "d3dfile.h"


class Model : public D3DBase  {
		char *m_pName;
		bool m_Dynamic;

		D3DMATERIAL7 *m_pMat;
		DWORD m_MatCount;
		CD3DFile *m_pMesh;
		//Tex	**			m_pTexArr;


	public:
		Model(const char *pname, bool dynamic = false);
		virtual ~Model();

#ifdef _DEBUG
		virtual char *getName(void)  {
			return m_pName;
		}
#endif
		virtual HRESULT Open();			// Called when a d3d device goes online.
		virtual HRESULT Restore();		// Called when a device is restored.
		virtual HRESULT Invalidate();	// Called when a device goes down.
		virtual HRESULT Close();		// Called when a device gets destroyed.
		virtual HRESULT Render();

		virtual HRESULT Render(D3DMATRIX &mat);

		virtual int ReplaceTexture(const char *name, Tex *ptex);		// Replace all textures with that name with this this new texture 
};

#endif // !defined(AFX_MODEL_H__B84E5E81_1966_11D5_8F41_E7E31B502761__INCLUDED_)
