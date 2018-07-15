//cStatusWnd.h
//Present current status to the player
//180618 : (dc)

#pragma once

#include "ProjDefs.h"
#include "cParWnd.h"
#include "cActor.h"

//--------------------------------------------------------------+
class cSTATUS_WND : public cCHILD_WND {
private:
   //End current movement phase
   HWND m_hwndEndMoveButton;

   const wchar_t* m_pOutputString;

   const wchar_t* m_pCurArmyName;
   cACTOR* m_pCurrentActor;

   //TODO : dc These should probably be moved up to cChildWnd
   BOOL m_bBlockResize;
   BOOL m_bBlockMove;

   //-- private methods ---+
   void fillWC(void);  //this is called by SUPER

public:
   cSTATUS_WND(HWND hParent, int childID, const wchar_t* pCaption = NULL);
   ~cSTATUS_WND(void);

   HWND CreateChild(DWORD style, int width, int height);

   void SetArmyName(const wchar_t* pPlayerName) { m_pCurArmyName = pPlayerName; }
   void SetCurrentActor(cACTOR* pActor) { m_pCurrentActor = pActor; }

   //Block resizing of window?
   void SetBlockResize(BOOL b) { m_bBlockResize = b; }
   void SetBlockMove(BOOL b) { m_bBlockMove = b; }

   void SetOutputString(const wchar_t* pString) { m_pOutputString = pString; }

   void OnPaint(HDC hdc);
   LRESULT EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
};
