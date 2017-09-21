


#include <utils.h>
#include <boxgrid.h>

/**************************************************************************************

**************************************************************************************/

//BoxGrid::BoxGrid(int _nrows, int _ncols, float _ph, float _phmargin, float _phpadding, float _pw, float _pvmargin, float _pvpadding)  {
BoxGrid::BoxGrid(
				int _nrows, 
				int _ncols, 

				// top/bottom:
				float _ph, 
				float _ptopmargin, 
				float _pbottommargin, 
				float _pvpadding,

				// left/right:
				float _pw, 
				float _pleftmargin, 
				float _prightmargin, 
				float _phpadding
				)  {

	initialized = false;

	nrows = _nrows;
	ncols = _ncols;

	// top/bottom:
	ph = _ph, 
	ptopmargin = _ptopmargin;
	pbottommargin = _pbottommargin;
	pvpadding = _pvpadding;

	// left/right:
	pw = _pw;
	pleftmargin = _pleftmargin;
	prightmargin = _prightmargin;
	phpadding = _phpadding;

	rows = NULL;
	cols = NULL;
	bp = 0;
	err = BOXGRID_OK;

	/*
	ph = _ph;
	pvmargin = _pvmargin;
	pvpadding = _pvpadding;
	*/

	float totalpw, totalph;

	totalph = nrows*ph + ptopmargin + pbottommargin + (nrows-1)*pvpadding;

	if (totalph > 1.00f)  {
		bp = 1;
		err = BOXGRID_TOO_HIGH;
		return;
	}

	totalpw = ncols*pw + pleftmargin + prightmargin + (ncols-1)*phpadding;
	if (totalpw > 1.00f)  {
		bp = 1;
		err = BOXGRID_TOO_WIDE;
		return;
	}

	winw = 0.0f;
	winh = 0.0f;
	memset(&win, 0, sizeof(win));

	leftmargin = 0.0f;
	rightmargin = 0.0f;
	hpadding = 0.0f;
	boxh = 0.0f;

	topmargin = 0.0f;
	bottommargin = 0.0f;
	vpadding = 0.0f;
	boxw = 0.0f;

	rows = new ROW[nrows];
	cols = new COL[ncols];
	memset(rows, 0, nrows*sizeof(ROW));
	memset(cols, 0, ncols*sizeof(COL));

	mx = bx = my = by = 0.0f;

	bp = 0;
	initialized = true;

}

/**************************************************************************************

**************************************************************************************/

BoxGrid::~BoxGrid(void) {
	DELARR(rows);
	DELARR(cols);

}

/**************************************************************************************

**************************************************************************************/

void BoxGrid::resize(FRECT *_win) {
	int i, j;

	memcpy(&win, _win, sizeof(win));
	winw = win.right - win.left;
	winh = win.bottom - win.top;

	boxw = pw*winw;
	boxh = ph*winh;

	topmargin = ptopmargin * winh;
	bottommargin = pbottommargin * winh;
	leftmargin = pleftmargin * winw;
	rightmargin = prightmargin * winw;

	vpadding = pvpadding * winh;
	hpadding = phpadding * winw;

	map(0.0f, (float)(nrows-1), win.top+topmargin, win.bottom-(boxh+bottommargin), &my, &by);

	map(0.0f, (float)(ncols-1), win.left+leftmargin, win.right-(boxw+rightmargin), &mx, &bx);

	for(i=0; i<nrows; i++)  {
		rows[i].top = my*i + by;
		rows[i].bot = rows[i].top + boxh;

		for(j=0; j<ncols; j++)  {
			cols[j].left = mx*j + bx;
			cols[j].right = cols[j].left + boxw;
		}
	}


	bp = 1;

	return;
}

/**************************************************************************************
	note: may modify pw
**************************************************************************************/

float BoxGrid::compute_h_padding(int NCOLS, float pleftmargin, float prightmargin, float &pw)  {
	float m, b, phpadding, totalpw;

	map(8.0f, 16.0f, .5f, .1f, &m, &b);
	phpadding = ((1.0f - pleftmargin - prightmargin)/(float)NCOLS - pw) / (1.0f - 1.0f/(float)NCOLS);
	totalpw = NCOLS*pw + pleftmargin + prightmargin + (NCOLS-1)*phpadding;

	if (totalpw > 1.0f)  {
		phpadding = 0.0f;
		pw = (1.0f -pleftmargin - prightmargin)/(float)NCOLS;
#ifdef _DEBUG
		totalpw = NCOLS*pw + pleftmargin + prightmargin + (NCOLS-1)*phpadding;
#endif
	}

	return phpadding;
}

