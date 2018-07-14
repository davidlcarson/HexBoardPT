//cHexMapWnd.cpp
//Point Top Version
//180625 : dc

#include "cHexMapWnd.h"
#include "DebugRoutines.h"
#include "cImageBuffer.h"

const wchar_t* kHexWndClassName = L"HexWndClass";

//forward proto
LRESULT CALLBACK HexWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

//access for Proc
static cHEX_MAP_WND* sg_pHexMapWnd = NULL;

int cHEX_MAP_WND::m_nFieldSpacesWide = 0;
int cHEX_MAP_WND::m_nFieldSpacesTall = 0;
//int cHEX_SPACE::m_nSpaceSize = 0;
SHORT cHEX_SPACE::m_spaceHalfWide = 0;
SHORT cHEX_SPACE::m_spaceQuarterTall = 0;

//colors of square
COLORREF cHEX_SPACE::m_crStandardColor = RGB(220, 220, 220);
COLORREF cHEX_SPACE::m_crHiliteColor = RGB(255, 255, 255);
COLORREF cHEX_SPACE::m_crPenColor = RGB(0, 0, 0);

//HexMap Child ID
const int childID = 1958;

/****************************************************/
cHEX_SPACE::cHEX_SPACE(void)
{
	m_bHilighted = FALSE;	

	return;
}

/****************************************************/
cHEX_SPACE::~cHEX_SPACE(void)
{

}

/****************************************************/
void cHEX_SPACE::SetSpaceSize(int SpaceSize)
{
	//set the space dimension (1/2 wide, 1/4 tall)
	m_spaceHalfWide = (SHORT)(SpaceSize * SQRT3);
	m_spaceQuarterTall = (SHORT)SpaceSize;

	return;
}

#if 0
//upper-left corner this square in (parent client) pixels
/****************************************************/
void cHEX_SPACE::SetLocation(POINTXY psLocation)
{
	m_sLocation = psLocation;
}
#endif

/****************************************************/
void cHEX_SPACE::PaintSpace(HDC hdc) const
{
	HBRUSH hBrush;
	HPEN hPen;

	if (m_bHilighted) {
		hBrush = CreateSolidBrush(m_crHiliteColor);
	}
	else {
		hBrush = CreateSolidBrush(m_crStandardColor);
	}
	hPen = CreatePen(PS_SOLID, 1, m_crPenColor);

	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);

	//int sq3sz = (int)(SQRT3 * m_nSpaceSize);
	//int sq3sz2 = sq3sz * 2;

	//polygon version
	POINT apts[6];
	apts[0].x = m_ptsLocation.x;
	apts[0].y = m_ptsLocation.y + m_spaceQuarterTall; // m_nSpaceSize;
	apts[1].x = m_ptsLocation.x + m_spaceHalfWide; //sq3sz;
	apts[1].y = m_ptsLocation.y;
	apts[2].x = m_ptsLocation.x + m_spaceHalfWide * 2; // sq3sz2;
	apts[2].y = m_ptsLocation.y + m_spaceQuarterTall; // m_nSpaceSize;
	apts[3].x = m_ptsLocation.x + m_spaceHalfWide * 2; // sq3sz2;
	apts[3].y = m_ptsLocation.y + m_spaceQuarterTall * 3; //  (3 * m_nSpaceSize);
	apts[4].x = m_ptsLocation.x + m_spaceHalfWide; // sq3sz;
	apts[4].y = m_ptsLocation.y + m_spaceQuarterTall * 4; // (4 * m_nSpaceSize);
	apts[5].x = m_ptsLocation.x;
	apts[5].y = m_ptsLocation.y + m_spaceQuarterTall * 3; // (3 * m_nSpaceSize);

	Polygon(hdc, apts, 6);

#if 0 /// line version
	//Draw one (pointy top) hex
	MoveToEx(hdc, m_sLocation.x, m_sLocation.y + m_nSpaceSize, NULL);
	LineTo(hdc, m_sLocation.x + sq3sz, m_sLocation.y);
	LineTo(hdc, m_sLocation.x + sq3sz2, m_sLocation.y + m_nSpaceSize);
	LineTo(hdc, m_sLocation.x + sq3sz2, m_sLocation.y + (3 * m_nSpaceSize));
	LineTo(hdc, m_sLocation.x + sq3sz, m_sLocation.y + (4 * m_nSpaceSize));
	LineTo(hdc, m_sLocation.x, m_sLocation.y + (3 * m_nSpaceSize));
	LineTo(hdc, m_sLocation.x, m_sLocation.y + m_nSpaceSize);
#endif

