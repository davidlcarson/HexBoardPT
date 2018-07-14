//cActor.h
//180614 (dc)

#pragma once

#include "ProjDefs.h"
#include "cHexMapWnd.h"
//#include "cEntity.h"

//Base Class Actor
//-------------------------------------------------+
class cACTOR{
protected:
   int m_nHitPoints;
   int m_nRange;
   BOOL m_bMoved; //this turn

   //we don't own
   cHEX_SPACE* m_pHexLocation;

   eARMY m_army; //army Actor belongs to.
   const wchar_t* m_pActorName;
   eACTOR_TYPE m_actorType;
   int m_index;

public:
   cACTOR(const wchar_t* pName, eARMY eArmy, int index, eACTOR_TYPE actorType);
   ~cACTOR(void);

   //flag so can't move more than once per turn
   //void SetMoved(BOOL b) { m_bMoved = b; }
   //BOOL GetHasMoved(void) { return m_bMoved; }

   void SetpLocation(cHEX_SPACE* pLocation) { m_pHexLocation = pLocation; }
   cHEX_SPACE* GetpLocation(void) const { return m_pHexLocation; }

   eARMY GetActorArmy(void) const { return m_army; }
   int GetIndex(void) const { return m_index; }
   //const wchar_t* GetpActorName(void) { return m_pActorName; }
   //int GetHitPoints(void) { return m_nHitPoints; }
   //int GetRange(void) { return m_nRange; }
   //eACTOR_TYPE GetActorType(void) { return m_actorType; }

   void Paint(HDC hdc) ;
};

