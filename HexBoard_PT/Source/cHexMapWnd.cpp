//cHexMapWnd.cpp
//Point Top Version
//180625 : dc

#include "cHexMapWnd.h"
#include "DebugRoutines.h"

const wchar_t* kHexWndClassName = L"HexWndClass";

//forward proto
LRESULT CALLBACK HexWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

//access for Proc
static cHEX_MAP_WND* sg_pHexMapWnd = NULL;

int cHEX_MAP_WND::m_nFieldSpacesWide = 0;
int cHEX_MAP_WND::m_nFieldSpacesTall = 0;
int cHEX_SPACE::m_nSpaceSize = 0;

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
	m_nSpaceSize = 12;

	return;
}

/****************************************************/
cHEX_SPACE::~cHEX_SPACE(void)
{

}

#if 0
//upper-left corner this square in (parent client) pixels
/****************************************************/
void cHEX_SPACE::SetLocation(POINTS psLocation)
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

	int sq3sz = (int)(SQRT3 * m_nSpaceSize);
	int sq3sz2 = sq3sz << 1;

	//polygon version
	POINT apts[6];
	apts[0].x = m_ptsLocation.x;
	apts[0].y = m_ptsLocation.y + m_nSpaceSize;
	apts[1].x = m_ptsLocation.x + sq3sz;
	apts[1].y = m_ptsLocation.y;
	apts[2].x = m_ptsLocation.x + sq3sz2;
	apts[2].y = m_ptsLocation.y + m_nSpaceSize;
	apts[3].x = m_ptsLocation.x + sq3sz2;
	apts[3].y = m_ptsLocation.y + (3 * m_nSpaceSize);
	apts[4].x = m_ptsLocation.x + sq3sz;
	apts[4].y = m_ptsLocation.y + (4 * m_nSpaceSize);
	apts[5].x = m_ptsLocation.x;
	apts[5].y = m_ptsLocation.y + (3 * m_nSpaceSize);

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
POINTS cHEX_SPACE::GetCenterCoord(void) const
{
	POINTS retValue;
	float x = m_ptsLocation.x + (SQRT3 * m_nSpaceSize);
	float y = m_ptsLocation.y + ( 2.0f * m_nSpaceSize);
	retValue.x = (SHORT)x;
	retValue.y = (SHORT)y;
	
	return (POINTS)retValue;
}

/*****************************************************************************/
/*****************************************************************************/
cHEX_MAP_WND::cHEX_MAP_WND(HWND hParent, int spaceSize, int numSpacesWide, int numSpacesTall) : cCHILD_WND(hParent, childID)
{
	sg_pHexMapWnd = this;
	m_pClassName = kHexWndClassName;
	m_apSpaces = NULL;

	m_nFieldSpacesWide = numSpacesWide;
	m_nFieldSpacesTall = numSpacesTall;

	m_hBGBrush = CreateSolidBrush(RGB(190, 180, 100));

	//Create an array of hexes spaces wide x s[aces tall
	int numSquares = m_nFieldSpacesWide * m_nFieldSpacesTall;
	m_apSpaces = new cHEX_SPACE*[numSquares];

	//Create and Store Corners
	int currentSpace = 0;
	//POINTS currentLocation{ 0, 0 };	
	int sq3sz = (int)(spaceSize * SQRT3);

	POINTS currentCorner = m_sMargin;

	//Create array of hexes and init location
	for (int i = 0; i < numSpacesTall; i++) {

		//Draw  one row of hexes
		for (int i2 = 0; i2 < numSpacesWide; i2++) {
			m_apSpaces[currentSpace] = new cHEX_SPACE();
			m_apSpaces[currentSpace]->SetLocation(currentCorner);
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

/*****************************************************************************/
POINTS cHEX_MAP_WND::GetCenterCoord(POINTS colRow) const
{
	int space;
	MYASSERT(colRow.x <= m_nFieldSpacesWide);
	MYASSERT(colRow.y <= m_nFieldSpacesTall);

	//Get space in array from col/Row
	space = (colRow.y * m_nFieldSpacesWide) + colRow.x;

	POINTS retValue = m_apSpaces[space]->GetCenterCoord();

	return retValue;
}

/*****************************************************************************/
POINTS cHEX_MAP_WND::DesiredClient(void) const
{
	POINTS retPt{ 0, 0 };

	MYASSERT(m_apSpaces != NULL);
	int spaceSize = m_apSpaces[0]->GetSpaceSize();
	MYASSERT(spaceSize != NULL);

	MYASSERT(m_nFieldSpacesWide != 0);

	POINTS workingPt = (POINTS)m_apSpaces[m_nFieldSpacesWide - 1]->GetLocation();
	workingPt.x += (SHORT) (SQRT3 * spaceSize * 3);
	retPt.x = workingPt.x + m_sMargin.x;

	workingPt = m_apSpaces[(m_nFieldSpacesTall  -1) * m_nFieldSpacesWide]->GetLocation();
	workingPt.y += spaceSize * 4;
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
		//Let GameMaster handle this
		SendMessage(GetParent(hWnd), UM_ARENA_LBUTTON_DOWN, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
	{
		SHORT x;
		SHORT y;
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		SendMessage(GetParent(hWnd), UM_ARENA_MOUSEMOVE, wParam, lParam);
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

