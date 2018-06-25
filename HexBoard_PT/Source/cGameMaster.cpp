//cGameMaster.cpp

#include "cGameMaster.h"
//#include <math.h>  //sqrrt <-- do as define
#include "MyResource.h" //dialog box

//const int kMAX_TURNS = 12;
const int kNUM_SQUARES_WIDE = 4;
const int kNUM_SQUARES_TALL = 3;
//const eHEX_TYPE kHEX_TYPE = eHexPointSide;

static cGAME_MASTER* sg_pGameMaster = NULL;

/*****************************************/
cGAME_MASTER::cGAME_MASTER(HWND hParentWnd)
{
   sg_pGameMaster = this;

   m_hParWnd = hParentWnd;
#if 0
   m_pSelectedLocation = NULL;

   for (int i = 0; i < kNUM_ARMIES; i++) {
      m_apArmies[i] = NULL;
   }

   m_currentGameMode = eInitializing;

   m_nMaxTurns = kMAX_TURNS;
   m_nCurTurn = 0;
   m_nNumArmies = 0;
#endif
   
   //Create Map Wnd 'constant' spaces wide x 'constant' spaces tall 
   //(See top of this module)
#if 0
   m_pHexMapWnd = new cHEX_MAP_WND(hParentWnd, 1933, kNUM_SQUARES_WIDE, kNUM_SQUARES_TALL);

   //POINTS clientSize = m_pHexMapWnd->GetClientSize();
   //int squareSize = m_pHexMapWnd->GetSpaceSize();

   m_pHexMapWnd->RegisterChild();

   //re-size based on hex row/col
   m_pHexMapWnd->CreateHexMapChild();
   
   m_pHexMapWnd->SizeHexForClient();

   positionMap();
#endif
#if 0
   m_pStatusWnd = new cSTATUS_WND(hParentWnd, 1934);
   m_pStatusWnd->RegisterChild();
   m_pStatusWnd->CreateChild(WS_THICKFRAME, 256, 300);

   RECT winRect;
   GetWindowRect(m_pArenaWnd->GethWnd(), &winRect);

   ScreenToClient(m_hParWnd, (POINT*)&winRect.left);
   ScreenToClient(m_hParWnd, (POINT*)&winRect.right);

   //put status window to the right of arena window
   SetWindowPos(m_pStatusWnd->GethWnd(), NULL, winRect.right, winRect.top, 0, 0, SWP_NOSIZE);
#endif

   //m_pHexMapWnd->Show();
  // m_pHexMapWnd->SetBlockReSize(TRUE);
  // m_pHexMapWnd->SetBlockMove(TRUE);

   //m_pStatusWnd->Show(); 
   //m_pStatusWnd->SetBlockResize(TRUE);
   //m_pStatusWnd->SetBlockMove(TRUE);
#if 0
   //Init the Army
   m_apArmies[0] = new cARMY(L"Northern Army");
   m_nNumArmies++;
   m_apArmies[1] = new cARMY(L"Southern Army");
   m_nNumArmies++;

   //Create the actors in army1
   cFIELD_SQUARE* pFieldSquare;
   ACTOR_INFO actorInfo;
   int nActorIndex = 0; //(in army's array of actors)

   cACTOR* pActor = new cACTOR(L"Bob the Barbarian", eArmy1, eHUMAN);

   //Set its location
   //set army 1, actor 1 @ square 1, 0
   pFieldSquare = m_pArenaWnd->GetpFieldSquare(1);
    
   pActor->SetpLocation(pFieldSquare);
   nActorIndex = m_apArmies[eArmy1]->AddActor(pActor);   

   //Set AI in square
   actorInfo.eArmy = eArmy1;
   actorInfo.index = nActorIndex; //in its army
   pFieldSquare->SetActorInfo(actorInfo);

   /// next army actors
   pActor = new cACTOR(L"Scrappy Zeke", eArmy2, eHUMAN);

   pFieldSquare = m_pArenaWnd->GetpFieldSquare((13 * kNUM_SQUARES_WIDE) + (kNUM_SQUARES_WIDE -2));
   pActor->SetpLocation(pFieldSquare);
   nActorIndex = m_apArmies[eArmy2]->AddActor(pActor);

   //set AI
   actorInfo.eArmy = eArmy2;
   actorInfo.index = nActorIndex; //in its army
   pFieldSquare->SetActorInfo(actorInfo);
   
   //done

   //Army 1 up first
   m_currentArmy = eArmy1;

   MYASSERT(m_pStatusWnd != NULL);
   m_pStatusWnd->SetArmyName(m_apArmies[m_currentArmy]->GetpName());

   m_currentGameMode = eSelection;
#endif
   return;
}

