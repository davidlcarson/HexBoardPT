//cGameMaster.h
//Really and extension of cAPP
//180625 : dc

#pragma once
#include "ProjDefs.h"

//#include "cArenaMapWnd.h"
//#include "cStatusWnd.h"
//#include "cArmy.h"

//#include "cHexMapWnd.h"

//static const int kNUM_ARMIES = 4;

#if 0
static const COLORREF kARMY_COLORS[4] = {
RGB(255, 0, 0),
RGB(0, 255, 0),
RGB(0, 0, 255),
RGB(255, 0, 255)
};
#endif

//------------------------------------------------------+
class cGAME_MASTER {
private:
   HWND m_hParWnd;  //our parent wnd
   //cHEX_MAP_WND* m_pHexMapWnd;
   //cARENA_MAP_WND* m_pArenaWnd;
   //cSTATUS_WND* m_pStatusWnd;

   //int m_nMaxTurns;
   //int m_nCurTurn;

   //cARMY* m_apArmies[kNUM_ARMIES];
   //eARMY m_currentArmy; //should be in range 0-3
   //int m_nNumArmies;

   //Square holding Actor selected for moving
   //cFIELD_SQUARE* m_pSelectedLocation;
   //Current valid square as potential destination
   //cFIELD_SQUARE* m_pPotentialLocation;

   //GAME_MODE m_currentGameMode;

   //--- private methods ----_
   //void paintArmies(HDC hdc);
   void positionMap(void);

public:
   cGAME_MASTER(HWND hParWnd);
   ~cGAME_MASTER(void);

   //void CreateBoardWindow(HWND hParent);
   //BOOL OnArenaLButtonDown(SHORT x, SHORT y);
   //BOOL OnArenaMouseMove(SHORT x, SHORT y);
   //BOOL OnArenaLButtonUp(SHORT x, SHORT y);

   //void EndMovement(void);  //end the movement phase

   //void ProcessCombat(void);
   //void QuerryCombat(cACTOR* pUs, cACTOR* pEnemy);

   //Called in response to WM_PAINT in parwnd
   void OnPaint(void);

   LRESULT EventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

