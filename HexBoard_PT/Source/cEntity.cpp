//cEntity.cpp
#if 0
#include "cEntity.h"

COLORREF cENTITY::m_PenColor = RGB(0, 0, 0);
COLORREF cENTITY::m_FillColor = RGB(255, 255, 255);

/*****************************************************/
cENTITY::cENTITY(eENTITY_TYPE eType)
{
	//m_sLocation = POINTXY{ 0, 0 };

	//m_pCurrentSpace = NULL;
	//m_eEntityType = eType;

	//m_bActive = TRUE;
	//m_bVisible = TRUE;

	return;
}

/*****************************************************/
cENTITY::~cENTITY(void)
{

	return;
}

/*****************************************************/
void cENTITY::Paint(HDC hdc, POINTXY hexCenter) const
{
	//receives: hexCenter in pixels
	int size = kSPACE_SIZE;

	//if(m_bVisible)
		//if(m_bActive)
			//if (m_pCurrentSpace != 0) {
				//POINTXY center = m_pCurrentSpace->GetCenterCoord();
				HBRUSH fillBrush = CreateSolidBrush(m_FillColor);
				HPEN pen = CreatePen(PS_SOLID, 2, m_PenColor);

				SelectObject(hdc, fillBrush);
				SelectObject(hdc, pen);

				Rectangle(hdc, hexCenter.x - size, hexCenter.y - size, hexCenter.x+size, hexCenter.y+size);

				DeleteObject(fillBrush);
				DeleteObject(pen);
				
			//}
	
	return;
}
#endif