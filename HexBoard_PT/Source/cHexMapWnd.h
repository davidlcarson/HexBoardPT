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
	POINTXY m_pxyLocation;

	//size of hex elements
	static SHORT m_spaceHalfWide;
	static SHORT m_spaceQuarterTall;
	//static int m_nSpaceSize;  //in pixels

	//colors of space
	static COLORREF m_crStandardColor;
	static COLORREF m_crHiliteColor;
	static COLORREF m_crPenColor;

	//NOT A POINTER
	ACTOR_INFO m_stActorInfo;

public:
	cHEX_SPACE(void);
	~cHEX_SPACE(void);

	//Following 3 are in hexMap screen pixels
	void SetLocation(POINTXY ptsLocation) { m_pxyLocation = ptsLocation; }
	POINTXY GetLocation(void) const { return m_pxyLocation; }
	POINTXY GetCenterCoord(void) const;

	//Set space element size 
	void SetSpaceSize(int size); 	

	SHORT GetSpaceHalfWide(void) const { return m_spaceHalfWide; }
	SHORT GetSpaceQuarterTall(void) const { return m_spaceQuarterTall; }

	void SetHilighted(BOOL b) { m_bHilighted = b; }	

	//Each square contains an ACTOR_INFO structure
	void SetActorInfo(ACTOR_INFO* pAI) { m_stActorInfo = *pAI; }
	ACTOR_INFO* GetpActorInfo(void) { return & m_stActorInfo; }

	void PaintSpace(HDC hdc) const;
};

//-----------------------------------------------------+
class cHEX_MAP_WND : public cCHILD_WND {
private:
	//In squares, not pixels!!
	static int m_nFieldSpacesWide;
	static int m_nFieldSpacesTall;

	POINTXY m_ptxyMouseMapSize;
	unsigned char* m_pMouseMapIndices;

	//margin around spaces
	POINTXY m_sMargin{2, 2};

	cHEX_SPACE** m_apSpaces;

	//brush for background area
	HBRUSH m_hBGBrush;

	//-- private methods ----+
	void fillWC(void);
	int getSpaceIndex(POINTCR colRow) const;
	void fillMouseMap(const wchar_t* pFilename);
	POINTCR crAdjustment(POINTXY offsetIntoSpace, SHORT row) const;

public:
	cHEX_MAP_WND(HWND hParent, int spaceSize, int numSpacesWide, int numSpacesTall);
	~cHEX_MAP_WND(void);

	//Tell someone what size client we'd like
	POINTXY DesiredClient(void) const;

	//Return center pixel coords of hex space at col/row on map
	POINTXY GetCenterCoord(POINTCR colRow) const;

	//Two methods do get pSpace
	cHEX_SPACE* GetpSpaceCR(POINTCR colRow) const;
	cHEX_SPACE* GetpSpaceXY(SHORT x, SHORT y) const;
	POINTCR GetCRFromXY(POINTXY pxy) { return GetCRFromXY(pxy.x, pxy.y); }
	POINTCR GetCRFromXY(SHORT x, SHORT y) const;

	void OnPaint(HDC hdc) const;

	LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};




