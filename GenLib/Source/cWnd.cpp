//cWnd.cpp
//gamelib version
//04/19/04

#include "cWnd.h"

//--- globals -------------------------------------------------------------+
const wchar_t* BasicWndClassName = L"BasicWndClass";

//--- local protos --------------------------------------------------------+

LRESULT CALLBACK PlainWndProc(HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam);

//****************************************************************************/
cWND::cWND(void)
{
   m_hWnd = NULL;
   m_hParent = NULL;

   m_pWindowName = L"";      
   m_pClassName = NULL;
   m_bRegistered = false;
   m_id = 0;
   m_bBlockMove = false;
   m_bBlockSize = false;

   //Make a gray brush
   //m_hBGBrush = CreateSolidBrush(RGB(128, 128, 128));
   m_hBGBrush = NULL;  //Allow for no painting
 
   m_style = WS_BORDER;

   //fill the WNDCLASS struct with default values..
   fillWC();

   return;
}

//****************************************************************************/
cWND::~cWND(void)
{   
   //destructors are called in reverse order (this is called last)
   //assume derived classes have cleaned up already

   if(m_hBGBrush)
      DeleteObject(m_hBGBrush);

   if(m_hWnd){
      DestroyWindow(m_hWnd);
      m_hWnd = NULL;
   }

   return;
}

//called by constructor. Fills default data. Leaves func and class NULL
//****************************************************************************/
void cWND::fillWC(void)
{
   ZeroMemory(&m_wc, sizeof(m_wc));
   // Register the Window Class
   m_wc.cbSize        = sizeof(m_wc);
  
   //folloiwng 2 will fail unless filled before call to reg.
   m_wc.lpszClassName = NULL;
   m_wc.lpfnWndProc   = NULL;        

   m_wc.style         = NULL;
   m_wc.hInstance     = NULL;
   m_wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
   m_wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
   m_wc.hbrBackground = NULL; 
   m_wc.lpszMenuName  = NULL; 
   m_wc.cbClsExtra    = 0;
   m_wc.cbWndExtra    = 0;

   return;
}

//****************************************************************************/
void cWND::RegisterWndClass(void)
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

   //see if we're already registered
   bResult = GetClassInfoEx(NULL, m_pClassName, &wc);

   //if result != 0, then this class is registered
   if(bResult != 0){
      //have to copy over because some stuff is missing
      m_wc = wc;      
      m_bRegistered = true;      
   }
   else{

      ATOM result;
      result = RegisterClassEx(&m_wc);

      if(result != 0)
         m_bRegistered = true;         
   }

   return;
}

//****************************************************************************/
void cWND::SetStyle(DWORD style)
{
   MYASSERT(m_hWnd == NULL);

   m_style = style;

   return;
}

/************************************************************************************/
void cWND::AddStyle(DWORD styleMod)
{
   MYASSERT(m_hWnd == NULL);

   m_style |= styleMod;

   return;
}

//****************************************************************************/
void cWND::SetAsChild(HWND hParent, int id)
{
   MYASSERT(m_hWnd == NULL);

   m_hParent = hParent;

   m_style |= (WS_CHILD | WS_CLIPSIBLINGS);
   
   m_id = id;

   return;
}

//****************************************************************************/
HWND cWND::Create(WORD width, WORD height)
{
   //note: m_hWnd may also be set in WM_CREATE, since some
   //functions will require it before we return.
   m_hWnd = CreateWindowEx( 
                   0,                       //extended style
                   m_pClassName,			 //classname
                   m_pWindowName,           //windowbar title
                   
                   m_style,

                   CW_USEDEFAULT,
                   CW_USEDEFAULT,

                   width, height,
                   
                   m_hParent,         //owner or parent

                   NULL,             //hmenu: We set to null. hmenu or
                                     //..child id will be set later
                   NULL,
                   (void*)this);    //window creation data   

   MYASSERT(m_hWnd != NULL);

   //I cannot get window name to come up without adding this
   //..extra call... (This is probably due to earlier call to GetClassInfoEx()
   //SetWindowText(m_hWnd, m_pWindowName);

   return m_hWnd;
}

//****************************************************************************/
void cWND::Show(int cmdShow)
{
   ShowWindow(m_hWnd, cmdShow);

   return;
}

//****************************************************************************/
POINTS cWND::SizeForClient(WORD clientWidth, WORD clientHeight)
{   
   RECT winRect;
   RECT clientRect;

   long ncHeight;
   long ncWidth;

   GetWindowRect(m_hWnd, &winRect);
   GetClientRect(m_hWnd, &clientRect);
   ncWidth = (winRect.right - winRect.left) - clientRect.right;
   ncHeight = (winRect.bottom - winRect.top) - clientRect.bottom;

   //SetWindowPos(m_hWnd, NULL, 0, 0, clientWidth+ncWidth, clientHeight+ncHeight, SWP_NOZORDER | SWP_NOMOVE);
   POINTS retPoints;
   retPoints.x = (WORD)(clientWidth+ncWidth);
   retPoints.y = (WORD)(clientHeight+ncHeight);

   return retPoints;
}

