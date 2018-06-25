//cParWnd.cpp
//01/30/2013
//based on
// new simple version
// 11/30/04

#include <stdio.h>  //FILE
#include "cParWnd.h"

#include "DebugRoutines.h"
#include "LibResource.h"

const wchar_t* pParClassName = L"PARWND";

//--- prototype ------------------------------------------------------------------+
//These are implemented in other modules. Samples are in this module, below.
LRESULT CALLBACK ParWndProc( HWND hWnd, UINT uMessage, 
                 WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT uMessage,
   WPARAM wParam, LPARAM lParam);

//****************************************************************************/
cPARWND::cPARWND(const wchar_t* pWindowName)
{
   m_pClassName = pParClassName;
   m_pWindowName = pWindowName;
   //m_hInstance = hInstance;
   m_style = WS_OVERLAPPEDWINDOW;  //assume
   m_hClearBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

   memset(&m_wc, 0, sizeof(WNDCLASSEX));
   m_hWnd = NULL;

   return;
}

//****************************************************************************/
cPARWND::~cPARWND(void)
{
   DeleteObject(m_hClearBrush);

   return;
} 

/************************************************************************************/
void cPARWND::Show(int how)
{
   MYASSERT(m_hWnd != NULL);
   ShowWindow(m_hWnd, how);

  return;
}

/************************************************************************************/
bool cPARWND::Register(void)
{   
   if(GetClassInfoEx(NULL, m_pClassName, &m_wc))
   return true;

   // Register the Window Class
   m_wc.cbSize     = sizeof(m_wc);
   m_wc.lpszClassName = m_pClassName;
   m_wc.lpfnWndProc   = ParWndProc;
   m_wc.style         = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
   m_wc.hInstance     = NULL;
   m_wc.hIcon      = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hIconSm    = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hCursor    = LoadCursor( NULL, IDC_ARROW );
   m_wc.hbrBackground = NULL; //application handles erasing client
   m_wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
   m_wc.cbClsExtra    = 0;
   m_wc.cbWndExtra    = 0;

   if( RegisterClassEx( &m_wc ) == 0 )
     return false;

   return true;
}

/************************************************************************************/
HWND cPARWND::CreateWnd(WORD width, WORD height)
{             
   m_hWnd = CreateWindowEx( 
          0,//WS_EX_TOPMOST,   
          m_pClassName,        //classname
          m_pWindowName,       //windowbar title
          m_style,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          width,
          height,
          GetDesktopWindow(),  //NULL,        //owner or parent
          NULL,        //menu handle or child id
          NULL,
          (void*)this);    //window creation data   

   //if this trips, might be because WNDPROC isn't returning
   //default messagees for creation....
   MYASSERT(m_hWnd != NULL);

   m_hdc = ::GetDC(m_hWnd);

   return m_hWnd;
}

//probably will be derived
/************************************************************************************/
LRESULT cPARWND::EventHandler( HWND hWnd, UINT uMessage, 
                 WPARAM wParam, LPARAM lParam)
{
   LRESULT lr=0;   
      
   switch(uMessage){
   case WM_CREATE:
      {      
      lr = 1;
      }
      break;

   case WM_COMMAND:  // message: command from application menu
      switch(LOWORD(wParam)){
      
      case IDM_EXIT:
         DestroyWindow(hWnd);
         break;
      default:
         break;
      }

      lr = 0;
      break;      

   case WM_SYSCOMMAND:
      if(wParam == SC_CLOSE)
         lr = DestroyWindow(hWnd);            
      else
         lr = DefWindowProc(hWnd, uMessage, wParam, lParam);

      break;

   case WM_DESTROY:      
      PostQuitMessage(1);
      lr = 0;
      break;

   case WM_ERASEBKGND:
	   RECT clientRect;
	   GetClientRect(hWnd, &clientRect);
	   FillRect((HDC)wParam, &clientRect, m_hClearBrush);
	   lr = true; //non-zero means we erased
	   break;

   case WM_WINDOWPOSCHANGING:
      if(m_bBlockMove == true)
         ((WINDOWPOS*)lParam)->flags |= SWP_NOMOVE;
      if(m_bBlockSize == true)
         ((WINDOWPOS*)lParam)->flags |= SWP_NOSIZE;
      break;

   case WM_PAINT:
      {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);      
      
      EndPaint(hWnd, &ps);
      lr = 0;
      }
      break;

   default:
      lr = DefWindowProc( hWnd, uMessage, wParam, lParam );
   }

   return lr;
}

