
//cImageWnd.cpp
//New version
//4/24/04

#include "cImageWnd.h"

const wchar_t* ImageChildWndClassName = L"ImageChildClass";
const wchar_t* ScrollImageChildWndClassName = L"ScrollImageChildClass";

LRESULT CALLBACK ImageChildWndProc( HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK ScrollImageChildWndProc( HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam);

//This version we will creat buffer size of file image, set window client to fit
//****************************************************************************/
cIMAGE_WND::cIMAGE_WND(const wchar_t* pFilename) :
    cPLAINWND()
{
   m_style = (WS_CAPTION | WS_CLIPSIBLINGS);
    
   //Store, but we don't own
   m_pFilename = pFilename;

   MYASSERT(m_pImageBuffer == NULL);
   m_pImageBuffer = CreateImageBuffer(m_pFilename);

   m_pClassName = ImageChildWndClassName;

   m_pWindowName = pFilename;

   m_wc.lpszClassName = m_pClassName;   
   m_wc.lpfnWndProc = ImageChildWndProc;   

   //m_hClearBrush = NULL;

   return;
}

//This version we will set client to buffer handed us (is now ours)
//****************************************************************************/
cIMAGE_WND::cIMAGE_WND(cIMAGE_BUFFER* pImageBuffer)
{
   MYASSERT(pImageBuffer->GetRowsTall() > 31);
   MYASSERT(pImageBuffer->GetPixWide() > 31);

   m_style = (WS_CAPTION | WS_CLIPSIBLINGS);
   m_pFilename = NULL;

   m_pImageBuffer = pImageBuffer;

   m_pWindowName = m_pFilename;

   m_wc.lpszClassName = m_pClassName;   
   m_wc.lpfnWndProc = ImageChildWndProc;   

   //m_hClearBrush = NULL;

   return;
}

//This verison, derived class has to implement its own Create
//****************************************************************************/
cIMAGE_WND::cIMAGE_WND(void)
{
   //no caption, this version (yet)
   m_style = WS_CLIPSIBLINGS;

   m_pFilename = NULL;
   m_pImageBuffer = NULL;

   m_pWindowName = m_pFilename;  //though no caption this version

   m_wc.lpszClassName = m_pClassName;   
   m_wc.lpfnWndProc = ImageChildWndProc;   

   return;
}

//****************************************************************************/
cIMAGE_WND::~cIMAGE_WND(void)
{
	cWND::~cWND();
	
	CLEAN_DELETE(m_pImageBuffer);

   return;
}

//****************************************************************************/
HWND cIMAGE_WND::Create(void)
{            

   HWND hWnd;
   //Create an arbitrary size, and count on WM_CREATE to re-size for
   //client area!
   hWnd = cPLAINWND::Create(320, 240);

   return hWnd;
}

//****************************************************************************/
//****************************************************************************/
cSCIMAGE_WND::cSCIMAGE_WND(const wchar_t* pFilename) :
    cIMAGE_WND(pFilename)
{
   m_corner.x = 0;
   m_corner.y = 0; 
   m_clientWide = 0;
   m_clientTall = 0;
   m_maxRight = 0;
   m_maxDown = 0;  

   m_bScrollingImage = false;

   //brush was set to null in cIMAGE_WND

   m_pClassName = ScrollImageChildWndClassName;

   m_wc.lpszClassName = m_pClassName;   
   m_wc.lpfnWndProc = ScrollImageChildWndProc;  
   
   return;
}

//****************************************************************************/
cSCIMAGE_WND::~cSCIMAGE_WND(void)
{
   if(m_hWnd){
      DestroyWindow(m_hWnd);
      m_hWnd = NULL;
   }

   return;
}

//****************************************************************************/
HWND cSCIMAGE_WND::Create(WORD width, WORD height)
{

   DWORD newStyle;
   HWND hWnd = NULL;

   newStyle = m_style | (WS_HSCROLL | WS_VSCROLL | WS_CAPTION | WS_CLIPSIBLINGS);

   //Parent Window not showing yet

   //Child create is more appropriate than image_child's
   hWnd = cPLAINWND::Create(width, height);

   SetWindowText(hWnd, m_pFilename);   

   return hWnd;
}

//****************************************************************************/
void cSCIMAGE_WND::SetImagePosition(POINTS newCorner)
{
   //Called (indirectly) by the navigator window.

   //we just set the corner of our visible window to the new coordinates.   
   m_corner.x = (short)newCorner.x;
   m_corner.y = (short)newCorner.y;
  
   return;
}

//****************************************************************************/
void cSCIMAGE_WND::UpdateScrollPosition(void)
{

   m_vScroll.fMask = SIF_POS;               
   m_vScroll.nPos = m_corner.y;
   SetScrollInfo(m_hWnd, SB_VERT, &m_vScroll, TRUE);               

   m_hScroll.fMask = SIF_POS;               
   m_hScroll.nPos = m_corner.x;
   SetScrollInfo(m_hWnd, SB_HORZ, &m_hScroll, TRUE);
   
   return;
}

//****************************************************************************/
//****************************************************************************/
LRESULT cIMAGE_WND::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   switch(uMessage){
      case WM_CREATE:
         {

         //if there's no imagebuffer, derived class HAS to implement its
         //own Create routine!
         MYASSERT(m_pImageBuffer != NULL);

         POINTS winSize;  
         m_hWnd = hWnd;
         winSize = SizeForClient(m_pImageBuffer->GetPixWide(), m_pImageBuffer->GetRowsTall());
         SetWindowPos(hWnd, NULL, 0, 0, winSize.x, winSize.y, SWP_NOZORDER | SWP_NOMOVE);
         }
         break;

         
      case WM_PAINT:
         {
         PAINTSTRUCT ps;
         BeginPaint(hWnd, &ps);

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
         }
         break;

      default:
         return cPLAINWND::EventHandler(hWnd, uMessage, wParam, lParam);
   }

   return 0;
}

