//cApp.h
//Application class for Melee
//180613 (dc)

#pragma once

#include "cParWnd.h"
#include "cGameMaster.h"


//--------------------------------------------------------------+
class cAPP {
private:
	cPARWND* m_pParWnd;	
	cGAME_MASTER* m_pGameMaster;
	
   BOOL m_bWantWindowed;   

public:
	cAPP(void);
	~cAPP(void);

	void InitGame(void);
   //Called from a loop
   void CreateParentWindow(void);

   void FrameMgr(LPARAM lParam);

   //Handles messages from the Parent Window
   LRESULT EventHandler(HWND, UINT, WPARAM, LPARAM);
};