#if 0  //PUT IN ANOTHER MODULE
//The parent window process
//*************************************************************************************************
LRESULT CALLBACK ParWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   //possible multiple windows of this type means no static
   cPARWND* pWindow = NULL;
   LRESULT lr = 0;
         
   switch(msg){

      case WM_NCCREATE: 
         CREATESTRUCT* pcs;        
         pcs = (CREATESTRUCT*)lParam;
         pWindow = (cPARWND*)pcs->lpCreateParams;
         SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);

         lr = 1; //want to create window!

      default:         
		  LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		  pWindow = (cPARWND*)ptr;
         if(pWindow){            
            lr = pWindow->EventHandler(hWnd, msg, wParam, lParam);         
         }
         else
            lr = DefWindowProc(hWnd, msg, wParam, lParam);
   }

   return lr;
}
#endif

const wchar_t* ChildWndClassName = L"ChildWndClass";

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT uMessage,
   WPARAM wParam, LPARAM lParam);

/**************************************************************************/
cCHILD_WND::cCHILD_WND(HWND hParent, int childID, const wchar_t* pCaption)
{
   m_hParent = hParent;
   m_pClassName = ChildWndClassName;
   m_childID = childID;

   m_wc.lpszClassName = m_pClassName;
   m_wc.lpfnWndProc = ChildWndProc;

   //default to darkish gray
   m_crBGColor = RGB(200, 200, 200);

   //Don't block resizing until window is created and sized for client
   m_bBlockReSize = FALSE;
   m_bBlockMove = FALSE;

   return;
}

/*************************************************************************/
   cCHILD_WND::~cCHILD_WND(void)
{

}

//****************************************************************************/
void cCHILD_WND::fillWC(void)
{
   /// windows will automatically delete when class ends
   HBRUSH hBGBrush = CreateSolidBrush(m_crBGColor);

   ZeroMemory(&m_wc, sizeof(m_wc));

   // Register the Window Class
   m_wc.cbSize = sizeof(m_wc);

   //folloiwng 2 will fail unless filled before call to reg.
   m_wc.lpszClassName = m_pClassName;
   m_wc.lpfnWndProc = ChildWndProc;

   m_wc.style = CS_VREDRAW | CS_HREDRAW;
   m_wc.hInstance = NULL;
   m_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
   m_wc.hbrBackground = hBGBrush;
   m_wc.lpszMenuName = NULL;
   m_wc.cbClsExtra = 0;
   m_wc.cbWndExtra = 0;

   return;
}

//****************************************************************************/
bool cCHILD_WND::RegisterChild(void)
{
   /*
   If the class is already registered, registering it will cause result to be
   not true (even tho there's no problem). So, to prevent this, I first call GetClassInfo,
   which returns true if the class is already registered...
   ... but for some reason, calling it when
   not yet registered causes the classname to null out in wc!!! so I call it with a
   pointer to a local wc...
   Which means if the class is already registered, but the wc's don't match, there
   could be a problem...

   Maybe if returns true I should copy the local wc to the member wc..??
   */

   BOOL bResult;
   WNDCLASSEX wc;

   //if false, menu and class names are nulled out!!!
   bResult = GetClassInfoEx(NULL, m_pClassName, &wc);

   if (bResult != 0) {
      m_wc = wc;
      return true;
   }

   fillWC();

   ATOM result;

   result = RegisterClassEx(&m_wc);

   if (result == 0)
      return false;
   else
      return true;
}

