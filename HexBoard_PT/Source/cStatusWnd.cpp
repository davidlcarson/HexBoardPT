//cStatusWnd.cpp

#include "cStatusWnd.h"

const wchar_t* kStatusClassName = L"StatusWndClass";
//forward proto
LRESULT CALLBACK StatusWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

static cSTATUS_WND* sg_pStatusWnd = NULL;

/*********************************************************/
cSTATUS_WND::cSTATUS_WND(HWND hParent, int childID, const wchar_t* pCaption) : cCHILD_WND(hParent, childID, pCaption)
{
   sg_pStatusWnd = this;
   m_pClassName = kStatusClassName;

   m_pCurArmyName = NULL;
   m_pCurrentActor = NULL;

   return;
}

/*********************************************************/
cSTATUS_WND::~cSTATUS_WND(void)
{



}

/*********************************************************/
void cSTATUS_WND::fillWC(void)
{
   /// windows will automatically delete when class ends
   //HBRUSH hBGBrush = CreateSolidBrush(m_crBGColor);

   ZeroMemory(&m_wc, sizeof(m_wc));

   // Register the Window Class
   m_wc.cbSize = sizeof(m_wc);

   //folloiwng 2 will fail unless filled before call to reg.
   m_wc.lpszClassName = m_pClassName;
   m_wc.lpfnWndProc = StatusWndProc;

   m_wc.style = CS_VREDRAW | CS_HREDRAW | CS_CLASSDC;
   m_wc.hInstance = NULL;
   m_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   m_wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
   m_wc.lpszMenuName = NULL;
   m_wc.cbClsExtra = 0;
   m_wc.cbWndExtra = 0;

   return;
}

/*********************************************************/
HWND cSTATUS_WND::CreateChild(DWORD style, int width, int height)
{
   HWND hReturnWnd;
   hReturnWnd = cCHILD_WND::CreateChild(style, width, height);

   m_hwndEndMoveButton = CreateWindow(
      L"BUTTON",  // Predefined class; Unicode assumed 
      L"End Move Phase",      // Button text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
      10,         // x position 
      120,         // y position 
      140,        // Button width
      35,        // Button height
      m_hChildWnd,     // Parent window
      (HMENU)2001,       // No menu.
      0, // (HINSTANCE)GetWindowLong(m_hwnd, GWL_HINSTANCE),
      NULL);      // Pointer not needed.


   return hReturnWnd;
}

/*********************************************************/
void cSTATUS_WND::OnPaint(HDC hdc)
{
   wchar_t string1[512];
   UINT format = DT_TOP | DT_TABSTOP | DT_EXPANDTABS;
   format |= 0x400;  //tab stops = 4?
   RECT rect;

   GetClientRect(m_hChildWnd, &rect);

   DrawText(hdc, m_pOutputString, -1, &rect, format);

#if 0
   wsprintf(string1, L"Current Army:\n\t%s", m_pCurArmyName);
   DrawText(hdc, string1, -1, &rect, format);
   
   //if there's a current actor, show info
   if (m_pCurrentActor) {
      wsprintf(string1, L"Actor: %s\nHit Points: %d\nRange: %d", m_pCurrentActor->GetpActorName(), m_pCurrentActor->GetHitPoints(), m_pCurrentActor->GetRange());
      
      GetClientRect(m_hChildWnd, &rect);
      rect.top += 50;
      rect.left += 4;
      DrawText(hdc, string1, -1, &rect, 0);
   }
#endif

   return;
}

/*******************************************************************************************/
LRESULT cSTATUS_WND::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   LRESULT lr = FALSE;

   switch (uMessage) {


   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);
      sg_pStatusWnd->OnPaint(ps.hdc);
      EndPaint(hWnd, &ps);
   }
   break;
 
   case WM_COMMAND:
      //switch on control ID
      switch (HIWORD(wParam)) {
      case BN_CLICKED:
         if (LOWORD(wParam) == 2001)
            //End MovementPhase Button
            SendMessage(GetParent(hWnd), UM_STATUS_ENDMOVEMENT, wParam, lParam);
         break;
      }
      break;

   default:
      lr = cCHILD_WND::EventHandler(hWnd, uMessage, wParam, lParam);
   }

   return lr;
}

/**************************************************************************************/
LRESULT CALLBACK StatusWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   LRESULT lr = FALSE;

   switch (uMessage) {
   case WM_CREATE:
      lr = TRUE;
      break;
 
   default:
      
      if(sg_pStatusWnd) {
         lr = sg_pStatusWnd->EventHandler(hWnd, uMessage, wParam, lParam);
      }
      else
         lr = DefWindowProc(hWnd, uMessage, wParam, lParam);
      break;
   }

   return lr;
}
