//cHexMapWnd.h
//Holds the hext map

#pragma once

#include "ProjDefs.h"
#include "cParWnd.h"


const int childID = 1958;

//-----------------------------------------------------+
class cHEX_MAP_WND : public cCHILD_WND {
protected:

	void fillWC(void);

public:
	cHEX_MAP_WND(HWND hParent, int numSpacesWide, int numSpacesTall);
	~cHEX_MAP_WND(void);

	void OnPaint(void);

	LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};




