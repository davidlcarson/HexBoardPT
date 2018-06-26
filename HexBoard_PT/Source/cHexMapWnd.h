//cHexMapWnd.h
//Holds the hext map

#pragma once

#include "ProjDefs.h"
#include "cParWnd.h"


//Default Point Top Hex
//------------------------------------------------------+
class cHEX_SPACE {
private:
	BOOL m_bHilighted;

	//UpperLeft Corner in Screen Coords
	POINTS m_sLocation;

	static int m_nSpaceSize;  //in pixels

	//colors of space
	static COLORREF m_crStandardColor;
	static COLORREF m_crHiliteColor;
	static COLORREF m_crPenColor;

	//ACTOR_INFO m_stActorInfo;

public:
	cHEX_SPACE(void);
	~cHEX_SPACE(void);

	//Set the corner of the square in client pixels
	void SetLocation(POINTS pLocation);
	POINTS GetLocation(void) { return m_sLocation; }

	void SetSpaceSize(int size) { m_nSpaceSize = size; }
	static int GetSpaceSize(void) { return m_nSpaceSize; }

	void SetHilighted(BOOL b) { m_bHilighted = b; }

	POINTS GetCenterCoord(void);

	//Each square contains an ACTOR_INFO structure
	//void SetActorInfo(ACTOR_INFO ai) { m_stActorInfo = ai; }
	//ACTOR_INFO GetActorInfo(void) { return m_stActorInfo; }

	void PaintSpace(HDC hdc);
};

//-----------------------------------------------------+
class cHEX_MAP_WND : public cCHILD_WND {
private:
	//In squares, not pixels!!
	static int m_nFieldSpacesWide;
	static int m_nFieldSpacesTall;

	//margin around spaces
	POINTS m_sMargin{2, 2};

	cHEX_SPACE** m_apSpaces;

	//brush for background area
	HBRUSH m_hBGBrush;

	//-- private methods ----+
	void fillWC(void);

public:
	cHEX_MAP_WND(HWND hParent, int spaceSize, int numSpacesWide, int numSpacesTall);
	~cHEX_MAP_WND(void);

	//Tell someone what size client we'd like
	POINTS DesiredClient(void);

	void OnPaint(HDC hdc);

	LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};




