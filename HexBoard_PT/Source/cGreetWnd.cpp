//cGreetWnd.cpp

#include "cGreetWnd.h"
#include "DebugRoutines.h"
#include "cParWnd.h"

//--- static globals --------------------------------------+
const wchar_t* GreetWndClassName = L"GreetWndClass";

//--- local protos ---------------------------------------+
LRESULT CALLBACK GreetWndProc(HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam);

//****************************************************************************/
cGREETWND::cGREETWND(const wchar_t* pFilename)
{   
	//parent is probably desktop
	m_hGreetWnd = NULL;
	m_pClassName = GreetWndClassName;

   m_bTimedOut = false;

   memset(&m_wc, 0, sizeof(WNDCLASSEX));

   //create the image buffer
   m_pImageBuffer = CreateImageBuffer(pFilename);

   return;
}

//****************************************************************************/
cGREETWND::~cGREETWND(void)
{
   //stop messages coming to this window
	CLEAN_DELETE(m_pImageBuffer);

   if(m_hGreetWnd){
      DestroyWindow(m_hGreetWnd);
      m_hGreetWnd = NULL;
   }

   return;
}

/************************************************************************************/
bool cGREETWND::RegisterGreetWnd(void)
{   
   if(GetClassInfoEx(0, m_pClassName, &m_wc))
      return true;

   m_wc.cbSize     = sizeof(m_wc);
   m_wc.lpszClassName = m_pClassName;
   m_wc.lpfnWndProc   = GreetWndProc;
   m_wc.style         = CS_CLASSDC | CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
   m_wc.hInstance     = 0;
   m_wc.hIcon      = LoadIcon(0, (LPCWSTR)IDI_APPLICATION);
   m_wc.hIconSm    = LoadIcon(0, (LPCWSTR)IDI_APPLICATION);
   m_wc.hCursor    = LoadCursor( NULL, IDC_ARROW );
   m_wc.hbrBackground = NULL; //application handles erasing
   m_wc.lpszMenuName  = NULL; //MAKEINTRESOURCE(IDR_MENU1);
   m_wc.cbClsExtra    = 0;
   m_wc.cbWndExtra    = 0;

   if( RegisterClassEx( &m_wc ) == 0 ){
      MYASSERT(0);
     return false;
   }

   return true;
}

/****************************************************************************************************/
HWND cGREETWND::CreateGreetWnd(void)
{
   DWORD style = WS_POPUP; // WS_BORDER;

   m_hGreetWnd = CreateWindowEx(
      0,             //DWORD extended window style,
      L"GreetWndClass",  //LPCSTR lpClassName
      (LPCWSTR)m_pImageBuffer->GetFileName(),
      style, //DWORD dwStyle
      0, //int x
      0, //int y
      320, //int nWidth
      240, //int nHeight
      NULL, //HWND hParent
      NULL, //HMENU hMenu or child id
      0, //HINSTANCE hInstance
      (void*)this); //LPVOID lpParam

   MYASSERT(m_hGreetWnd != NULL);
   
   SizeForClient(m_hGreetWnd, (WORD)m_pImageBuffer->GetPixWide(), (WORD)m_pImageBuffer->GetRowsTall());
   CenterWindow(m_hGreetWnd);

   return m_hGreetWnd;
}

//****************************************************************************/
void cGREETWND::Show(int how)
{
   MYASSERT(m_hGreetWnd != NULL);
   ShowWindow(m_hGreetWnd, how);

  return;
}

const UINT_PTR GREET_TIMER = 1001;

//GreetWnd event handler
//****************************************************************************/
LRESULT cGREETWND::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0;

   switch(uMessage){     

      case WM_CREATE:        
         //we want to self-destruct in 8 seconds:
         SetTimer(hWnd, GREET_TIMER, 3000, NULL);
		 lr = 0;
         break;

      case WM_LBUTTONDOWN:
         m_bTimedOut = true;
         break;

      case WM_TIMER:
         m_bTimedOut = true;
         KillTimer(hWnd, GREET_TIMER);
         break;

      case WM_PAINT:
         {
         PAINTSTRUCT ps;
         BeginPaint(hWnd, &ps);

		 //high quality stretching
         SetStretchBltMode(ps.hdc, HALFTONE);
         //probably not necessary unless we're using brushes
         //SetBrushOrgEx(ps.hdc, 0, 0, NULL);

         //for debugging
         //!!! was -1 on each!!
         int destWide = m_pImageBuffer->GetPixWide();
         int destTall = m_pImageBuffer->GetRowsTall();

         int srcWide = destWide;
         int srcTall = destTall;

         CONST void* pBits = m_pImageBuffer->GetpBits();
         BITMAPINFO* pbmi = m_pImageBuffer->GetpBmi();

         StretchDIBits(ps.hdc, 
             0, 0, destWide, destTall,
             0, 0,
             srcWide, srcTall,
             pBits, pbmi, 
             DIB_RGB_COLORS, SRCCOPY);

         EndPaint(hWnd, &ps);
		 lr = 0;
         }
         break;

      //Close program when parent window closes
      case WM_DESTROY:
         m_hGreetWnd = NULL;
         break;

      default:
         lr = DefWindowProc( hWnd, uMessage, wParam, lParam );    
		 break;
   }

   return lr;
}

//can be static
static cGREETWND* pGreetWnd = NULL;

//GreetWnd process
/************************************************************************************/
LRESULT CALLBACK GreetWndProc( HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam)
{   
               
   switch(uMessage){

      case WM_NCCREATE: 
         CREATESTRUCT* pcs;        
         pcs = (CREATESTRUCT*)lParam;
         pGreetWnd = (cGREETWND*)pcs->lpCreateParams;                  

         return TRUE; //want to create window!

      default:         
         if(pGreetWnd){            
            return pGreetWnd->EventHandler(hWnd, uMessage, wParam, lParam);         
         }
         else
            return DefWindowProc(hWnd, uMessage, wParam, lParam);
   }

   return 0;
}
