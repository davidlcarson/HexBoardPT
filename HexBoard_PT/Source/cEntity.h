//cEntity.h
//Everything that can appear in the game
// 180627 : dc

#pragma once

#include "cHexMapWnd.h"

//--------------------------------------------------------------+
class cENTITY {
private:
	POINTCR m_ptsLocation; //x, y col/row on map

	//We don't own
	//cHEX_SPACE* m_pCurrentSpace;

	//eENTITY_TYPE m_eEntityType;

	//BOOL m_bActive;
	//BOOL m_bVisible;

	static COLORREF m_PenColor;
	static COLORREF m_FillColor;

protected:

public:
	cENTITY(eENTITY_TYPE eType = eEntity_Unknown);
	~cENTITY(void);

	//Following two are in col/row on map
	void SetLocation(POINTCR Location) { m_ptsLocation = Location; }
	POINTCR GetLocation(void) const { return m_ptsLocation; }

	//void SetVisible(BOOL b) { m_bVisible = b; }

	//void SetActive(BOOL b) { m_bActive = b; }
	//BOOL GetActive(void) { return m_bActive; }

	void Paint(HDC hdc, POINTXY hexCenter) const;
};
