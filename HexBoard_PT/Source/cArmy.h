//cArmy.h

#pragma once

#include "ProjDefs.h"
#include "cActor.h"

static const int kMAX_ARMY_SIZE = 5;
//----------------------------------------------------------
class cARMY {
private:
   const wchar_t* m_pName;

   //for now, just limit size of army
   int m_numActors;
   cACTOR* m_apActors[kMAX_ARMY_SIZE];

public:
   cARMY(const wchar_t* pName);
   ~cARMY(void);

   int AddActor(cACTOR* pActor);
   int GetNumberActors(void) { return m_numActors; }
   cACTOR* GetpActor(int index) { return m_apActors[index]; }
   const wchar_t* GetpName(void) { return m_pName; }

   void PaintArmy(HDC hdc);
};


