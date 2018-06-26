//cHexMapWnd.cpp

#include "cHexMapWnd.h"

const wchar_t* kHexWndClassName = L"HexWndClass";

//forward proto
LRESULT CALLBACK HexWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

//access for Proc
static cHEX_MAP_WND* sg_pHexMapWnd = NULL;

int cHEX_MAP_WND::m_nFieldSpacesWide = 0;
int cHEX_MAP_WND::m_nFieldSpacesTall = 0;

//colors of square
COLORREF cHEX_SPACE::m_crStandardColor = RGB(210, 210, 210);
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

//upper-left corner this square in (parent client) pixels
/****************************************************/
void cHEX_SPACE::SetLocation(POINTS psLocation)
{
	m_sLocation = psLocation;
}

/****************************************************/
void cHEX_SPACE::PaintSpace(HDC hdc)
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

	POINTS corner{ 0, 0 };
	int sz = 8;
	int sq3sz = (int)(SQRT3 * sz);

#if 1 //pointy top
	//Draw 3 rows of hexes
	for (int i = 0; i < 3; i++) {

		//Draw  one row of hexes
		for (int i2 = 0; i2 < 5; i2++) {

			//Draw one (pointy top) hex
			MoveToEx(hdc, corner.x, corner.y + sz, NULL);
			LineTo(hdc, corner.x + sq3sz, corner.y);
			LineTo(hdc, corner.x + (sq3sz * 2), corner.y + sz);
			LineTo(hdc, corner.x + (sq3sz * 2), corner.y + (3 * sz));
			LineTo(hdc, corner.x + sq3sz, corner.y + (4 * sz));
			LineTo(hdc, corner.x, corner.y + (3 * sz));
			LineTo(hdc, corner.x, corner.y + sz);

			//next hex in row ->
			corner.x += sq3sz * 2;
		}
		//reset for next row ->

		if (i % 2)
			corner.x = 0;
		else
			corner.x = sq3sz;

		corner.y = corner.y + (sz * 3);
	}
#else

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
/*****************************************************************************/
cHEX_MAP_WND::cHEX_MAP_WND(HWND hParent, int numSpacesWide, int numSpacesTall) : cCHILD_WND(hParent, childID)
{
	sg_pHexMapWnd = this;
	m_pClassName = kHexWndClassName;
	m_apSpaces = NULL;

	m_nFieldSpacesWide = numSpacesWide;
	m_nFieldSpacesTall = numSpacesTall;

	//Create an array of hexes spaces wide x s[aces tall
	int numSquares = m_nFieldSpacesWide * m_nFieldSpacesTall;
	m_apSpaces = new cHEX_SPACE*[numSquares];
	for (int i = 0; i < numSquares; i++)
		m_apSpaces[i] = new cHEX_SPACE();


	return;
}

/*****************************************************************************/
cHEX_MAP_WND::~cHEX_MAP_WND(void)
{
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

//Called directly from GameMaster so it can paint armies on top
/*****************************************************************************/
void cHEX_MAP_WND::OnPaint(HDC hdc)
{
	//Clear the child window
	RECT client;
	GetClientRect(m_hChildWnd, &client);

	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	Rectangle(hdc, 0, 0, client.right, client.bottom);

#if 0
	int numSpaces = m_nFieldSpacesTall * m_nFieldSpacesWide;

	for (int i = 0; i < numSpaces; i++)
		m_apSpaces[i]->PaintSpace(hdc);
#endif
	//Paint the spaces
	m_apSpaces[0]->PaintSpace(hdc);

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

