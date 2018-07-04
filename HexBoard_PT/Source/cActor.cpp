//cActor.cpp
//180614  (dc)

#include "cActor.h"

const int kDefHitPoints = 5;
const int kDefRange = 10;
/*********************************************************/
cACTOR::cACTOR(const wchar_t* pActorName, eARMY army, eACTOR_TYPE type)
{
   m_army = army;  //army we belong to
   m_pActorName = pActorName;

   //Actor type. Later, we will use this to distinguis
   //..derived classes when getting pointer to array of actor
   m_actorType = type;

   m_nHitPoints = kDefHitPoints;
   m_nRange = kDefRange;
   
   m_pHexLocation = NULL;

   //Color of border is army color. Let army set

   return;
}

/*********************************************************/
cACTOR::~cACTOR(void)
{

}

//Note: The HDC needs to have the army color pen selected
//..when we receive this.
/*********************************************************/
void cACTOR::Paint(HDC hdc)
{
   //In case we're off the board.
   //..Not yet initialized? Dead?
   if (m_pHexLocation == NULL)
      return;

   POINTS center = m_pHexLocation->GetCenterCoord();
   HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);   

   SelectObject(hdc, hBrush);
   
   Rectangle(hdc, center.x - 10, center.y - 10, center.x + 10, center.y + 10);
   DeleteObject(hBrush);

   return;
}
