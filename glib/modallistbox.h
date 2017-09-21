
#pragma once

#include <vector>
#include <string>

#include <modaldialog.h>

/********************************************************************
	example from flonet project:


	std::vector<std::string> str;
	std::string ts;

	for(k=0; k<2; k++)  {
		if (k==0)  {
			s = &forward_sensor;
			strcpy(gstring, "FORWARD SENSOR");
			ts = gstring;
			str.push_back(ts);
		}
		else  {
			s = &return_sensor;
			strcpy(gstring, "RETURN SENSOR");
			ts = gstring;
			str.push_back(ts);
		}

		
		sprintf(gstring, "  sn = %d", s->sn);
		ts = gstring;
		str.push_back(ts);

		sprintf(gstring, "   date = %s", s->date);
		ts = gstring;
		str.push_back(ts);

		sprintf(gstring, "   time = %s", s->time);
		ts = gstring;
		str.push_back(ts);

		sprintf(gstring, "   op id = %d", s->opid);
		ts = gstring;
		str.push_back(ts);

		sprintf(gstring, "   lastname = %s", s->lastname);
		ts = gstring;
		str.push_back(ts);

		sprintf(gstring, "   firstname = %s", s->firstname);
		ts = gstring;
		str.push_back(ts);

		sprintf(gstring, "   blocktype = %s", s->blockType);
		ts = gstring;
		str.push_back(ts);

		sprintf(gstring, "   npoints = %d", s->points.size());
		ts = gstring;
		str.push_back(ts);

		for(i=0; i<s->points.size(); i++)  {
			sprintf(gstring, "      %8.3f    %8.3f", s->points[i].flowrate, s->points[i].hz);
			ts = gstring;
			str.push_back(ts);
		}

	}

	modalListBox *mlb = new modalListBox(NULL, "Sensors", str, CW_USEDEFAULT, CW_USEDEFAULT, w, h);
	DEL(mlb);
	InvalidateRect(hwnd, 0, TRUE);

********************************************************************/

class modalListBox : public modalDialog  {

	public:
		//modalListBox(HWND _phwnd, char *_title, char **items, int _x=0, int _y=0, int _w=0, int _h=0);
		modalListBox(HWND _phwnd, char *_title, const std::vector<std::string> &str, int _x=0, int _y=0, int _w=0, int _h=0, bool _autocenter=true );
		virtual ~modalListBox();

	private:
		#define IDC_LISTBOX 22
		HWND hlist;
		HDC listhdc;

		virtual void wm_command(WPARAM wParam, LPARAM lParam);
		virtual void wm_keydown(WPARAM wParam, LPARAM lParam);
		virtual void wm_keyup(WPARAM wParam, LPARAM lParam);
		virtual void wm_user(WPARAM wParam, LPARAM lParam);

};

