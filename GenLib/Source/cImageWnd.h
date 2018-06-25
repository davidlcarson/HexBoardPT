#pragma once

#include "Globdefs.h"
#include "cWnd.h"
#include "cImageBuffer.h"

//the image child window
//----------------------------------------------------------------------+
class cIMAGE_WND : public cPLAINWND{
 protected:
   const wchar_t* m_pFilename; //we don't own
   cIMAGE_BUFFER* m_pImageBuffer;

 public:
   cIMAGE_WND(const wchar_t* pFilename); //client from file size
   cIMAGE_WND(cIMAGE_BUFFER* pImageBuffer); //client from imagebuffer size
   cIMAGE_WND(void); //will create from client size

   virtual ~cIMAGE_WND(void);    

   virtual HWND Create(void);
   virtual LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
   WORD GetBufferWide(void){ return m_pImageBuffer->GetPixWide();}
   WORD GetBufferTall(void){ return m_pImageBuffer->GetRowsTall();}
   cIMAGE_BUFFER* GetpBuffer(void){ return m_pImageBuffer;}
};

const WORD kLinePix = 10; //number of pixels to scroll on a line scroll

//the scroll image child window
//----------------------------------------------------------------------+
class cSCIMAGE_WND : public cIMAGE_WND{
 protected:
   //the current corner of the visible portion of the image
   POINTS m_corner;
   WORD m_clientWide;
   WORD m_clientTall;
   WORD m_maxRight;
   WORD m_maxDown;

   SCROLLINFO m_hScroll;
   SCROLLINFO m_vScroll;

   bool m_bScrollingImage;

 public:
   cSCIMAGE_WND(const wchar_t* pFilename);  
   ~cSCIMAGE_WND(void);
   
   POINTS GetCurPosition(void){ return m_corner;}
   
   HWND Create(WORD width = 320, WORD height = 240);
   LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
   
   void SetImagePosition(POINTS pNewCorner);
   void UpdateScrollPosition(void);
};

