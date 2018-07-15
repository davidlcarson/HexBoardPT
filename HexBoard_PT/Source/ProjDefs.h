//ProjDefs.h
//Project-wide definitions
//For Melee
//180616 (dc)

#pragma once

#include "Globdefs.h"

typedef struct tPOINTCR{
	SHORT col;
	SHORT row;
	void Clear(void) { col = row = 0; }
	void Set(SHORT x, SHORT y) { col = x; row = y; }
	tPOINTCR(SHORT x = 0, SHORT y = 0) { Set(x, y); }
}POINTCR;

typedef struct tPOINTXY {
	SHORT x;
	SHORT y;
}POINTXY;

enum eARMY {
   eNone = -1,
   eArmy1,
   eArmy2,
   eArmy3,
   eArmy4
};

enum eACTOR_TYPE {
   eUNKNOWN = -1,
   eHUMAN,
   eANIMAL,
   eFANTASY
};

enum eENTITY_TYPE {
	eEntity_Unknown = -1,
	eProp,
	eSetpiece,
	eActor
};

//actor info structure -
//..one in each gameboard space 
//This is so cSPACE doesn't have to know about pActor
struct ACTOR_INFO {
   //army number (0-3)
   eARMY eArmy;
   //index into army's array of actors
   SHORT index;

   ACTOR_INFO(void) { Clear(); }
   void Clear(void) { eArmy = eNone; index = 0; }
};

//Note: Have verified there are no WM_USER messages
//..in the GenLib project

enum eUSER_MESSSAGES {
   UM_MAPWND_LBUTTON_DOWN = WM_USER + 1,  //(1025)
   UM_MAPWND_MOUSEMOVE,
   UM_STATUS_ENDMOVEMENT,  
};

// These will come from .ini and LevelData
const int kNUM_SQUARES_WIDE = 7;
const int kNUM_SQUARES_TALL = 14;
const int kSPACE_SIZE = 14;


#define SQRT3   1.73205080757f
#define SQRT3_2 3.46410161514f