//****************************************************************************/
void cWND::SetBGColor(COLORREF cr)
{
   if(m_hBGBrush)
	   DeleteObject(m_hBGBrush);

	m_hBGBrush = CreateSolidBrush(cr);
       
   return;
}

//****************************************************************************/
//****************************************************************************/
cPLAINWND::cPLAINWND(void) : cWND()
{
   //put our class name in the wc before registering
   m_pClassName = BasicWndClassName;

   m_wc.lpszClassName = m_pClassName;
   m_wc.lpfnWndProc = PlainWndProc;   

   //DeleteObject(m_hBGBrush);
   //m_hBGBrush = CreateSolidBrush(RGB(64, 64, 64));   

   return;
}

//****************************************************************************/
void cPLAINWND::CenterWindow(void)
{
   RECT desktop;
   RECT window;

   //child window?
   if (m_hParent) {
	   GetClientRect(m_hParent, &desktop);
	   GetWindowRect(m_hWnd, &window);
   }
   //nope, desktop window
   else {
	   GetClientRect(GetDesktopWindow(), &desktop);
	   GetWindowRect(m_hWnd, &window);
   }

   SetWindowPos(m_hWnd, NULL, (desktop.right - (window.right-window.left))/2,
      (desktop.bottom - (window.bottom-window.top))/2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

   return;
}

//****************************************************************************/
void cPLAINWND::Say(const wchar_t* pText)
{
   TextOut(GetDC(m_hWnd), 10, 10, pText, wcslen(pText));

   return;
}

//PlainWnd event handler
//****************************************************************************/
LRESULT cPLAINWND::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0;

   switch(uMessage){
      case WM_CREATE:
         break; //0 = continue creation

      case WM_WINDOWPOSCHANGING:
         if(m_bBlockMove == true)
            ((WINDOWPOS*)lParam)->flags |= SWP_NOMOVE;
         if(m_bBlockSize == true)
            ((WINDOWPOS*)lParam)->flags |= SWP_NOSIZE;
         break;

      //This keeps our cursor from becoming a move cursor when
      //moving over the nc area. It doesn't help if the
      //lbutton goes down...
      case WM_NCMOUSEMOVE:
         if(m_bBlockSize == true)
            SetCursor(LoadCursor(NULL, IDC_ARROW));         
         break;
 
      //This is the base for all windows, inclulding parent.
      //if this window has focus, and user presses esc, quit program!
      //If you don't want this behaviour, derive
      //case WM_KEYDOWN:
      //   if(wParam == VK_ESCAPE)
      //      PostQuitMessage(0);         
      //   break;

	  case WM_ERASEBKGND:
		  //we do our own erasing using the current clear brush.
		  //we ignore the class bkbrush. (in fact, can be NULL)

		  //we want to clear. Use our clearbrush        
		  if (m_hBGBrush != NULL) {
			  RECT clientRect;
			  GetClientRect(hWnd, &clientRect);
			  FillRect((HDC)wParam, &clientRect, m_hBGBrush);
		  }

		  //we say we erased, either way
		  lr = 1; //return true if you erase
		  break;

	  case WM_PAINT:
	  {
		  PAINTSTRUCT ps;
		  BeginPaint(hWnd, &ps);
		  EndPaint(hWnd, &ps);
	  }
	  break;
      //window was closed
      //The plain window is not a parent window per se, becuase it doesn't
      //have any chidren. But it is the only window, so when it is destroyed, 
      //the program should end.
      case WM_DESTROY:
         m_hWnd = NULL;
         //PostQuitMessage(0);
         break; //0 if you process

      default:
         lr = DefWindowProc(hWnd, uMessage, wParam, lParam);        
   }

   return lr;
}

//PlainWnd process
/************************************************************************************/
LRESULT CALLBACK PlainWndProc( HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam)
{   
   //this class can have multiple instances -
   //can't be static
   cPLAINWND* pWindow = NULL;
         
   switch(uMessage){

      case WM_NCCREATE: 
         CREATESTRUCT* pcs;        
         pcs = (CREATESTRUCT*)lParam;
         pWindow = (cPLAINWND*)pcs->lpCreateParams;         
         SetWindowLong(hWnd, GWL_USERDATA, (LONG)pWindow);
         return TRUE; //want to create window!

      default:
         pWindow = (cPLAINWND*)GetWindowLong(hWnd, GWL_USERDATA);
         if(pWindow)
            return pWindow->EventHandler(hWnd, uMessage, wParam, lParam);         
         else
            return DefWindowProc(hWnd, uMessage, wParam, lParam);
   }

   return 0;
}
