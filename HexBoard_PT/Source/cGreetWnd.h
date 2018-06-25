#pragma once

//cGreetWnd.h

#include "Utilities.h"
#include "cImageBuffer.h"

//--------------------------------------------+
class cGREETWND {
private:
   HWND m_hGreetWnd;
   const wchar_t* m_pClassName;
   bool m_bTimedOut;
   WNDCLASSEX m_wc;

   cIMAGE_BUFFER* m_pImageBuffer;

public:
   cGREETWND(const wchar_t* pFilename);
   ~cGREETWND(void);

   bool RegisterGreetWnd(void);
   HWND CreateGreetWnd(void);
   void Show(int how = SW_NORMAL);

   bool IsTimedOut(void) { return m_bTimedOut; }

   HWND GethWnd(void) { return m_hGreetWnd; }

   LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};