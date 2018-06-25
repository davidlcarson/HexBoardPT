#pragma once

#include "globdefs.h"
#include <math.h> 

#include "cFilter.h" 

//--------------------------------------------------------------------------------+
typedef struct 
{ 
   double* Weights;  // Normalized weights of neighboring pixels
   int Left;         // Bounds of source pixels window
   int Right;   
}tCONTRIBUTION_TYPE;  // Contirbution information for a single pixel

//--------------------------------------------------------------------------------+
typedef struct 
{ 
   tCONTRIBUTION_TYPE* ContribRow; // Row (or column) of contribution weights 
   UINT WindowSize;              // Filter window size (of affecting source pixels) 
   UINT LineLength;              // Length of line (no. or rows / cols) 
}tLINE_CONTRIB_TYPE;               // Contribution information for an entire line (row or column)


//--------------------------------------------------------------------------------+
class c2PASS_SCALE {
 private:      
   WORD m_bpp;

   //private methods
   tLINE_CONTRIB_TYPE* AllocContributions(UINT uLineLength, UINT uWindowSize);

   void FreeContributions (tLINE_CONTRIB_TYPE* p);

   tLINE_CONTRIB_TYPE* CalcContributions(eFILTERTYPE eFilterType, UINT uLineSize,
           UINT uSrcSize, double  dScale);

   void ScaleRow(unsigned char* pSrc, 
           UINT                 uSrcWidth,
           unsigned char*       pRes, 
           UINT                 uResWidth,
           UINT                 uRow, 
           tLINE_CONTRIB_TYPE*  Contrib);

   void HorizScale(unsigned char* pSrc, 
           UINT                   uSrcWidth,
           UINT                   uSrcHeight,
           unsigned char*         pDst,
           UINT                   uResWidth,
           UINT                   uResHeight);

   void ScaleCol(unsigned char* pSrc, 
           UINT                 uSrcWidth,
           unsigned char*       pRes, 
           UINT                 uResWidth,
           UINT                 uResHeight,
           UINT                 uCol, 
           tLINE_CONTRIB_TYPE*  Contrib);

   void VertScale(unsigned char* pSrc, 
           UINT                  uSrcWidth,
           UINT                  uSrcHeight,
           unsigned char*        pDst,
           UINT                  uResWidth,
           UINT                  uResHeight);

 public:    
    c2PASS_SCALE(WORD bitsPerPixel);
    virtual ~c2PASS_SCALE(void);

    unsigned char* AllocAndScale (  
        unsigned char* pOrigImage, 
        UINT   uOrigWidth, 
        UINT   uOrigHeight, 
        UINT   uNewWidth, 
        UINT   uNewHeight);

    unsigned char* Scale (  
        unsigned char* pOrigImage, 
        UINT   uOrigWidth, 
        UINT   uOrigHeight, 
        unsigned char* pDstImage,
        UINT   uNewWidth, 
        UINT   uNewHeight);
};