#if 0
	//poiny side
	//Draw columns of hexes   
	for (int i = 0; i < 3; i++) {

		//Draw  one column of hexes
		for (int i2 = 0; i2 < 5; i2++) {

			//Draw one (pointy side) hex
			MoveToEx(hdc, corner.x + sz, corner.y, NULL);
			LineTo(hdc, corner.x + (3 * sz), corner.y);
			LineTo(hdc, corner.x + (4 * sz), corner.y + (sq3sz));
			LineTo(hdc, corner.x + (3 * sz), corner.y + (sq3sz * 2));
			LineTo(hdc, corner.x + sz, corner.y + (sq3sz * 2));
			LineTo(hdc, corner.x, corner.y + sq3sz);
			LineTo(hdc, corner.x + sz, corner.y);

			//next hex in col V
			corner.y += sq3sz * 2;
		}
		//reset for next column V

		if (i % 2)
			corner.y = 0;
		else
			corner.y = sq3sz;

		corner.x = corner.x + (sz * 3);
	}
#endif
	DeleteObject(hBrush);
	DeleteObject(hPen);

	return;
}

/*****************************************************************************/
POINTXY cHEX_SPACE::GetCenterCoord(void) const
{
	POINTXY retValue;
	SHORT x = m_ptsLocation.x + m_spaceHalfWide; // (SQRT3 * m_nSpaceSize);
	SHORT y = m_ptsLocation.y + m_spaceQuarterTall * 2; // (2.0f * m_nSpaceSize);
	retValue.x = x;
	retValue.y = y;
	
	return (POINTXY)retValue;
}

const wchar_t* KMOUSEMAP_FILENAME = L"../Textures/MouseMap.bmp";

/*****************************************************************************/
/*****************************************************************************/
cHEX_MAP_WND::cHEX_MAP_WND(HWND hParent, int spaceSize, int numSpacesWide, int numSpacesTall) : cCHILD_WND(hParent, childID)
{
	sg_pHexMapWnd = this;
	m_pClassName = kHexWndClassName;
	m_apSpaces = NULL;

	m_nFieldSpacesWide = numSpacesWide;
	m_nFieldSpacesTall = numSpacesTall;

	fillMouseMap(KMOUSEMAP_FILENAME);

	m_hBGBrush = CreateSolidBrush(RGB(190, 180, 100));

	//Create an array of hexes spaces wide x s[aces tall
	int numSquares = m_nFieldSpacesWide * m_nFieldSpacesTall;
	m_apSpaces = new cHEX_SPACE*[numSquares];

	//Create and Store Corners
	int currentSpace = 0;
	//POINTXY currentLocation{ 0, 0 };	
	int sq3sz = (int)(spaceSize * SQRT3);

	POINTXY currentCorner = m_sMargin;

	//Create array of hexes and init location
	for (int i = 0; i < numSpacesTall; i++) {

		//Draw  one row of hexes
		for (int i2 = 0; i2 < numSpacesWide; i2++) {
			m_apSpaces[currentSpace] = new cHEX_SPACE();
			m_apSpaces[currentSpace]->SetLocation(currentCorner);

			//debug: print all corners
			//wchar_t string[128];
			//swprintf_s(string, 128, L"HexCorner %d: %d x %d\n", currentSpace, currentCorner.x, currentCorner.y);
			//debugsay(string);

			currentSpace++;
			//next hex in row ->
			currentCorner.x += (sq3sz * 2);			
		}
		//reset for next row ->

		if (i % 2)
			currentCorner.x = m_sMargin.x;			
		else
			currentCorner.x = sq3sz + m_sMargin.x;
			
		currentCorner.y = currentCorner.y + (spaceSize * 3);		
	}

	m_apSpaces[0]->SetSpaceSize(spaceSize);

	return;
}

/*****************************************************************************/
cHEX_MAP_WND::~cHEX_MAP_WND(void)
{
	DeleteObject(m_hBGBrush);

	CLEAN_DELETE(m_pMouseMapIndices);

	int numSpaces = m_nFieldSpacesWide * m_nFieldSpacesTall;

	//delete the instances
	for (int i = 0; i < numSpaces; i++)
		CLEAN_DELETE(m_apSpaces[i]);

	//delete the (now null) pointers;
	delete[] m_apSpaces;

	return;
}

//THIS IS CALLED FROM GENLIB cCHILD_WND
/***************************************************************/
void cHEX_MAP_WND::fillWC(void)
{
	/// windows will automatically delete when class ends
	//HBRUSH hBGBrush = CreateSolidBrush(m_crBGColor);

	ZeroMemory(&m_wc, sizeof(m_wc));

	// Register the Window Class
	m_wc.cbSize = sizeof(m_wc);

	//folloiwng 2 will fail unless filled before call to reg.
	m_wc.lpszClassName = m_pClassName;
	m_wc.lpfnWndProc = HexWndProc;

	m_wc.style = CS_VREDRAW | CS_HREDRAW | CS_CLASSDC;
	m_wc.hInstance = NULL;
	m_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	m_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	m_wc.hbrBackground = NULL; //This has to be NULL, or Window gets painted over
	m_wc.lpszMenuName = NULL;
	m_wc.cbClsExtra = 0;
	m_wc.cbWndExtra = 0;

	return;
}

