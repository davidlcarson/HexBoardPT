//cHexMapWnd.cpp

#include "cHexMapWnd.h"

const wchar_t* kHexWndClassName = L"HexWndClass";

//forward proto
LRESULT CALLBACK HexWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

//access for Proc
static cHEX_MAP_WND* sg_pHexMapWnd = NULL;

/*****************************************************************************/
cHEX_MAP_WND::cHEX_MAP_WND(HWND hParent, int numSpacesWide, int numSpacesTall) : cCHILD_WND(hParent, childID)
{
	sg_pHexMapWnd = this;
	m_pClassName = kHexWndClassName;

	return;
}

/*****************************************************************************/
cHEX_MAP_WND::~cHEX_MAP_WND(void)
{

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
	m_wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //TEMP
	m_wc.lpszMenuName = NULL;
	m_wc.cbClsExtra = 0;
	m_wc.cbWndExtra = 0;

	return;
}



/*****************************************************************************/
void cHEX_MAP_WND::OnPaint(void)
{

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