/****************************************/
cGAME_MASTER::~cGAME_MASTER(void)
{
#if 0
   for (int i = 0; i < kNUM_ARMIES; i++) {
      CLEAN_DELETE(m_apArmies[i]);
   }
   
   CLEAN_DELETE(m_pStatusWnd);
#endif

   //CLEAN_DELETE(m_pHexMapWnd);

   return;
}

/************************************************************************/
void cGAME_MASTER::positionMap(void)
{
#if 0
   RECT parentClientRect;
   GetClientRect(GetParent(m_pHexMapWnd->GethWnd()), &parentClientRect);
   RECT mapWinRect;
   GetWindowRect(m_pHexMapWnd->GethWnd(), &mapWinRect);

   int x = (parentClientRect.right - (mapWinRect.right - mapWinRect.left)) / 2;
   int y = (parentClientRect.bottom - (mapWinRect.bottom - mapWinRect.top)) / 4;
   SetWindowPos(m_pHexMapWnd->GethWnd(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
#endif
   return;
}

#if 0
//Left Button went down in arena
/**************************************************/
BOOL cGAME_MASTER::OnArenaLButtonDown(SHORT x, SHORT y)
{
   /*
   X and Y are in field client coordinates.
   We have to convert to square number.
   Then get pSquare and info. Go from there

   Return (bool) Do we need repaint?
   */
   
   //Indicat 'Need Repaint"
   BOOL bRetValue = FALSE;   

   switch (m_currentGameMode) {
   case eSelection:
   {
      //Get ptr to Square that was 'clicked'
      cFIELD_SQUARE * pSquare = m_pArenaWnd->GetpFieldSquare(x, y);
      //No non-square places on map
      MYASSERT(pSquare != NULL);

      //Get the actor info
      ACTOR_INFO AI = pSquare->GetActorInfo();

      //If not our player there, we're done
      if (AI.eArmy != m_currentArmy)
         break;

      //Get the actor and find out about him
      cACTOR* pSelectedActor = m_apArmies[AI.eArmy]->GetpActor(AI.index);

      if (pSelectedActor->GetHasMoved() == TRUE) 
         break;      

      m_pSelectedLocation = pSquare;
      pSquare->SetHilighted(TRUE);
      bRetValue = TRUE;

      //Tell Status Window we've selected an Actor
      m_pStatusWnd->SetCurrentActor(pSelectedActor);

      m_currentGameMode = eMoving;

      //Parent will invalidted rect, which will call
      //..our paint routine
      
      }
      break;

   //LButtonDown
   case eMoving:
      {
      //Last Potential is our destination.

      MYASSERT(m_pSelectedLocation != NULL);
      //This is still a problem
      MYASSERT(m_pPotentialLocation != NULL);

      //1. Get AI from current location
      ACTOR_INFO curAI;
      curAI = m_pSelectedLocation->GetActorInfo();    

      //2. Use it to get pointer to actor on the square
      cACTOR* pMovingActor = m_apArmies[curAI.eArmy]->GetpActor(curAI.index);

      //3. write the AI to the dest squre
      m_pPotentialLocation->SetActorInfo(curAI);

      //4. clear the ai and write empty ai to current square
      curAI.eArmy = eNone;
      curAI.index = 0;
      m_pSelectedLocation->SetActorInfo(curAI);
      m_pSelectedLocation->SetHilighted(FALSE);

      //5. AI done. Now store dest square as location in actor
      pMovingActor->SetpLocation(m_pPotentialLocation);
      pMovingActor->SetMoved(TRUE);

      //Set for next Actor movement
      m_pSelectedLocation->SetHilighted(FALSE);
      m_pSelectedLocation = NULL;
      m_pPotentialLocation->SetHilighted(FALSE);
      m_pPotentialLocation = NULL;

      m_pStatusWnd->SetCurrentActor(NULL);

      m_currentGameMode = eSelection;

      bRetValue = TRUE;

      }
      break;
  
   default:
      break;

   }// end 'switch current mode'

   return bRetValue;
}
#endif

#if 0
/***********************************************************************/
BOOL cGAME_MASTER::OnArenaMouseMove(SHORT x, SHORT y)
{
   /*
   X and Y are in field client coordinates.
   We have to convert to square number.
   Then get pSquare and info. Go from there

   Return (bool) Do we need repaint?
   */

   //Indicat 'Need Repaint"
   BOOL bRetValue = FALSE;

   switch (m_currentGameMode) {
   case eMoving:
      {
      //track square when we were here last. If same,
      //..then bail to save time and redraws
      static cFIELD_SQUARE* pPrevMMSquare = NULL;

      //1. Get valid square relating to current X/y
      //Get ptr to Square that is moused over
      cFIELD_SQUARE * pSquare = m_pArenaWnd->GetpFieldSquare(x, y);
      if (pSquare == m_pSelectedLocation)
         break;

      //Often, we're still in square from last MM. Bail
      if (pSquare == pPrevMMSquare)
         break;

      //If square comes back null, bail
      if (pSquare == NULL)
         break;
      
      //to spped things up next time
      pPrevMMSquare = pSquare;

      //if square occupied, bail
      ACTOR_INFO AI = pSquare->GetActorInfo();
      if (AI.eArmy != eNone)
         break;

      //2. Calc distance from current occupied square
      //Get Center of m_pSelectedLocation
      POINTS pt1 = m_pSelectedLocation->GetCenterCoord();
      //Get Center of potentialSquare
      POINTS pt2 = pSquare->GetCenterCoord();
     
      //Calc distance
      int d1 = abs(pt1.x - pt2.x);
      int d2 = abs(pt1.y - pt2.y);
      d1 = d1 * d1;
      d2 = d2 * d2;
      double MouseDistance = sqrt(d1 + d2);

      AI = m_pSelectedLocation->GetActorInfo();

      //get p to actor in selectedlocation
      cACTOR* pActor = m_apArmies[AI.eArmy]->GetpActor(AI.index);
      MYASSERT(pActor != NULL);

      //if distance > range then bail
      int allowedDistance = pActor->GetRange();
      allowedDistance *= m_pArenaWnd->GetSquareSize();

      if (MouseDistance > (double)allowedDistance)
         break;

      //-New valid potential squrae
      //3 Set as new valid & Hilite. Unhilite prev valid
      if (m_pPotentialLocation != NULL)
         m_pPotentialLocation->SetHilighted(FALSE);
      m_pPotentialLocation = pSquare;
      m_pPotentialLocation->SetHilighted(TRUE);
     
      bRetValue = TRUE;

      }
      break;

   default:
      break;

   }// end 'switch current mode'

   return bRetValue;
}
#endif

#if 0
/***************************************************************/
void cGAME_MASTER::QuerryCombat(cACTOR* pUs, cACTOR* pEnemy)
{
   int enemyStrength;
   int ourStrength;

   enemyStrength = pEnemy->GetHitPoints();
   ourStrength = pUs->GetHitPoints();

   return;
}
#endif

#if 0
/*************************************************/
void cGAME_MASTER::ProcessCombat(void)
{
   //1. For each actor in our army
   cARMY* pOurArmy = m_apArmies[m_currentArmy];

   cACTOR* pOurActor;
   cFIELD_SQUARE* pOurSquare;
   for (int i = 0; i < pOurArmy->GetNumberActors(); i++) {
      //Sweep the enemy and see if any are in combat range

      pOurActor = pOurArmy->GetpActor(i);
      pOurSquare = pOurActor->GetpLocation();
      POINTS ourCenter = pOurSquare->GetCenterCoord();

      cARMY * pCurrentEnemyArmy;
      for (int i2 = 0; i2 < m_nNumArmies; i2++) {
         //1. Get the next army
         //don't process ourselves
         if (i2 == m_currentArmy)
            continue;
         pCurrentEnemyArmy = m_apArmies[i2];
         MYASSERT(pCurrentEnemyArmy != NULL);

         //2. Check all actors that army
         cACTOR* pEnemyActor;
         cFIELD_SQUARE* pEnemySquare;
         for (int i3 = 0; i3 < pCurrentEnemyArmy->GetNumberActors(); i3++) {
            pEnemyActor = pCurrentEnemyArmy->GetpActor(i3);
            pEnemySquare = pEnemyActor->GetpLocation();

            //Get distance between us
            
            POINTS enemyLocation = pEnemySquare->GetCenterCoord();
            POINTS rc = m_pArenaWnd->PixToRowCol(enemyLocation);

            int xOffset = abs(ourCenter.x - rc.x);
            int yOffset = abs(ourCenter.y - rc.y);

            if ((xOffset == 1) || (yOffset == 1))
               QuerryCombat(pOurActor, pEnemyActor);
            
         }
      }//end for
   }//end for all our army guys

}
#endif
#if 0
/*************************************************/
void cGAME_MASTER::EndMovement(void)
{
   cARMY* pCurArmy;
   cACTOR* pCurActor;
   pCurArmy = m_apArmies[m_currentArmy];

   //Clear all the 'moved' flags
   for(int i=0; i< pCurArmy->GetNumberActors(); i++){
      pCurActor = pCurArmy->GetpActor(i);
      pCurActor->SetMoved(FALSE);
   }

   //clear the squares
   if (m_pSelectedLocation) {
      m_pSelectedLocation->SetHilighted(FALSE);
      m_pSelectedLocation = NULL;
   }
   if (m_pPotentialLocation) {
      m_pPotentialLocation->SetHilighted(FALSE);
      m_pPotentialLocation = NULL;
   }

   //Change current army to next
   int nCurrentArmy = m_currentArmy;
   nCurrentArmy++;
   nCurrentArmy = nCurrentArmy % m_nNumArmies;
   //set the enum
   m_currentArmy = (eARMY)(nCurrentArmy);

   //send data to window
   
   pCurArmy = m_apArmies[m_currentArmy];
   const wchar_t* pName = pCurArmy->GetpName();
   m_pStatusWnd->SetArmyName(pName);
   //just to be sure
   m_pStatusWnd->SetCurrentActor(NULL);
   
   //Make sure we're in selection mode
   m_currentGameMode = eSelection;

   if (m_currentArmy == (m_nNumArmies - 1))
      m_nCurTurn++;

   InvalidateRect(m_hParWnd, NULL, FALSE);

   return;
}
#endif

#if 0
/*************************************************/
void cGAME_MASTER::paintArmies(HDC hdc)
{
   HPEN hPen = CreatePen(PS_SOLID, 1, kARMY_COLORS[eArmy1]);
   SelectObject(hdc, hPen);

   m_apArmies[0]->PaintArmy(hdc);

   DeleteObject(hPen);

   hPen = CreatePen(PS_SOLID, 1, kARMY_COLORS[eArmy2]);
   SelectObject(hdc, hPen);

   m_apArmies[1]->PaintArmy(hdc);

   DeleteObject(hPen);

   return;
}
#endif

//Called by:
// ParWnd when it receives a WM_PAINT message
/*************************************************/
void cGAME_MASTER::OnPaint(void)
{
   //debugsay(L"At the OnPaint routine in cGAME_MASTER\n");

   //Players (actors) know nothing about their
   //..parent window, so we provide HDC
#
   //HDC hdc = GetDC(m_pHexMapWnd->GethWnd());

   //m_pHexMapWnd->OnPaint(hdc);

   //paintArmies(hdc);

   //ReleaseDC(m_pHexMapWnd->GethWnd(), hdc);

   return;
}

//This is called from the cAPPLICATION EventHandler
/**********************************************************************************/
LRESULT cGAME_MASTER::EventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   LRESULT lr = 0;

   switch (msg) {
#if 0
   //arena sends us message when it receives an LBUTTONDOWN
   case UM_ARENA_LBUTTON_DOWN:
      {
      //Send Pixel Coordinates!!!
      SHORT x = GET_X_LPARAM(lParam);
      SHORT y = GET_Y_LPARAM(lParam);

      /*
      Send the x/y client coordinates of the mouse to OUR OnArenaLButtonDown routine.
      If returns true, we need to repaint.
      */
      if (OnArenaLButtonDown(x, y)) {
         InvalidateRect(hWnd, NULL, FALSE);
         if (m_currentGameMode == eCombat) {
            //ProcessCombat();
            m_currentGameMode = eSelection;
         }
      }
      }
      break;

   case UM_ARENA_MOUSEMOVE:
      {
      //Send Pixel Coordinates!!!
      SHORT x = GET_X_LPARAM(lParam);
      SHORT y = GET_Y_LPARAM(lParam);

      if (OnArenaMouseMove(x, y))
         InvalidateRect(hWnd, NULL, FALSE);            
      }
      break;

   case UM_STATUS_ENDMOVEMENT:
      EndMovement();
      break;
#endif

   default:
      break;
   }

   return lr;
}