//****************************************************************************/
LRESULT cSCIMAGE_WND::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{

   static BOOL bRepaint = FALSE;

   static bool bMoveImage = false;
   //static bool bScrollingImage = false;

   static WORD prevMouseXPos = 0;
   static WORD prevMouseYPos = 0;

   switch(uMessage){

      case WM_CREATE:
         ZeroMemory(&m_hScroll, sizeof(SCROLLINFO));
         m_hScroll.cbSize = sizeof(SCROLLINFO);                 
         m_hScroll.nMin = 0;       
         m_hScroll.nMax = 100; //arbitrary for now
         SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, bRepaint);

         ZeroMemory(&m_vScroll, sizeof(SCROLLINFO));
         m_vScroll.cbSize = sizeof(SCROLLINFO);                 
         m_vScroll.fMask = SIF_RANGE;
         m_vScroll.nMin = 0;
         m_vScroll.nMax = 100; //arbitrary for now
         SetScrollInfo(hWnd, SB_VERT, &m_vScroll, bRepaint);
         break;

      case WM_SIZE:

         m_clientWide = LOWORD(lParam);

         MYASSERT(m_pImageBuffer != NULL);
         m_hScroll.nMax = m_pImageBuffer->GetPixWide();         
         m_hScroll.nPage = m_clientWide;
         m_hScroll.fMask = SIF_RANGE | SIF_PAGE;         

         SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, bRepaint);

         m_clientTall = HIWORD(lParam);         
         m_vScroll.nMax = m_pImageBuffer->GetRowsTall();
         m_vScroll.nPage = m_clientTall;
         m_vScroll.fMask = SIF_RANGE | SIF_PAGE;

         SetScrollInfo(hWnd, SB_VERT, &m_vScroll, bRepaint);

         m_maxRight = m_pImageBuffer->GetPixWide() - m_clientWide;         
         m_maxDown = m_pImageBuffer->GetRowsTall() - m_clientTall;

         break;

      case WM_HSCROLL:

         switch(LOWORD(wParam)){          

            case SB_THUMBTRACK:
               //we're tracking. Update corner and re-paint
               m_corner.x = (short)HIWORD(wParam);
               m_hScroll.fMask = SIF_POS;               
               m_hScroll.nPos = HIWORD(wParam);
               SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, TRUE);               

               InvalidateRect(hWnd, NULL, false);

               break;
           
            case SB_THUMBPOSITION:               
               //freeze thumb at current position.
               m_hScroll.fMask = SIF_POS;               
               m_hScroll.nPos = HIWORD(wParam);
               SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, TRUE);

               InvalidateRect(hWnd, NULL, false);

               break;         

            case SB_PAGELEFT:
               m_hScroll.fMask = SIF_POS;
               m_corner.x -= m_clientWide;
               if(m_corner.x < 0)
                  m_corner.x = 0;

               m_hScroll.nPos = m_corner.x;
               SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, TRUE);

               InvalidateRect(hWnd, NULL, false);

               break;

            case SB_PAGERIGHT:
               m_hScroll.fMask = SIF_POS;
               m_corner.x += m_clientWide;
               if(m_corner.x > m_maxRight)
                  m_corner.x = m_maxRight;

               m_hScroll.nPos = m_corner.x;               
               SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, TRUE);
               
               InvalidateRect(hWnd, NULL, false);

               break;

            case SB_LINELEFT:
               m_hScroll.fMask = SIF_POS;
               m_corner.x -= kLinePix;
               if(m_corner.x < 0)
                  m_corner.x = 0;

               m_hScroll.nPos = m_corner.x;
               SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, TRUE);
               
               InvalidateRect(hWnd, NULL, false);

               break;

            case SB_LINERIGHT:
               m_hScroll.fMask = SIF_POS;
               m_corner.x += kLinePix;
               if(m_corner.x > m_maxRight)
                  m_corner.x = m_maxRight;

               m_hScroll.nPos = m_corner.x;
               SetScrollInfo(hWnd, SB_HORZ, &m_hScroll, TRUE);

               InvalidateRect(hWnd, NULL, false);

               break;

         }

         break;

      case WM_VSCROLL:

         switch(LOWORD(wParam)){          
            case SB_THUMBTRACK:
               //see notes above
               m_corner.y = (short)HIWORD(wParam);
               m_vScroll.fMask = SIF_POS;               
               m_vScroll.nPos = HIWORD(wParam);
               SetScrollInfo(hWnd, SB_VERT, &m_vScroll, TRUE);               

               InvalidateRect(hWnd, NULL, false);

               break;

            case SB_THUMBPOSITION:               
               //see notes above
               m_vScroll.fMask = SIF_POS;               
               m_vScroll.nPos = HIWORD(wParam);
               SetScrollInfo(hWnd, SB_VERT, &m_vScroll, TRUE);               

               InvalidateRect(hWnd, NULL, false);

               break;         

            case SB_PAGELEFT:
               m_vScroll.fMask = SIF_POS;
               m_corner.y -= m_clientTall;               
               if(m_corner.y < 0)
                  m_corner.y = 0;

               m_vScroll.nPos = m_corner.y;
               SetScrollInfo(hWnd, SB_VERT, &m_vScroll, TRUE);

               InvalidateRect(hWnd, NULL, false);

               break;

            case SB_PAGERIGHT:
               m_vScroll.fMask = SIF_POS;
               m_corner.y += m_clientTall;
               if(m_corner.y > m_maxDown)
                  m_corner.y = m_maxDown;

               m_vScroll.nPos = m_corner.y;
               SetScrollInfo(hWnd, SB_VERT, &m_vScroll, TRUE);

               InvalidateRect(hWnd, NULL, false);

               break;

            case SB_LINELEFT:
               m_vScroll.fMask = SIF_POS;
               m_corner.y -= kLinePix;
               if(m_corner.y < 0)
                  m_corner.y = 0;

               m_vScroll.nPos = m_corner.y;
               SetScrollInfo(hWnd, SB_VERT, &m_vScroll, TRUE);

               InvalidateRect(hWnd, NULL, false);

               break;

            case SB_LINERIGHT:
               m_vScroll.fMask = SIF_POS;
               m_corner.y += kLinePix;
               if(m_corner.y > m_maxDown)
                  m_corner.y = m_maxDown;

               m_vScroll.nPos = m_corner.y;
               SetScrollInfo(hWnd, SB_VERT, &m_vScroll, TRUE);

               InvalidateRect(hWnd, NULL, false);

               break;
         }
         break;

      case WM_KEYDOWN:
         switch(wParam){
            case VK_SPACE:              
              SetCapture(hWnd);
              SetCursor(LoadCursor(NULL, IDC_HAND));                         
              bMoveImage = true;
              break;
         }
         break;

      case WM_KEYUP:
         ReleaseCapture();
         SetCursor(LoadCursor(NULL, IDC_ARROW));         
         bMoveImage = false;
         break;

      case WM_LBUTTONDOWN:
         if(bMoveImage){
           prevMouseXPos = GET_X_LPARAM(lParam); 
           prevMouseYPos = GET_Y_LPARAM(lParam); 

           m_bScrollingImage = true;
         }
         break;

      case WM_LBUTTONUP:                  
         m_bScrollingImage = false;
         break;

       case WM_MOUSEMOVE:
         if(m_bScrollingImage){
            int right;
            int down;

            int xparam; //the x position of the mouse
            xparam = GET_X_LPARAM(lParam);

            //calculate amount we've moved to the right (may be neg if moved to left)
            //adjust corner x by that amount, and temporarily call it 'right'
            right = m_corner.x - (xparam - prevMouseXPos);

            if(right > m_maxRight)
              right = m_maxRight;
            if(right < 0)
              right = 0;            
            m_corner.x = right;            

            int yparam; //the y position of the mouse
            yparam = GET_Y_LPARAM(lParam);

            down = m_corner.y - (yparam - prevMouseYPos);
            if(down > m_maxDown)
               down = m_maxDown;
            if(down < 0)
               down = 0;
            m_corner.y = down;

            //our current position is now the previous
            prevMouseXPos = xparam;
            prevMouseYPos = yparam;
            UpdateScrollPosition();

            //SendMessage(m_hParent, UM_NEW_IMAGEPOS, 0, NULL);
            InvalidateRect(hWnd, NULL, FALSE);

            return 0; //0 = we processed
         }
         else
            return 1; //1 = didn't process
         
      case WM_PAINT:
         {
         PAINTSTRUCT ps;
         BeginPaint(hWnd, &ps);
         
         SetStretchBltMode(ps.hdc, HALFTONE);
         //probably not necessary unless we're using brushes
         //SetBrushOrgEx(ps.hdc, 0, 0, NULL);         

         StretchDIBits(ps.hdc, 
             0, 0,  //dest x/y
             m_clientWide, m_clientTall,  //dest width, height

             m_corner.x, //source x
             (m_pImageBuffer->GetRowsTall()-m_clientTall)-m_corner.y, //source y
             m_clientWide, m_clientTall,  //source width, height

             m_pImageBuffer->GetpBits(), m_pImageBuffer->GetpBmi(), 
             DIB_RGB_COLORS, SRCCOPY);

         EndPaint(hWnd, &ps);
         }
         break;

      default:
         return cIMAGE_WND::EventHandler(hWnd, uMessage, wParam, lParam);
   }

   return 0;
}