//Convert .bmp image file to array of indices (3 values)
/*******************************************************************/
void cHEX_MAP_WND::fillMouseMap(const wchar_t* pFilename)
{
	//Filename includes path

	//debug
	//wchar_t curDir[128];
	//GetCurrentDirectory(128, curDir);

	cIMAGE_BUFFER8* pImageBuffer;
	pImageBuffer = new cIMAGE_BUFFER8(pFilename);
	unsigned char* pBits = (unsigned char*)pImageBuffer->GetpBits();
	WORD tall = (WORD)pImageBuffer->GetRowsTall();
	WORD wide = (WORD)pImageBuffer->GetPaddedWide();
	int size = tall * wide;

	m_ptxyMouseMapSize.x = wide;
	m_ptxyMouseMapSize.y = tall;
	m_pMouseMapIndices = new unsigned char[size];
	for (int i = 0; i < size; i++)
		m_pMouseMapIndices[i] = pBits[i];

	CLEAN_DELETE(pImageBuffer);

	return;
}

//Helper funciton
//Converts column/row into the absolute index into the array of spaces
/******************************************************************************/
int cHEX_MAP_WND::getSpaceIndex(POINTCR colRow) const
{
	int spaceIndex;
	MYASSERT(colRow.col <= m_nFieldSpacesWide);
	MYASSERT(colRow.row <= m_nFieldSpacesTall);

	//Get space in array from col/Row
	spaceIndex = (colRow.row * m_nFieldSpacesWide) + colRow.col;

	return spaceIndex;
}

//addjust the calculated CR by mousemap
/******************************************************************************/
POINTCR cHEX_MAP_WND::crAdjustment(POINTXY xy, SHORT row) const
{
	POINTCR retPoint = POINTCR{ 0, 0 };	

	SHORT spaceWidth = m_apSpaces[0]->GetSpaceHalfWide() * 2;
	SHORT spaceHeight = m_apSpaces[0]->GetSpaceQuarterTall() * 3;

	float scaleX = m_ptxyMouseMapSize.x / (float)spaceWidth;
	float scaleY = m_ptxyMouseMapSize.y / (float)spaceHeight;
	
	SHORT effectiveX = (SHORT)(xy.x * scaleX);
	SHORT effectiveY = (SHORT)(xy.y * scaleY);

	//translate xy into absolute index	
	int index = (effectiveY * m_ptxyMouseMapSize.x) + effectiveX;

	//get indices[index]
	unsigned char adjustment = m_pMouseMapIndices[index];
	switch (adjustment) {

	//Black: No Adjust
	case 4:
		break;

	//Red
	case 1:
		//odd row
		if (row % 2) {
			retPoint.row = -1;		
		}
		//even row
		else {							
			retPoint.row = -1;
			retPoint.col = -1;
		}
		break;

	//Blue
	case 0:
		//odd rows
		if (row % 2) {
			retPoint.row = -1;
			retPoint.col = 1;
		}
		//even row
		else
			retPoint.row = -1;

		break;

	default:
		MYASSERT(0);
		break;
	}

	//Need to accomodate final row!!

	return retPoint;
}

//Converts screen x/y into hex column/row
/*****************************************************************************/
POINTCR cHEX_MAP_WND::GetCRFromXY(SHORT x, SHORT y) const
{			
	int col;
	int row;	
	POINTXY offsetIntoSpace; //mouse map coord
	POINTCR retPoint = POINTCR{ 0, 0 };

	//int size = m_apSpaces[0]->GetSpaceSize();	
	SHORT quarterSize = m_apSpaces[0]->GetSpaceQuarterTall();
	SHORT halfSize = m_apSpaces[0]->GetSpaceHalfWide();

	//Get the Row (up/down) first since it will affect the column (<->)
	//deduct top margin
	y = y - m_sMargin.y;
	row = y / (quarterSize * 3);	

	//coord is in empty space at bottom
	if (row >= m_nFieldSpacesTall) {
		retPoint.row = -1;
		return retPoint;
	}
	
	offsetIntoSpace.y = y % (quarterSize * 3);

	//now can do column <>
	//Deduct margin from mouse x
    x = x - m_sMargin.x;
		
	//if odd row, deduct 1/2 hex width from mouxe x
	if (row % 2) {
		x -= (SHORT)(halfSize);
		//x is in margin area left side
		if (x < 0) {
			retPoint.col = -1;
			return retPoint;
		}

	}
	col = (SHORT)(x / (halfSize * 2));

	if(col >= m_nFieldSpacesWide) {
		retPoint.col = -1;
		return retPoint;
	}

	offsetIntoSpace.x = x % (SHORT)(halfSize * 2);

	POINTCR crAdjust = crAdjustment(offsetIntoSpace, row);
	col += crAdjust.col;
	row += crAdjust.row;

	retPoint.col = col;
	retPoint.row = row;

	return retPoint;
}

