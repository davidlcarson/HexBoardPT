//Utilities_2013_1.h
//01/30/2013

#pragma once

#ifndef STRICT
#define STRICT	                   
#define _WIN32_WINNT 0x0501
#include <windows.h>
#endif

#define _PI_  3.14159265358979323f
#define _2PI_ 6.283185307f
#define PI _PI_

//#define BYTE unsigned char
//#define U8 unsigned char
//#define U16 unsigned short
//#define UINT unsigned int

//from Microsoft defines
#define PIOVER180  0.0174532925199432957692369076848861f
#define PIUNDER180  57.2957795130823208767981548141052f

#define DEG_TO_RAD(angle)((angle)*PIOVER180)
#define RAD_TO_DEG(radians) ((radians)*PIUNDER180)

#define FEET_TO_METERS(feet) ((feet)*.3048f)

#define CLEAN_DELETE(x) if(x){ delete x; x=NULL;}
#define CLEAN_RELEASE(x) if(x){ x->Release(); x=NULL;}


