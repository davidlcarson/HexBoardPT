//cArmy.cpp
//The army class
//180618  (dc)

#include "cArmy.h"


/*********************************************************************/
cARMY::cARMY(const wchar_t* pName)
{
   m_pName = pName;

   for (int i = 0; i < kMAX_ARMY_SIZE; i++)
      m_apActors[i] = NULL;

   m_numActors = 0;

   return;
}

/*********************************************************************/
cARMY::~cARMY(void)
{
   for (int i = 0; i < kMAX_ARMY_SIZE; i++)
      CLEAN_DELETE(m_apActors[i]);

   return;
}

/*********************************************************************/
int cARMY::AddActor(cACTOR* pActor)
{
   if (m_numActors == kMAX_ARMY_SIZE)
      return -1;

   m_apActors[m_numActors] = pActor;
   m_numActors++;

   return m_numActors - 1;
}

/*********************************************************************/
void cARMY::PaintArmy(HDC hdc)
{
   //Correct pen color already selected in hdc
   for (int i = 0; i < m_numActors; i++){
      m_apActors[i]->Paint(hdc);
   }

   return;
}