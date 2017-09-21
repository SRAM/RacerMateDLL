#define STRICT
#define WIN32_LEAN_AND_MEAN
//#define D3D_OVERLOADS

#ifdef WIN32
	#include <windowsx.h>
#endif


#include <comutils.h>
#include <utils.h>

#include <windower.h>


/**********************************************************************

	left, right refer to the record number in the file

**********************************************************************/

#ifdef WIN32
Windower::Windower(HDC _hdc, RECT _waverect, int _records )  {
	hdc = _hdc;
	cursorpen = CreatePen(PS_SOLID, 0, RGB(255, 0, 0) );
#else
Windower::Windower(RECT _waverect, int _records )  {
#endif

	records = _records;
	CopyRect(&r, &_waverect);

	setsize(_records);

}

/**********************************************************************

**********************************************************************/

Windower::~Windower()  {
	destroy();
	return;
}

/*******************************************************************************

*******************************************************************************/

void Windower::enter(void)  {
	map(0.0f, (float)(w-1), (float)left, (float)right, &m, &b);
	map((float)left, (float)right, (float)(r.left + 1), (float)(r.right-1), &mc, &bc);
	minsamp = left;
	maxsamp = right;

	if ((right - left) <= w)  {
		zoomed_in = true;
	}
	else  {
		zoomed_in = false;
	}
	return;
}

/**********************************************************************

**********************************************************************/

void Windower::moveright(void)  {

	delta = (float)(right-left) / 15.0f;
	left += (long)delta;
	right += (long)delta;

	if (right>maxsamp)  {
		right = (int)maxsamp;
	}

	long diff = right - left;

	if ( diff < (w-1))  {
		left = right - w + 1;
	}

	return;
}

/**********************************************************************

**********************************************************************/

void Windower::moveleft(void)  {

	delta = (float)(right-left) / 15.0f;
	left -= (long)delta;
	right -= (long)delta;

	if (left<minsamp)  {
		left = (int)minsamp;
	}

	long diff = right - left;

	if ( diff < (w-1))  {
		right = left + w - 1;
	}

	return;
}


/**********************************************************************

**********************************************************************/

void Windower::movein(void)  {

	//delta = (float)(right-left) / 15.0f;
	delta = 1;
	left += (long) (.5f + delta);
	right -= (long) (.5f + delta);


	/*
	long diff = right - left;
	if ( diff < (w-1))  {
		long center = (long) (.5f + (float)(left+right)/2.0f);
		left = (int) (.5f + (float)center - (float)w / 2.0f);
		right = left + w - 1;
	}
	*/

	draw();

	return;
}

/**********************************************************************

**********************************************************************/

void Windower::moveout(void)  {
	//delta = (float)(right-left) / 15.0f;
	delta = 1;
	left -= (long) (.5f + delta);
	right += (long) (.5f + delta);

	if (left<minsamp)  {
		left = (int)minsamp;
		if (right > maxsamp)  {
			right = (int)maxsamp;
			return;
		}
		return;
	}


	if (right>maxsamp)  {
		right = (int)maxsamp;
		if (left<minsamp)  {
			left = (int)minsamp;
			return;
		}
		return;
	}

	long diff = right - left;

	if ( diff < (w-1))  {
		long center = (long) (.5f + (float)(left+right)/2.0f);
		left = (int) (.5f + (float)center - (float)w / 2.0f);
		right = left + w - 1;
	}

	draw();

	return;
}


/**********************************************************************

**********************************************************************/

long Windower::getLeft(void)  {
	return left;
}

/**********************************************************************

**********************************************************************/

long Windower::getRight(void)  {
	return right;
}

/*******************************************************************************
	home key hit, zoom all the way out and beep
*******************************************************************************/

void Windower::home(void)  {
	left = 0L;
	right = records - 1;

	return;
}




/*******************************************************************************

*******************************************************************************/

void Windower::draw(void)  {
	int x1, x2;

	//pd3dDevice->SetMaterial(&zwcursormaterial);

	x1 = (int) (.5 + mc*left + bc);
	x2 = (int) (.5 + mc*right + bc);

	vline(x1, 0);
	vline(x2, 1);

	return;
}

/**********************************************************************

**********************************************************************/

void Windower::destroy(void)  {

#ifdef WIN32
	if (cursorpen)  {
		DeleteObject(cursorpen);
		cursorpen = 0;
	}
#endif

	return;
}

/**********************************************************************

**********************************************************************/

void Windower::setsize(int _records)  {
	records = _records;

	zoomed_in = false;
	left = 0L;
	right = records - 1;
	lastx[0] = -1;
	lastx[1] = -1;

	w = r.right - r.left - 1;					// W is 2 less than the full width of waverect

	enter();
	return;
}

/**********************************************************************

**********************************************************************/

void Windower::vline(int x, int k)  {
	bool flag = false;
#ifdef WIN32
	int oldrop;
#endif

	if (x != lastx[k])  {
#ifdef WIN32
		SelectPen(hdc, cursorpen);
		oldrop = SetROP2(hdc, R2_XORPEN);
#endif

		flag = true;

#ifdef WIN32
		if (lastx[k] != -1)  {
			MoveToEx(hdc, lastx[k], r.top, NULL);
			LineTo(hdc, lastx[k], r.bottom);

			if (x < r.left)  {
				x = r.left;
			}
			else if (x>r.right)  {
				x = r.right;
			}
			MoveToEx(hdc, x, r.top, NULL);
			LineTo(hdc, x, r.bottom);
			lastx[k] = x;
		}
		else  {
			if (x < r.left)  {
				lastx[k] = r.left;
			}
			else if (x > r.right)  {
				lastx[k] = r.right;
			}
			else  {
				lastx[k] = x;
			}
			MoveToEx(hdc, lastx[k], r.top, NULL);
			LineTo(hdc, lastx[k], r.bottom);
		}
#endif
	}

	if (flag)  {
#ifdef WIN32
		SetROP2(hdc, oldrop);
#endif
	}

	return;
}

