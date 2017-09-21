
#include <stdio.h>

#include <utilsclass.h>


/********************************************************************
					m a p
 returns m,b that maps x1,x2 to y1,y2
 general mapping:
	m = (y2 - y1) / (x2-x1)
	y = mx + (y1 - m*x1)
 exit:
	 returns -1 when mapping is impossible (divide by 0)
********************************************************************/


short Utils::map(float x1,float x2,float y1,float y2,float &m,float &b)  {
	float d;

	d = x2 - x1;

	if (d == 0.0f) {		// prevent divide by 0
		m = 0.0f;
		b = y1;
		return -1;
   }
   m = (y2-y1) / (x2-x1);
   b = (y1 - m*x1);

	return 0;
}


/**********************************************************************

**********************************************************************/

#ifdef WIN32
void Utils::line(HDC hdc, int x1, int y1, int x2, int y2)  {
	MoveToEx(hdc,x1,y1,NULL);
	LineTo(hdc,x2,y2);
   return;
}
#endif

/***********************************************************************

***********************************************************************/

/*
BOOL Utils::exists(char *fname)  {
	FILE *stream;

	stream = fopen(fname,"rb");
	if (stream==NULL)  {
		return FALSE;
	}
	fclose(stream);
	return TRUE;
}
*/

/**********************************************************************
										M A V
	computes the m-point moving average of an array of data x[n].
	m should be odd

'flag' determines whether to extend the ends of the data to the first or
last moving average data
**********************************************************************/


void Utils::mav(float *x,int n,int m,int flag)  {
	int i;
	double sum=0.0;
	int k;
	float *window;
	int ptr=0;

	if (m==0) return;

	window = new float[(unsigned int)m];

	if (window==NULL)  {
		//log.write(10,0,0,"\nno memory for mav window\n");
		return;
	}

	k = m/2;

/* SEED THE MOVING AVERAGE */

	for(i=0;i<m;i++)  {
		window[i] = x[i];
		sum += x[i];
	}

	if (flag)  {
		for(i=0;i<=k;i++)  {
			x[i] = (float) (sum/m);
		}
	}

/* RUN MOVING AVERAGE ON THE ARRAY */

	for(i=k+1;i<n-k;i++)  {
		sum = sum - window[ptr];				/* subtract oldest data */
		window[ptr] = x[i+k];
		sum = sum + window[ptr];				/* add newest data */
		ptr = (ptr+1)%m;
		x[i] = (float) (sum/m);
	}

	if (flag)  {
		for(i=n-k;i<n;i++)  {
			x[i] = x[n-k-1];
		}
	}

	//delete window;
	if (window)  {
		delete [] window;
		window = NULL;
	}

}

/*******************************************************************************

*******************************************************************************/
#ifdef WIN32

void Utils::GradientFill(
							HDC hdc,
							RECT *rect,
							COLORREF from,
							COLORREF to,
							bool leftright)  {

    //#define NBAND 512
    //#define NBAND 256
	int N = 256;

    int fromrgb[3],drgb[3],rgb[3];

	 if (rect==NULL)  {
		 return;
	 }


    // get R,G,B values
    fromrgb[0]=GetRValue(from);
    fromrgb[1]=GetGValue(from);
    fromrgb[2]=GetBValue(from);

    drgb[0]=GetRValue(to) - fromrgb[0];
    drgb[1]=GetGValue(to) - fromrgb[1];
    drgb[2]=GetBValue(to) - fromrgb[2];

    // draw from top to bottom or left to right

    int size,start;
    int left=0, top=0, width=0, height=0;

    if(leftright){
       start= rect->left;
       size=  rect->right - rect->left;
       top=  rect->top;
       height=rect->bottom - rect->top;
    }
	 else{
       start= rect->top;
       size=  rect->bottom - rect->top;
       left=  rect->left;
       width= rect->right - rect->left;
	}

	for(int i=0;i<N;i++)  {

		for(int j=0; j<3; j++){
			rgb[j] = fromrgb[j] + MulDiv(i, drgb[j], 255);
		}

		HBRUSH hbrush=(HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(rgb[0],rgb[1],rgb[2])));
if (hbrush==NULL)  {
	int bp = 1;
}

		int z1 = MulDiv(i,  size, N);
		int z2 = MulDiv(i+1,size, N);

		if(leftright){
			left = start+z1; // from left
			width = z2-z1;
		}
		else{
			top = start+z1; // from top
			height = z2-z1;
		}

		PatBlt(hdc,left,top,width,height,PATCOPY);
		DeleteObject(SelectObject(hdc,hbrush));
	}

	return;
}

#endif

