//Globdefs.h

#pragma once

#ifndef STRICT
#define STRICT
#define WIN32__LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h> //get x lparam
#endif

#include "DebugRoutines.h"

#define CLEAN_DELETE(x) if(x){ delete x; x=NULL;}
#define CLEAN_RELEASE(x) if(x){ x->Release(); x=NULL;}


#if 0

#define ALT     1
#define CONTROL 2
#define SHIFT   4

//user messages
#define UM_NEW_NAVIGATOR WM_USER+1    //sent by navigator window
#define UM_NEW_IMAGEPOS  WM_USER+2    //sent by scrolling image thumbs

//the child window id's
#define MAP_WINDOW 1933
#define WORLD_WINDOW 1934

//timer id's
#define GREET_TIMER 1925

#define _PI_  3.14159265358979323f
#define _2PI_ 6.283185307f
#define PI _PI_

//from Microsoft defines
#define PIOVER180  0.0174532925199432957692369076848861f
#define PIUNDER180  57.2957795130823208767981548141052f

#define DEG_TO_RAD(angle)((angle)*PIOVER180)
#define RAD_TO_DEG(radians) ((radians)*PIUNDER180)

#define FEET_TO_METERS(feet) ((feet)*.3048f)

#endif 