/************************************************************************************/
LRESULT CALLBACK ImageChildWndProc( HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam)
{   
   //Multiple children means no static
   cIMAGE_WND* pWindow = NULL;
         
   switch(uMessage){

      case WM_NCCREATE: 
         CREATESTRUCT* pcs;        
         pcs = (CREATESTRUCT*)lParam;
         pWindow = (cIMAGE_WND*)pcs->lpCreateParams;                  
         SetWindowLong(hWnd, GWL_USERDATA, (long)pWindow);

         return TRUE; //want to create window!

      default:         
         pWindow = (cIMAGE_WND*)GetWindowLong(hWnd, GWL_USERDATA);
         if(pWindow){            
            return pWindow->EventHandler(hWnd, uMessage, wParam, lParam);         
         }
         else
            return DefWindowProc(hWnd, uMessage, wParam, lParam);
   }

   return 0;
}

/************************************************************************************/
LRESULT CALLBACK ScrollImageChildWndProc( HWND hWnd, UINT uMessage, 
                                WPARAM wParam, LPARAM lParam)
{   
   //Multiple children means no static
   cSCIMAGE_WND* pWindow = NULL;
         
   switch(uMessage){

      case WM_NCCREATE: 
         CREATESTRUCT* pcs;        
         pcs = (CREATESTRUCT*)lParam;
         pWindow = (cSCIMAGE_WND*)pcs->lpCreateParams;                  
         SetWindowLong(hWnd, GWL_USERDATA, (long)pWindow);

         return TRUE; //want to create window!

      default:         
         pWindow = (cSCIMAGE_WND*)GetWindowLong(hWnd, GWL_USERDATA);
         if(pWindow){            
            return pWindow->EventHandler(hWnd, uMessage, wParam, lParam);         
         }
         else
            return DefWindowProc(hWnd, uMessage, wParam, lParam);
   }

   return 0;
}
