
/**********************************************************
Ref.h

Code Library:		NONE 
Depenent Libraries:	NONE

***********************************************************/


// Ref.h: interface for the Ref class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REF_H__40B0F7E1_7759_11D2_8BF0_0020AFE612D6__INCLUDED_)
#define AFX_REF_H__40B0F7E1_7759_11D2_8BF0_0020AFE612D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**********************************************************
CLASS
	Ref 
DESCRIPTION
	Include to add Reference functionality to a class. It is
	suggested that the destructor should be protected in the
	derived classes.

	When an instance of this class is created the reference count is set to 1. 
	Everytime the AddRef() is called the value is incremented by one.  When Release() is 
	called the the ref is decreased by one until the count reaches 0.  When m_cRef reaches 
	zero durring the Release() call the object is deleted and a 0 is returned indecating that
	that object was really deleted.

	<h3>When to use:</h3>

	This object should be used whenever a there is a need for multiple copies of the reference to 
	the object.   This class provides a safe way to keep the object around even when the other parts of 
	the program don't need it any more.   Here is a quick example of a usage:
	  <pre>
	  class A
	  {
	  protected:
			 Ref  	*m_pRefObj;
	  public:
			 A( Ref *pref )
			 {
					m_pRefObj = pref;
					if (m_pRefObj)
						  pref->AddRef();
			 }
			 ~A()
			 {
					if (m_pRefObj)
						  m_pRefObj->Release();
			 }
	  };

	  void func()
	  {
			 Ref* pr = new Ref();
			 A aclass( pr );
			 pr->Release();
	  }
	  </pre>

	In the above example the actual instance of pr is not released until "aclass" goes out of scope, at which time
	the pr is deleted automaticly.

AUTHOR
	Will Ware
***********************************************************/
class Ref{
	protected:

		//-----------------------------------------------------
		//  [use] ~Ref
		//		The destructor is virtual and protected so that the user cannot destroy the object.   
		//
		//		This of course can be defeated if the derived class makes its destructor public.
		//
		virtual ~Ref()  {
		}

		//-----------------------------------------------------
		//  [] m_cRef
		//		contains the reference count for the object.  This starts out with 1, each AddRef() adds
		//		one to the ref.  Each Release() removes one until the count is zero in which the object
		//		is deleted.
		int m_cRef;
	public:

		//-----------------------------------------------------
		//  [use] Ref
		//		Creates the object and sets the m_cRef member to 1.
		Ref()  {
			m_cRef = 1;
		}

		//-----------------------------------------------------
		//  [use] AddRef
		//		Adds one to the reference count.
		//
		virtual void AddRef()  {
			m_cRef++;
		}

		//-----------------------------------------------------
		//  [use] Release
		//		Decrements the reference count by one, if the new reference count list <= 0 then the object deletes itself.
		//
		//		Care must be taken with these objects due to this property.  After a Release is called the pointer should be 
		//		concidered invalid.
		//
		//  [out] int
		//		Returns the number of the m_cRef or 0 if the object was deleted. <strong>Use with care!</strong>
		//
		virtual int Release()  {
			if (--m_cRef <= 0)  {
				if (m_cRef == 0)	// Delete ONLY on zero..
					delete this;
				return 0;
			}
			return m_cRef;
		}
};


#endif // !defined(AFX_REF_H__40B0F7E1_7759_11D2_8BF0_0020AFE612D6__INCLUDED_)