/*************************************************************************/
HWND cCHILD_WND::CreateChild(DWORD style, int width, int height)
{
   //make sure child is flagged
   //style |= (WS_CHILD | WS_BORDER);
   style |= WS_CHILD;

   m_hChildWnd = CreateWindowEx(
      0,                    //extended style
      m_pClassName,      //classname
      m_pWindowName,     //windowbar title

      style,

      0,
      0,
      width,
      height,
      m_hParent,              //owner or parent

      (HMENU)m_childID,              //menu handle or child id
                                     //..null == use class menu
      NULL,    //hInstance
      (void*)this);    //window creation data   

   MYASSERT(m_hChildWnd != NULL);

   return m_hChildWnd;
}

/*************************************************************************/
void cCHILD_WND::Show(int how)
{
   ShowWindow(m_hChildWnd, how);
}

//****************************************************************************/
LRESULT cCHILD_WND::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   LRESULT lr = 0;

   switch (uMessage) {
   case WM_CREATE:
      lr = TRUE;
      break;

   case WM_WINDOWPOSCHANGING:
      if (m_bBlockMove == TRUE)
         ((WINDOWPOS*)lParam)->flags |= SWP_NOMOVE;
      if (m_bBlockReSize == TRUE)
         ((WINDOWPOS*)lParam)->flags |= SWP_NOSIZE;
      break;

   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      BeginPaint(hWnd, &ps);

      EndPaint(hWnd, &ps);
   }
   break;

   //Close program when parent window closes
   case WM_DESTROY:
      m_hChildWnd = NULL;
      break;

   default:
      lr = DefWindowProc(hWnd, uMessage, wParam, lParam);
   }

   return lr;
}
//Child Window Process. May want to move to another module...
/************************************************************************************/
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT uMessage,
   WPARAM wParam, LPARAM lParam)
{
   LRESULT lr = 0;

   //Multiple children means no static
   cCHILD_WND* pWindow = NULL;

   switch (uMessage) {

   case WM_NCCREATE:
      CREATESTRUCT * pcs;
      pcs = (CREATESTRUCT*)lParam;
      pWindow = (cCHILD_WND*)pcs->lpCreateParams;
      SetWindowLong(hWnd, GWL_USERDATA, (long)pWindow);

      lr = TRUE;
      break;

   default:
      pWindow = (cCHILD_WND*)GetWindowLong(hWnd, GWL_USERDATA);
      if (pWindow) {
         lr = pWindow->EventHandler(hWnd, uMessage, wParam, lParam);
      }
      else
         lr = DefWindowProc(hWnd, uMessage, wParam, lParam);
   }

   return lr;
}


/*************************************************************************/
/******************************* General Routines *******************/
//Re-sizes the window so the client area is a desired size.
/**********************************************************************/
POINTS SizeForClient(HWND hWnd, WORD width, WORD height)
{
   long ncHeight;
   long ncWidth;
   POINTS retPt;

   RECT windowRect;
   RECT clientRect;

   GetWindowRect(hWnd, &windowRect);
   GetClientRect(hWnd, &clientRect);

   ncWidth = (windowRect.right - windowRect.left) - clientRect.right;
   ncHeight = (windowRect.bottom - windowRect.top) - clientRect.bottom;

   SetWindowPos(hWnd, NULL, 0, 0, width + ncWidth, height + ncHeight, SWP_NOZORDER | SWP_NOMOVE);

   retPt.x = (WORD)(width + ncWidth);
   retPt.y = (WORD)(width + ncHeight);

   return retPt;
}

//Centers the window on the desktop
/**********************************************************************/
void CenterWindow(HWND hWnd)
{
   RECT windowRect;
   RECT desktopRect;

   GetWindowRect(GetDesktopWindow(), &desktopRect);
   GetWindowRect(hWnd, &windowRect);

   SetWindowPos(hWnd, NULL, ((desktopRect.right-desktopRect.left)-(windowRect.right-windowRect.left))/2, 
      ((desktopRect.bottom-desktopRect.top)-(windowRect.bottom-windowRect.top))/2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

   return;
}



