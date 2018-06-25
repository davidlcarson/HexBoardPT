//cParWnd_2013_1.h
//01/30/2013
//based on code from GenLib 10

#pragma once

#include "GlobDefs.h"

//--------------------------------------------------------------------------------+
class cPARWND{
 protected:
   HWND m_hWnd;    
   WNDCLASSEX m_wc;
   const wchar_t* m_pClassName;
   HBRUSH m_hClearBrush;

   bool m_bBlockMove;
   bool m_bBlockSize;

   const wchar_t* m_pWindowName;
   
   DWORD m_style;
   HDC m_hdc;

 public:
   cPARWND(const wchar_t* pWindowName);
   ~cPARWND(void);

   HWND GethWnd(void){ return m_hWnd;}

   void BlockMove(bool b){ m_bBlockMove = b;}
   void BlockSize(bool b){ m_bBlockSize = b;}

   //HINSTANCE GethInstance(void){ return m_hInstance;}
   HDC GetDC(void){ return m_hdc;}
   void Show(int how = SW_SHOW);
   virtual bool Register(void);
   HWND CreateWnd(WORD width = (WORD)640, WORD height = (WORD)480);   

   virtual LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

};

//--------------------------------------------------------------------------------+
class cCHILD_WND{
protected:
   HWND m_hParent;
   HWND m_hChildWnd;

   int m_childID;

   BOOL m_bBlockReSize;
   BOOL m_bBlockMove;

   WNDCLASSEX m_wc;
   const wchar_t* m_pClassName;
   const wchar_t* m_pWindowName;

   COLORREF m_crBGColor;

   //This is where wndproc can be set in derived
   virtual void fillWC(void);

public:
   cCHILD_WND(HWND hParent, int childID, const wchar_t* pCaption = NULL);
   ~cCHILD_WND(void);

   HWND GethWnd(void) {return m_hChildWnd;}

   //Block resizing of window?
   void SetBlockReSize(BOOL b) { m_bBlockReSize = b; }
   void SetBlockMove(BOOL b) { m_bBlockMove = b; }

   bool RegisterChild(void);
   virtual HWND CreateChild(DWORD style, int width, int height);
   virtual LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
   
   void Show(int how = SW_SHOW);
};


#if 0 //not using
//--------------------------------------------------------------------------------+
class cPOPUP_WND{
private:
   HWND m_hPopupWnd;
   HINSTANCE m_hInstance;
   WNDCLASSEX m_wc;
   char* m_pClassName;
   const char* m_pWIndowName;
   HDC m_hdc;

public:
   cPOPUP_WND(HINSTANCE hInstance);
   ~cPOPUP_WND(void);

   bool RegisterPopup(void);
   void Show(int how = SW_SHOW);
   HWND GethWnd(void){ return m_hPopupWnd;}
};
#endif


//--- general window routines ----+
POINTS SizeForClient(HWND hWnd, WORD width, WORD height);
void CenterWindow(HWND hWnd);
