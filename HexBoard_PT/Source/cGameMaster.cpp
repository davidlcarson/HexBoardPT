//cGameMaster.cpp

#include "cGameMaster.h"
//#include "MyResource.h" //dialog box

//const eHEX_TYPE kHEX_TYPE = eHexPointSide;

static cGAME_MASTER* sg_pGameMaster = NULL;

/*****************************************/
cGAME_MASTER::cGAME_MASTER(HWND hParentWnd)
{
   sg_pGameMaster = this;

   m_hParWnd = hParentWnd;

   //Board size in spaces 
   m_pHexMapWnd = new cHEX_MAP_WND(hParentWnd, kSPACE_SIZE, kNUM_SQUARES_WIDE, kNUM_SQUARES_TALL);

   m_pHexMapWnd->RegisterChild();

   m_pHexMapWnd->CreateChild(WS_THICKFRAME, 320, 240);
   
   POINTXY client = m_pHexMapWnd->DesiredClient();
   SizeForClient(m_pHexMapWnd->GethWnd(), client.x, client.y);
   positionMap();

   m_pHexMapWnd->Show();
   m_pHexMapWnd->SetBlockMove(TRUE);
   m_pHexMapWnd->SetBlockReSize(TRUE);

#if 0
   m_pSelectedLocation = NULL;

   for (int i = 0; i < kNUM_ARMIES; i++) {
      m_apArmies[i] = NULL;
   }
#endif

   m_currentGameMode = eInitializing;

#if 0
   m_nMaxTurns = kMAX_TURNS;
   m_nCurTurn = 0;
   m_nNumArmies = 0;
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

   m_pStatusWnd->Show(); 
   m_pStatusWnd->SetBlockResize(TRUE);
   m_pStatusWnd->SetBlockMove(TRUE);
   
#endif
   //Create the 1 Entity
   //m_pEntity = new cENTITY();
   //Set its location (col, row)
   //m_pEntity->SetLocation(POINTCR{ 4, 2 });

   //init
   m_nNumArmies=0;
   m_currentArmy=eArmy1; //should be in range 0-3

   //Create an army
   cARMY* pArmy = new cARMY(L"Northern Army");
   //Create actor(s) to populate army
   //index 0
   cACTOR* pActor = new cACTOR(L"Actor 1", m_currentArmy, 0, eHUMAN);

   //arbitrary location for now
   POINTCR location(2, 3);
   setActorLocation(pActor, location);

   //Hand off ownership of pointers
   pArmy->AddActor(pActor);

   m_apArmies[m_currentArmy] = pArmy;
   m_nNumArmies++;

#if 0
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

   MYASSERT(m_pStatusWnd != NULL);
   m_pStatusWnd->SetArmyName(m_apArmies[m_currentArmy]->GetpName());
#endif

   m_currentGameMode = eSelection;

   //Invalidate Parent will set chain of paints that
   //..will paint the new MapWnd
   InvalidateRect(m_hParWnd, NULL, FALSE);
   return;
}

/****************************************/
cGAME_MASTER::~cGAME_MASTER(void)
{

   for (int i = 0; i < m_nNumArmies; i++) {
      CLEAN_DELETE(m_apArmies[i]);
   }
   
   //CLEAN_DELETE(m_pStatusWnd);


   //CLEAN_DELETE(m_pEntity);
   CLEAN_DELETE(m_pHexMapWnd);

   return;
}

/************************************************************************/
void cGAME_MASTER::setActorLocation(cACTOR* pActor, POINTCR location)
{
	//get pHex
	cHEX_SPACE* pHexSpace;
	pHexSpace = m_pHexMapWnd->GetpSpaceCR(location);

	//set pHex in actor
	pActor->SetpLocation(pHexSpace);

	//get actor info
	ACTOR_INFO* pActorInfo = pHexSpace->GetpActorInfo();
	pActorInfo->eArmy = pActor->GetActorArmy();
	pActorInfo->index = pActor->GetIndex();

	return;
}