/*****************************************************************************/
cHEX_SPACE* cHEX_MAP_WND::GetpSpaceCR(POINTCR colRow) const
{
	int spaceIndex = getSpaceIndex(colRow);
	return m_apSpaces[spaceIndex];
}

//input is in pixels
/*****************************************************************************/
cHEX_SPACE* cHEX_MAP_WND::GetpSpaceXY(SHORT x, SHORT y) const
{
	cHEX_SPACE* pHexReturn = NULL;

	POINTCR cr = GetCRFromXY(x, y);
	if(!(cr.col < 0 || cr.row < 0))
		pHexReturn = GetpSpaceCR(POINTCR{ (SHORT)cr.col, (SHORT)cr.row });

	return pHexReturn;
}

/*****************************************************************************/
POINTXY cHEX_MAP_WND::GetCenterCoord(POINTCR colRow) const
{
	
	return GetpSpaceCR(colRow)->GetCenterCoord();
}

/*****************************************************************************/
POINTXY cHEX_MAP_WND::DesiredClient(void) const
{
	POINTXY retPt{ 0, 0 };

	MYASSERT(m_apSpaces != NULL);
	//int spaceSize = m_apSpaces[0]->GetSpaceSize();
	//MYASSERT(spaceSize != NULL);
	SHORT quarterSize = m_apSpaces[0]->GetSpaceQuarterTall();
	SHORT halfSize = m_apSpaces[0]->GetSpaceHalfWide();

	MYASSERT(m_nFieldSpacesWide != 0);

	POINTXY workingPt = (POINTXY)m_apSpaces[m_nFieldSpacesWide - 1]->GetLocation();
	workingPt.x += halfSize * 3; //(SHORT) (SQRT3 * spaceSize * 3);
	retPt.x = workingPt.x + m_sMargin.x;

	workingPt = m_apSpaces[(m_nFieldSpacesTall  -1) * m_nFieldSpacesWide]->GetLocation();
	workingPt.y += quarterSize * 4; //spaceSize * 4;
	retPt.y = workingPt.y + m_sMargin.y;

	return retPt;
}

//Called directly from GameMaster so it can paint armies on top
/*****************************************************************************/
void cHEX_MAP_WND::OnPaint(HDC hdc) const
{
	//Clear the child window
	RECT client;
	GetClientRect(m_hChildWnd, &client);

	SelectObject(hdc, m_hBGBrush);
	Rectangle(hdc, 0, 0, client.right, client.bottom);

	int numSpaces = m_nFieldSpacesTall * m_nFieldSpacesWide;

	for (int i = 0; i < numSpaces; i++)
		m_apSpaces[i]->PaintSpace(hdc);

	return;
}

/*****************************************************************************/
LRESULT cHEX_MAP_WND::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = FALSE;

	switch (uMessage) {

	//painting is handled by GameMaster calling OnPaint directly
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);

		lr = FALSE;  //0 = Say I processed
	}
	break;

	case WM_LBUTTONDOWN:
	{
		HWND hParent = GetParent(hWnd);
		//Let GameMaster handle this
		SendMessage(hParent, UM_MAPWND_LBUTTON_DOWN, wParam, lParam);
	}
		break;

	case WM_MOUSEMOVE:
	{
		SHORT x;
		SHORT y;
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);

		SendMessage(GetParent(hWnd), UM_MAPWND_MOUSEMOVE, wParam, lParam);
	}
	break;

	default:
		lr = cCHILD_WND::EventHandler(hWnd, uMessage, wParam, lParam);
		break;
	}

	return lr;
}

/**************************************************************************************/
LRESULT CALLBACK HexWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = FALSE;

	switch (uMessage) {

	case WM_CREATE:
		lr = TRUE;
		break;

	default:
		if (sg_pHexMapWnd) {
			lr = sg_pHexMapWnd->EventHandler(hWnd, uMessage, wParam, lParam);
		}
		else
			lr = DefWindowProc(hWnd, uMessage, wParam, lParam);
		break;
	}

	return lr;
}

