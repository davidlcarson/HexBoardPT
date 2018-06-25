#pragma once

//gamelib version
//4/19/04

#include "Globdefs.h"

//base window class: requires some derived functions
//----------------------------------------------------------------------+
class cWND{
 protected:
   WNDCLASSEX m_wc;
   HWND m_hWnd;  
   HWND m_hParent; 
   //HDC m_hdc;
   HBRUSH m_hBGBrush;

   bool m_bRegistered;

   bool m_bBlockMove;
   bool m_bBlockSize;

   //we don't own
   const wchar_t* m_pWindowName;
   const wchar_t* m_pClassName;

   DWORD m_style;   
   int m_id;

   //private methods
   void fillWC(void);   

 public:
   cWND(void);
   virtual ~cWND(void);
   
   virtual void RegisterWndClass(void);
   //set the clear bg color
   void SetBGColor(COLORREF cr);

   bool GetRegistered(void){return m_bRegistered;}
   void AddStyle(DWORD styleMod);
   void SetStyle(DWORD style);

   //create a window with a desired client size
   POINTS SizeForClient(WORD clientWidth, WORD clientHeight);   
   virtual void SetAsChild(HWND hParent, int id = 0);

   void BlockMove(bool b){ m_bBlockMove = b;}
   void BlockSize(bool b){ m_bBlockSize = b;}
      
   virtual HWND Create(WORD width = 320, WORD height = 240);
   virtual void Show(int cmdShow = SW_SHOW);

   //pure virtual
   virtual LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) = 0;

   //inlines
   HWND GethWnd(void){ return m_hWnd;}
};

//minimum window class: Has no parent
//----------------------------------------------------------------------+
class cPLAINWND : public cWND{
  private:    

  public:
    cPLAINWND(void);

    void CenterWindow(void); //center on desktop

    void Say(const wchar_t* pText);

    virtual LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);    
};