//Centers map in Parent Window (I think)
/************************************************************************/
void cGAME_MASTER::positionMap(void)
{
   RECT parentClientRect;
   GetClientRect(GetParent(m_pHexMapWnd->GethWnd()), &parentClientRect);
   RECT mapWinRect;
   GetWindowRect(m_pHexMapWnd->GethWnd(), &mapWinRect);

   int x = (parentClientRect.right - (mapWinRect.right - mapWinRect.left)) / 2;
   int y = (parentClientRect.bottom - (mapWinRect.bottom - mapWinRect.top)) / 4;
   SetWindowPos(m_pHexMapWnd->GethWnd(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE);

   return;
}

//Left Button went down in MapWnd
/**************************************************/
void cGAME_MASTER::OnMapWndLButtonDown(SHORT x, SHORT y)
{
   //x & y are in client coordinates

   //get col/row of space clicked on
   POINTCR nCR = m_pHexMapWnd->GetCRFromXY(x, y);

   if ((nCR.col < 0 || nCR.row < 0))
	   return;

   //Get ptr to Space that was 'clicked'
   cHEX_SPACE * pSpace = m_pHexMapWnd->GetpSpaceCR(nCR);
   if (pSpace == NULL)
	   return;

   switch (m_currentGameMode) {

   case eSelection:
   {
      //Get the actor info
	  ACTOR_INFO* pAI = pSpace->GetpActorInfo();

      //If not current army there, we're done
      if (pAI->eArmy != m_currentArmy)
         break;

      //Our army: Get the actor and find out about him
      cACTOR* pSelectedActor = m_apArmies[pAI->eArmy]->GetpActor(pAI->index);

      //if (pSelectedActor->GetHasMoved() == TRUE) 
      //   break;      

	  //Fill data for moving space/actor
	  m_MovingOriginData.pActor = pSelectedActor;	  
      m_MovingOriginData.pSpace = pSpace;
	  m_MovingOriginData.pActorInfo = pAI;
	  m_MovingOriginData.crLocation = m_pHexMapWnd->GetCRFromXY(pSpace->GetCenterCoord());

      pSpace->SetHilighted(TRUE);

      //Tell Status Window we've selected an Actor
      //m_pStatusWnd->SetCurrentActor(pSelectedActor);

      m_currentGameMode = eMoving;

	  //Redraw map and army
	  //TODO: Support re-drawing just one space?
	  InvalidateRect(m_hParWnd, NULL, FALSE);

      }
      break;

   //LButtonDown
   case eMoving:
      {
	   //Mouse clicked during move phase..
      //Last valid potential is our destination.

      MYASSERT(m_MovingDestData.pSpace != NULL);

	  //Move actor to last valid space we moused over
	  m_MovingOriginData.pActor->SetpLocation(m_MovingDestData.pSpace);
	  //pMovingActor->SetMoved(TRUE);

	  //Set the AI for the new space...
	  *m_MovingDestData.pActorInfo = *m_MovingOriginData.pActorInfo;

	  //Clear the AI for the origin space
	  m_MovingOriginData.pActorInfo->Clear();

	  //Clear the hilites
	  m_MovingOriginData.pSpace->SetHilighted(FALSE);
	  m_MovingDestData.pSpace->SetHilighted(FALSE);

      //Clear data;
	  m_MovingOriginData.Clear();
	  m_MovingDestData.Clear();
     
	  m_currentGameMode = eSelection;

	  //Redraw map and entity
	  InvalidateRect(m_hParWnd, NULL, FALSE);

      }
      break;
  
   default:
      break;

   }// end 'switch current mode'

   return;
}


/***********************************************************************/
void cGAME_MASTER::OnMapWndMouseMove(SHORT x, SHORT y)
{
	   //X and Y are in field client coordinates.

   switch (m_currentGameMode) {
   case eMoving:
      {
	  //Get ptr to Square that is moused over
	  cHEX_SPACE * pSpace = m_pHexMapWnd->GetpSpaceXY(x, y);

	  if (pSpace == NULL)
		  break;

      //A static HexSpace that holds last space we moused over.
	  //..If it's this space, then we can bail
      static cHEX_SPACE* pPrevMMSquare = NULL;

	  //If we're still in space from last MM. Bail
	  if (pSpace == pPrevMMSquare)
		  break;

	  //Don't allow selection of origin
	  if (pSpace ==  m_MovingOriginData.pSpace)
		  break;

	  //Passed first tests...

	  //Get data from AI	  
	  ACTOR_INFO* pAI = pSpace->GetpActorInfo();

	  //if square occupied, bail
	  if (pAI->eArmy != eNone)
		  break;

	  //Get distance from origin space

      //1. Get CR from XY
	  POINTCR destCR = m_pHexMapWnd->GetCRFromXY(x, y);

      //2. Calc distance from current occupied square
      //Get rc of m_pSelectedLocation
	  unsigned short delta = abs(destCR.col - m_MovingOriginData.crLocation.col) + abs(destCR.row - m_MovingOriginData.crLocation.row);

      //Get Actor Range
      int allowedDistance = m_MovingOriginData.pActor->GetRange();

      if (delta > allowedDistance)
         break;

	  //-- This is a valid potential destination

      //-New valid potential squrae
      //Set as new valid & Hilite. Unhilite prev valid
      if (m_MovingDestData.pSpace != NULL)
         m_MovingDestData.pSpace->SetHilighted(FALSE);

	  //Fill the data structure
      m_MovingDestData.pSpace = pSpace;
	  m_MovingDestData.pSpace->SetHilighted(TRUE);
	  m_MovingDestData.pActorInfo = pAI;

	  //to speed things up next time
	  pPrevMMSquare = pSpace;

	  //Redraw map and entity
	  InvalidateRect(m_hParWnd, NULL, FALSE);

      }
      break;

   default:
      break;

   }// end 'switch current mode'

   return;
}

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
      POINTXY ourCenter = pOurSquare->GetCenterCoord();

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
            
            POINTXY enemyLocation = pEnemySquare->GetCenterCoord();
            POINTCR rc = m_pArenaWnd->PixToRowCol(enemyLocation);

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

   HDC hdc = GetDC(m_pHexMapWnd->GethWnd());

   //Paint the board
   m_pHexMapWnd->OnPaint(hdc);

   //paintArmies(hdc);
   for (int i = 0; i < m_nNumArmies; i++) {
	   m_apArmies[i]->PaintArmy(hdc);
   }

   ReleaseDC(m_pHexMapWnd->GethWnd(), hdc);

   return;
}

//This is called from the cAPPLICATION EventHandler to handle user messages
/**********************************************************************************/
LRESULT cGAME_MASTER::EventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   LRESULT lr = 0;

   switch (msg) {
   case WM_PAINT:
	   PAINTSTRUCT ps;
	   BeginPaint(hWnd, &ps);
	   EndPaint(hWnd, &ps);

	   break;

#if 0
   //arena sends us message when it receives an LBUTTONDOWN
   case UM_MAPWND_LBUTTON_DOWN:
      {
      //Send Pixel Coordinates!!!
      SHORT x = GET_X_LPARAM(lParam);
      SHORT y = GET_Y_LPARAM(lParam);

	  OnMapWndLButtonDown(x, y);

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
#endif

#if 0
   case UM_MAPWND_MOUSEMOVE:
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
