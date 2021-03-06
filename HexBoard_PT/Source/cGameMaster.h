//cGameMaster.h
//Really and extension of cAPP
//180625 : dc

#pragma once
#include "ProjDefs.h"
#include "cHexMapWnd.h"
#include "cStatusWnd.h"
#include "cArmy.h"

typedef struct tSPACE_DATA {
	cHEX_SPACE* pSpace;
	cACTOR* pActor;
	POINTXY xyCenter;
	ACTOR_INFO* pActorInfo;

	void Clear(void) {
		pSpace = NULL;  pActor = NULL;  pActorInfo = NULL;  xyCenter.Clear();
	}
	tSPACE_DATA(void) { Clear(); }
}SPACE_DATA;

static const int kMAX_ARMIES = 4;

enum eGAME_MODE {
	eInitializing,
	eSelection,
	eMoving,
	eCombat
};

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
   cHEX_MAP_WND* m_pHexMapWnd;

   cSTATUS_WND* m_pStatusWnd;

   //int m_nMaxTurns;
   //int m_nCurTurn;

   cARMY* m_apArmies[kMAX_ARMIES];
   eARMY m_currentArmy; //should be in range 0-3
   int m_nNumArmies;

   //structures for moving data
   SPACE_DATA m_MovingOriginData;
   SPACE_DATA m_MovingDestData;   

   eGAME_MODE m_currentGameMode;

   //--- private methods ----_
   //void paintArmies(HDC hdc);
   void positionMap(void);
   void setActorLocation(cACTOR* pActor, POINTCR location);

   //POINTXY calcClient();

public:
   cGAME_MASTER(HWND hParWnd);
   ~cGAME_MASTER(void);

   void OnMapWndLButtonDown(SHORT x, SHORT y);
   void OnMapWndMouseMove(SHORT x, SHORT y);

   //void EndMovement(void);  //end the movement phase

   //void ProcessCombat(void);
   //void QuerryCombat(cACTOR* pUs, cACTOR* pEnemy);

   //Called in response to WM_PAINT in parwnd
   void OnPaint(void);

   LRESULT EventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

