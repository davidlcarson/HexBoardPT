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
   POINTCR location = POINTCR{ 2, 3 };
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
	//set hex ai.army : actor army
	pActorInfo->index = pActor->GetIndex();
	//set hex ai.index: actor index
	//pHexSpace->SetActorInfo(ActorInfo);

	return;
}

#if 0 //doesn't really work becasue of floats. Let Map tell us what size it wants
/************************************************************************/
POINTXT cGAME_MASTER::calcClient(void)
{	
	//calc client size for Point Top Hexes
	POINTXY retPt{ 0, 0 };

	//width
	float width;

	width = kNUM_SQUARES_WIDE * (kSPACE_SIZE * SQRT3_2);
	width += kSPACE_SIZE * SQRT3;
	//width += .5f;
	retPt.x = (SHORT)width;

	//height
	float height;

	height = kNUM_SQUARES_TALL * (kSPACE_SIZE * 3);
	height += kSPACE_SIZE;
	//height += .5f
	retPt.y = (SHORT)height;

	return retPt;
}
#endif

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

//Left Button went down in arena
/**************************************************/
void cGAME_MASTER::OnMapWndLButtonDown(SHORT x, SHORT y)
{
   /*
   X and Y are in field client coordinates.
   We have to convert to square number.
   Then get pSquare and info. Go from there
   */   

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

      //Get the actor and find out about him
      cACTOR* pSelectedActor = m_apArmies[pAI->eArmy]->GetpActor(pAI->index);

      //if (pSelectedActor->GetHasMoved() == TRUE) 
      //   break;      

	  //Store pointer to space/actor that was selected for move
      m_pSelectedSpace = pSpace;
	  m_pcrSelectedCR = m_pHexMapWnd->GetCRFromXY(pSpace->GetCenterCoord());

      pSpace->SetHilighted(TRUE);
      //bRetValue = TRUE;

      //Tell Status Window we've selected an Actor
      //m_pStatusWnd->SetCurrentActor(pSelectedActor);

      m_currentGameMode = eMoving;

	  //Redraw map and entity
	  InvalidateRect(m_hParWnd, NULL, FALSE);

      }
      break;

   //LButtonDown
   case eMoving:
      {
	   m_pPotentialLocation = pSpace;
      //Last Potential is our destination.

      MYASSERT(m_pSelectedSpace != NULL);
      //This is still a problem
      MYASSERT(m_pPotentialLocation != NULL);

      //1. Get AI from current location
      ACTOR_INFO* pCurAI;
      pCurAI = m_pSelectedSpace->GetpActorInfo();    

      //2. Use it to get pointer to actor on the square
      cACTOR* pMovingActor = m_apArmies[pCurAI->eArmy]->GetpActor(pCurAI->index);

      //3. write the AI to the dest squre
      m_pPotentialLocation->SetActorInfo(pCurAI);

      //4. clear the ai and write empty ai to current square
      //curAI.eArmy = eNone;
      //curAI.index = 0;
      //m_pSelectedLocation->SetActorInfo(curAI);
	  pCurAI->Clear();
      m_pSelectedSpace->SetHilighted(FALSE);

      //5. AI done. Now store dest square as location in actor
      pMovingActor->SetpLocation(m_pPotentialLocation);
      //pMovingActor->SetMoved(TRUE);

      //Set for next Actor movement
      m_pSelectedSpace->SetHilighted(FALSE);
      m_pSelectedSpace = NULL;
      m_pPotentialLocation->SetHilighted(FALSE);
      m_pPotentialLocation = NULL;

      //m_pStatusWnd->SetCurrentActor(NULL);

	  //Set entinty location to this hex.
	  //m_pEntity->SetLocation(POINTCR{ (SHORT)nCR.col, (SHORT)nCR.row });
	  setActorLocation(pMovingActor, nCR);

	  m_currentGameMode = eMoving;

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
   /*
   X and Y are in field client coordinates.
   We have to convert to col/row

   Return (bool) Do we need repaint?
   */

   switch (m_currentGameMode) {
   case eMoving:
      {
      //track square when we were here last. If same,
      //..then bail to save time and redraws
      static cHEX_SPACE* pPrevMMSquare = NULL;

      //1. Get CR from XY
	  POINTCR destCR = m_pHexMapWnd->GetCRFromXY(x, y);

      //Get ptr to Square that is moused over
      cHEX_SPACE * pSpace = m_pHexMapWnd->GetpSpaceXY(x, y);
      if (pSpace == m_pSelectedSpace)
         break;

      //Often, we're still in square from last MM. Bail
      if (pSpace == pPrevMMSquare)
         break;

      //If square comes back null, bail
      if (pSpace == NULL)
         break;
      
      //to speed things up next time
      pPrevMMSquare = pSpace;

      //if square occupied, bail
      ACTOR_INFO* pAI = pSpace->GetpActorInfo();
      if (pAI->eArmy != eNone)
         break;

      //2. Calc distance from current occupied square
      //Get rc of m_pSelectedLocation
	  unsigned short delta = abs(destCR.col - m_pcrSelectedCR.col) + abs(destCR.row - m_pcrSelectedCR.row);
#if 0
      POINTXY pt1 = m_pSelectedLocation->GetCenterCoord();
      //Get Center of potentialSquare
      POINTXY pt2 = pSquare->GetCenterCoord();
     
      //Calc distance
      int d1 = abs(pt1.x - pt2.x);
      int d2 = abs(pt1.y - pt2.y);
      d1 = d1 * d1;
      d2 = d2 * d2;
      double MouseDistance = sqrt(d1 + d2);
#endif
      pAI = m_pSelectedSpace->GetpActorInfo();

      //get p to actor in selectedlocation
      cACTOR* pActor = m_apArmies[pAI->eArmy]->GetpActor(pAI->index);
      MYASSERT(pActor != NULL);

      //if distance > range then bail
      int allowedDistance = pActor->GetRange();
      //allowedDistance *= m_pArenaWnd->GetSquareSize();

      if (delta > allowedDistance)
         break;

      //-New valid potential squrae
      //3 Set as new valid & Hilite. Unhilite prev valid
      if (m_pPotentialLocation != NULL)
         m_pPotentialLocation->SetHilighted(FALSE);
      m_pPotentialLocation = pSpace;
      m_pPotentialLocation->SetHilighted(TRUE);

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
