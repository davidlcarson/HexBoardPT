//c2PASS_SCALE.cpp

//My Version created 7/4/03
//includes routines for 24bit and 8bit buffers!!!

//have to include cFilter.h and cFilter.cpp in project for
//this module to work...

#include "c2PassScale.h"
#include "cImageBuffer.h"  //bgra 

/************************************************************************************/
c2PASS_SCALE::c2PASS_SCALE(WORD bitsPerPixel)
{
   //store bpp of source image
   m_bpp = bitsPerPixel;

   return;
}        

/************************************************************************************/
c2PASS_SCALE::~c2PASS_SCALE(void)
{
   return;
}

/************************************************************************************/
tLINE_CONTRIB_TYPE* c2PASS_SCALE::AllocContributions (UINT uLineLength, UINT uWindowSize)
{
    tLINE_CONTRIB_TYPE* res = new tLINE_CONTRIB_TYPE; 

    // Init structure header 
    res->WindowSize = uWindowSize; 
    res->LineLength = uLineLength; 

    // Allocate list of contributions 
    res->ContribRow = new tCONTRIBUTION_TYPE[uLineLength];

    for (UINT u = 0 ; u < uLineLength ; u++){    
        // Allocate contributions for every pixel
        res->ContribRow[u].Weights = new double[uWindowSize];
    }

    return res; 
} 

/************************************************************************************/
void c2PASS_SCALE::FreeContributions (tLINE_CONTRIB_TYPE* p)
{ 
    for(UINT u = 0; u < p->LineLength; u++){    
        // Free contribs for every pixel
        delete [] p->ContribRow[u].Weights;
    }
    delete [] p->ContribRow;    // Free list of pixels contribs
    delete p;                   // Free contribs header

    return;
} 

/************************************************************************************/
tLINE_CONTRIB_TYPE* c2PASS_SCALE::CalcContributions(eFILTERTYPE eFilterType,
   UINT uLineSize, UINT uSrcSize, double dScale)
{ 
   cGENERIC_FILTER* pCurFilter = NULL;

    switch(eFilterType){
       case eBOXFILTER_3:
          pCurFilter = new cBOX_FILTER_3();
          break;
       case eBILINEAR_3:
          pCurFilter = new cBILINEAR_FILTER_3();
          break;
       case eGAUSSIAN_3:
          pCurFilter = new cGAUSSIAN_FILTER_3();
          break;
       case eHAMMING_3:
          pCurFilter = new cHAMMING_FILTER_3();
          break;
       case eBLACKMAN_3:
          pCurFilter = new cBLACKMAN_FILTER_3();
          break;
    }

    //FilterClass CurFilter;
    double dWidth;
    double dFScale = 1.0;
    double dFilterWidth = pCurFilter->GetWidth();

    if (dScale < 1.0) 
    {    // Minification
        dWidth = dFilterWidth / dScale; 
        dFScale = dScale; 
    } 
    else
    {    // Magnification
        dWidth= dFilterWidth; 
    }
 
    // Window size is the number of sampled pixels
    int iWindowSize = 2 * (int)ceil(dWidth) + 1; 
 
    // Allocate a new line contributions strucutre
    tLINE_CONTRIB_TYPE *res = AllocContributions (uLineSize, iWindowSize); 
 
    for (UINT u = 0; u < uLineSize; u++) 
    {   // Scan through line of contributions
        double dCenter = (double)u / dScale;   // Reverse mapping

        // Find the significant edge points that affect the pixel

        //corrected version
        int iLeft = max (0, (int)floor (dCenter - dWidth)); 
        int iRight = min ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1); 

        // Cut edge points to fit in filter window in case of spill-off
        if(iRight - iLeft + 1 > iWindowSize){
        
           if(iLeft < (int(uSrcSize) - 1 / 2))            
              iLeft++;            
           else            
              iRight--;            
        }

        res->ContribRow[u].Left = iLeft; 
        res->ContribRow[u].Right = iRight;
        //end corrected

        double dTotalWeight = 0.0;  // Zero sum of weights
        int iSrc;
        for (iSrc = iLeft; iSrc <= iRight; iSrc++)
        {   // Calculate weights
            dTotalWeight += (res->ContribRow[u].Weights[iSrc-iLeft] =  
                                dFScale * pCurFilter->Filter (dFScale * (dCenter - (double)iSrc))); 
        }

        if (dTotalWeight > 0.0)
        {   // Normalize weight of neighbouring points
            for (iSrc = iLeft; iSrc <= iRight; iSrc++)
            {   // Normalize point
                res->ContribRow[u].Weights[iSrc-iLeft] /= dTotalWeight; 
            }
        }
   } 

   if(pCurFilter)
      delete pCurFilter;

   return res; 
} 
 
/************************************************************************************/
void c2PASS_SCALE::ScaleRow(unsigned char* pSrc, UINT uSrcWidth, unsigned char* pRes, 
           UINT uResWidth, UINT uRow, tLINE_CONTRIB_TYPE *Contrib)
{
   //convert one row of pixels from source to result
   
   WORD paddedWide;
   unsigned char* pFirstByte;
   
   switch(m_bpp){
      case 32:
         {

         //calculate the address of the first pixel in this (source) row                  

         //get width of source buffer in bytes         
         paddedWide = uSrcWidth * sizeof(dcBGRA);
         while(paddedWide % 4)
            paddedWide++;

         //address of first byte:
         pFirstByte = (unsigned char*)pSrc;
         pFirstByte += paddedWide * uRow;

         //point first pixel
         dcBGRA* pCurSrcPixel;
         pCurSrcPixel = (dcBGRA*)pFirstByte;

         //calculate the address of the first pixel in this (dest) row
         //re-use paddedWide and pFirstByte
         paddedWide = uResWidth * sizeof(dcBGRA);
         while(paddedWide % 4)
            paddedWide++;
         
         //address of first byte
         pFirstByte = (unsigned char*)pRes;
         pFirstByte += paddedWide * uRow;

         //point to first dest pixel
         dcBGRA* pCurDstPixel;
         pCurDstPixel = (dcBGRA*)pFirstByte;                       

         //for each pixel in the result row....
         for(UINT curRowPixel = 0; curRowPixel<uResWidth; curRowPixel++) {

            dcBGRA curDstPixel;
            ZeroMemory(&curDstPixel, sizeof(dcBGRA));
                        
            //retrieve boundries
            int iLeft = Contrib->ContribRow[curRowPixel].Left;    
            int iRight = Contrib->ContribRow[curRowPixel].Right;  

            //for each weight for this dest pixel
            for (int i = iLeft; i <= iRight; i++){
               // Scan between boundries
               // Accumulate weighted effect of each neighboring pixel

               //get weight for this neighbor
               double weight = Contrib->ContribRow[curRowPixel].Weights[i-iLeft];               

               //add weighted value of this neighbor to current dst pixel
               curDstPixel.red += (unsigned char)(weight * (double) pCurSrcPixel[i].red);
               curDstPixel.green += (unsigned char)(weight * (double) pCurSrcPixel[i].green);
               curDstPixel.blue += (unsigned char)(weight * (double) pCurSrcPixel[i].blue);
               curDstPixel.alpha += (unsigned char)(weight * (double) pCurSrcPixel[i].alpha);

            } //all weights added

            //store dst pixel
            pCurDstPixel[curRowPixel] = curDstPixel;
         }
      }
      break;

      case 24:
         {
         //calculate the address of the first pixel in this (source) row                  

         //get width of source buffer in bytes
         paddedWide = uSrcWidth * sizeof(dcBGR);
         while(paddedWide % 4)
            paddedWide++;

         //address of first byte:
         pFirstByte = (unsigned char*)pSrc;
         pFirstByte += paddedWide * uRow;

         //point first pixel
         dcBGR* pCurSrcPixel;
         pCurSrcPixel = (dcBGR*)pFirstByte;

         //calculate the address of the first pixel in this (dest) row
         //re-use paddedWide and pFirstByte
         paddedWide = uResWidth * sizeof(dcBGR);
         while(paddedWide % 4)
            paddedWide++;
         
         //address of first byte
         pFirstByte = (unsigned char*)pRes;
         pFirstByte += paddedWide * uRow;

         //point to first dest pixel
         dcBGR* pCurDstPixel;
         pCurDstPixel = (dcBGR*)pFirstByte;                       

         //for each pixel in the result row....
         for(UINT curRowPixel = 0; curRowPixel<uResWidth; curRowPixel++) {

            dcBGR curDstPixel;
            ZeroMemory(&curDstPixel, sizeof(dcBGR));
                        
            //retrieve boundries
            int iLeft = Contrib->ContribRow[curRowPixel].Left;    
            int iRight = Contrib->ContribRow[curRowPixel].Right;  

            //for each weight for this dest pixel
            for (int i = iLeft; i <= iRight; i++){
               // Scan between boundries
               // Accumulate weighted effect of each neighboring pixel

               //get weight for this neighbor
               double weight = Contrib->ContribRow[curRowPixel].Weights[i-iLeft];

               //add weighted value of this neighbor to current dst pixel
               curDstPixel.red += (unsigned char)(weight * (double) pCurSrcPixel[i].red);
               curDstPixel.green += (unsigned char)(weight * (double) pCurSrcPixel[i].green);
               curDstPixel.blue += (unsigned char)(weight * (double) pCurSrcPixel[i].blue);

            } //all weights added

            //store dst pixel
            pCurDstPixel[curRowPixel] = curDstPixel;
         }
      }
      break;

      case 8:
         {
         //calculate the address of the first pixel in this (source) row                  

         //get width of source buffer in bytes
         paddedWide = uSrcWidth;
         while(paddedWide % 4)
            paddedWide++;

         //address of first byte:
         pFirstByte = (unsigned char*)pSrc;
         pFirstByte += paddedWide * uRow;

         //point first pixel
         unsigned char* pCurSrcPixel;
         pCurSrcPixel = (unsigned char*)pFirstByte;

         //calculate the address of the first pixel in this (dest) row
         //re-use paddedWide and pFirstByte
         paddedWide = uResWidth;
         while(paddedWide % 4)
            paddedWide++;
         
         //address of first byte
         pFirstByte = (unsigned char*)pRes;
         pFirstByte += paddedWide * uRow;

         //point to first dest pixel
         unsigned char* pCurDstPixel;
         pCurDstPixel = (unsigned char*)pFirstByte;                       

         //for each pixel in the result row....
         for(UINT curRowPixel = 0; curRowPixel<uResWidth; curRowPixel++) {

            unsigned char curDstPixel;
            //ZeroMemory(&curDstPixel, sizeof(char));
            curDstPixel = 0;
                        
            //retrieve boundries
            int iLeft = Contrib->ContribRow[curRowPixel].Left;    
            int iRight = Contrib->ContribRow[curRowPixel].Right;  

            //for each weight for this dest pixel
            for (int i = iLeft; i <= iRight; i++){                              

               //get weight for this neighbor
               double weight = Contrib->ContribRow[curRowPixel].Weights[i-iLeft];

               //add weighted value of this neighbor to current dst pixel
               curDstPixel += (unsigned char)(weight * (double) pCurSrcPixel[i]);               

            } //all weights added

            //store dst pixel
            pCurDstPixel[curRowPixel] = curDstPixel;
         }
      }
      break;

   }//end of switch           
     
   return;
} 

/************************************************************************************/
void c2PASS_SCALE::HorizScale(unsigned char* pSrc, UINT uSrcWidth, UINT uSrcHeight, 
                      unsigned char* pDst, UINT uResWidth, UINT uResHeight)
{ 
    
    //if result width is source width, we just copy
    if(uResWidth == uSrcWidth){
       unsigned char* pSrcWalker;
       unsigned char* pDstWalker;

       pSrcWalker = (unsigned char*)pSrc;
       pDstWalker = (unsigned char*)pDst;

       WORD PaddedWide;       
       WORD tall;

       switch(m_bpp){
          case 32:
             PaddedWide = uSrcWidth * sizeof(dcBGRA);
             while(PaddedWide % 4)
                PaddedWide++;

             // No scaling required, just copy
             for(tall=0; tall<uSrcHeight; tall++){
                //copy one line
                memcpy(pDstWalker, pSrcWalker, sizeof(dcBGRA) *  uSrcWidth);
                pDstWalker += PaddedWide;
                pSrcWalker += PaddedWide;
             }                
             break;

          case 24:
             PaddedWide = (WORD)(uSrcWidth * 3L);
             while(PaddedWide % 4)
                PaddedWide++;

             // No scaling required, just copy
             for(tall=0; tall<uSrcHeight; tall++){
                //copy one line
                memcpy(pDstWalker, pSrcWalker, 3L *  uSrcWidth);
                pDstWalker += PaddedWide;
                pSrcWalker += PaddedWide;
             }                
             break;

          case 8:
             PaddedWide = uSrcWidth;
             while(PaddedWide % 4)
                PaddedWide++;

             // No scaling required, just copy
             for(tall=0; tall<uSrcHeight; tall++){
                //copy one line
                memcpy(pDstWalker, pSrcWalker, uSrcWidth);
                pDstWalker += PaddedWide;
                pSrcWalker += PaddedWide;
             }                
             break;
       }

    }
    // Allocate and calculate the contributions
    tLINE_CONTRIB_TYPE * Contrib = CalcContributions (eBILINEAR_3, uResWidth, uSrcWidth, 
                        double(uResWidth) / double(uSrcWidth)); 

    for (UINT u = 0; u < uResHeight; u++){
        //Scale each row
        ScaleRow(pSrc, uSrcWidth, pDst, uResWidth, u, Contrib);   
    }

    //Free contributions structure
    FreeContributions (Contrib);  

    return;
} 

/************************************************************************************/
void c2PASS_SCALE::ScaleCol(unsigned char* pSrc, UINT uSrcWidth, unsigned char* pRes, 
          UINT uResWidth, UINT uResHeight, UINT uCol, tLINE_CONTRIB_TYPE* Contrib)
{
   
   unsigned char* pFirstDstPixel;

   WORD PaddedSrcWide;
   WORD PaddedDstWide;

   switch(m_bpp){
      case 32:
         {
         dcBGRA* pCurSrcPixel = NULL;
         dcBGRA* pCurDstPixel = NULL;

         //calculate the address of the first pixel in the Source column
         //calc padded wide for source (we'll need for bumping)
         PaddedSrcWide = uSrcWidth * sizeof(dcBGRA);
         while(PaddedSrcWide % 4)
            PaddedSrcWide++;

         //calculate the address of the first pixel in the dest column
         //calc padded wide for dest (also needed for bumping)
         PaddedDstWide = uResWidth * sizeof(dcBGRA);
         while(PaddedDstWide %4)
            PaddedDstWide++;

         pFirstDstPixel = (unsigned char*)pRes;
         pFirstDstPixel += uCol * sizeof(dcBGRA);         

         //pixel for accumulating values
         dcBGRA curDstPixel;

         //for each pixel in this column
         for (UINT curColPixel=0; curColPixel<uResHeight; curColPixel++){

            ZeroMemory(&curDstPixel, sizeof(dcBGRA));
    
            int iLeft = Contrib->ContribRow[curColPixel].Left;    // Retrieve left boundries
            int iRight = Contrib->ContribRow[curColPixel].Right;  // Retrieve right boundries

            //add weights 
            for (int i = iLeft; i <= iRight; i++){

               pCurDstPixel = (dcBGRA*)pFirstDstPixel;

               unsigned char* pWeightPixel;
               pWeightPixel = (unsigned char*)pSrc;
               pWeightPixel += i * PaddedSrcWide;
               pWeightPixel += uCol * sizeof(dcBGRA);

               pCurSrcPixel = (dcBGRA*)pWeightPixel;
        
               double weight;
               weight = Contrib->ContribRow[curColPixel].Weights[i-iLeft];                              

               curDstPixel.red += (unsigned char)(weight * pCurSrcPixel->red);
               curDstPixel.green += (unsigned char)(weight * pCurSrcPixel->green);
               curDstPixel.blue += (unsigned char)(weight * pCurSrcPixel->blue);
               curDstPixel.alpha += (unsigned char)(weight * pCurSrcPixel->alpha);
            }
            //store the dest pixel
            *pCurDstPixel = curDstPixel;

            //bump the addresses
            pFirstDstPixel += PaddedDstWide;            
         }
         }
         break;

      case 24:
         {
         dcBGR* pCurSrcPixel = NULL;
         dcBGR* pCurDstPixel = NULL;

         //calculate the address of the first pixel in the Source column
         //calc padded wide for source (we'll need for bumping)
         PaddedSrcWide = uSrcWidth * sizeof(dcBGR);
         while(PaddedSrcWide % 4)
            PaddedSrcWide++;

         //calculate the address of the first pixel in the dest column
         //calc padded wide for dest (also needed for bumping)
         PaddedDstWide = uResWidth * sizeof(dcBGR);
         while(PaddedDstWide %4)
            PaddedDstWide++;

         pFirstDstPixel = (unsigned char*)pRes;
         pFirstDstPixel += uCol * sizeof(dcBGR);         

         //pixel for accumulating values
         dcBGR curDstPixel;

         //for each pixel in this column
         for (UINT curColPixel=0; curColPixel<uResHeight; curColPixel++){

            ZeroMemory(&curDstPixel, sizeof(dcBGR));

            int iLeft = Contrib->ContribRow[curColPixel].Left;    // Retrieve left boundries
            int iRight = Contrib->ContribRow[curColPixel].Right;  // Retrieve right boundries

            //add weights 
            for (int i = iLeft; i <= iRight; i++){

               pCurDstPixel = (dcBGR*)pFirstDstPixel;

               unsigned char* pWeightPixel;
               pWeightPixel = (unsigned char*)pSrc;
               pWeightPixel += i * PaddedSrcWide;
               pWeightPixel += uCol * sizeof(dcBGR);

               pCurSrcPixel = (dcBGR*)pWeightPixel;
        
               double weight;
               weight = Contrib->ContribRow[curColPixel].Weights[i-iLeft];                              

               curDstPixel.red += (unsigned char)(weight * pCurSrcPixel->red);
               curDstPixel.green += (unsigned char)(weight * pCurSrcPixel->green);
               curDstPixel.blue += (unsigned char)(weight * pCurSrcPixel->blue);               
            }
            //store the dest pixel
            *pCurDstPixel = curDstPixel;

            //bump the addresses
            pFirstDstPixel += PaddedDstWide;            
         }
         }
         break;

      case 8:
         {
         unsigned char* pCurSrcPixel = NULL;
         unsigned char* pCurDstPixel = NULL;

         //calculate the address of the first pixel in the Source column
         //calc padded wide for source (we'll need for bumping)
         PaddedSrcWide = uSrcWidth;
         while(PaddedSrcWide % 4)
            PaddedSrcWide++;

         //calculate the address of the first pixel in the dest column
         //calc padded wide for dest (also needed for bumping)
         PaddedDstWide = uResWidth;
         while(PaddedDstWide %4)
            PaddedDstWide++;

         pFirstDstPixel = (unsigned char*)pRes;
         pFirstDstPixel += uCol;         

         //pixel for accumulating values
         unsigned char curDstPixel;

         //for each pixel in this column
         for (UINT curColPixel=0; curColPixel<uResHeight; curColPixel++){

            //ZeroMemory(&curDstPixel, sizeof(char));
            curDstPixel = 0;
    
            int iLeft = Contrib->ContribRow[curColPixel].Left;    // Retrieve left boundries
            int iRight = Contrib->ContribRow[curColPixel].Right;  // Retrieve right boundries

            //add weights 
            for (int i = iLeft; i <= iRight; i++){

               pCurDstPixel = (unsigned char*)pFirstDstPixel;

               unsigned char* pWeightPixel;
               pWeightPixel = (unsigned char*)pSrc;
               pWeightPixel += i * PaddedSrcWide;
               pWeightPixel += uCol;

               pCurSrcPixel = (unsigned char*)pWeightPixel;
        
               // Scan between boundries
               // Accumulate weighted effect of each neighboring pixel

               double weight;
               weight = Contrib->ContribRow[curColPixel].Weights[i-iLeft];                              

               curDstPixel += (unsigned char)(weight * *pCurSrcPixel);
            }

            //store the dest pixel            
            *pCurDstPixel = curDstPixel;

            //bump the addresses
            pFirstDstPixel += PaddedDstWide;            

         }//end for each pixel in column

         }//end case
         break;

     }//end switch

     return;
} 
 
/************************************************************************************/
void c2PASS_SCALE::VertScale(unsigned char* pSrc, UINT uSrcWidth, UINT uSrcHeight,
            unsigned char* pDst, UINT uResWidth, UINT uResHeight)
{ 
   
    //if result height is source height, we just copy
    if(uResHeight == uSrcHeight){
       unsigned char* pSrcWalker;
       unsigned char* pDstWalker;

       pSrcWalker = (unsigned char*)pSrc;
       pDstWalker = (unsigned char*)pDst;

       WORD PaddedWide;       
       WORD tall;

       switch(m_bpp){
          case 32:
             PaddedWide = uSrcWidth * sizeof(dcBGRA);
             while(PaddedWide % 4)
                PaddedWide++;

             // No scaling required, just copy
             for(tall=0; tall<uSrcHeight; tall++){
                //copy one line
                memcpy(pDstWalker, pSrcWalker, sizeof(dcBGRA) *  uSrcWidth);
                pDstWalker += PaddedWide;
                pSrcWalker += PaddedWide;
             }                
             break;

          case 24:
             PaddedWide = (WORD)(uSrcWidth * 3L);
             while(PaddedWide % 4)
                PaddedWide++;

             // No scaling required, just copy
             for(tall=0; tall<uSrcHeight; tall++){
                //copy one line
                memcpy(pDstWalker, pSrcWalker, 3L *  uSrcWidth);
                pDstWalker += PaddedWide;
                pSrcWalker += PaddedWide;
             }                
             break;

          case 8:
             PaddedWide = uSrcWidth;
             while(PaddedWide % 4)
                PaddedWide++;

             // No scaling required, just copy
             for(tall=0; tall<uSrcHeight; tall++){
                //copy one line
                memcpy(pDstWalker, pSrcWalker, uSrcWidth);
                pDstWalker += PaddedWide;
                pSrcWalker += PaddedWide;
             }                
             break;
       }

    }

   // Allocate and calculate the contributions
   tLINE_CONTRIB_TYPE * Contrib = CalcContributions(eBILINEAR_3, uResHeight,
               uSrcHeight, double(uResHeight) / double(uSrcHeight)); 

   for (UINT u = 0; u < uResWidth; u++){
      // Scale each column
      ScaleCol(pSrc, uSrcWidth, pDst, uResWidth, uResHeight, u, Contrib);
   }
            
   FreeContributions(Contrib); // Free contributions structure

   return;
} 

/************************************************************************************/
unsigned char* c2PASS_SCALE::AllocAndScale(unsigned char* pOrigImage, UINT uOrigWidth, 
    UINT uOrigHeight, UINT  uNewWidth, UINT uNewHeight)
{ 
   
   unsigned char* pRes = NULL;   //will be new buffer
   WORD NewPaddedWide;
   unsigned char* pTemp = NULL;  //mid resize buffer

   switch(m_bpp){
      case 32:  
         NewPaddedWide = uNewWidth * sizeof(dcBGRA);
         break;

      case 24:
         NewPaddedWide = uNewWidth * sizeof(dcBGR);
         break;

      case 8:
         NewPaddedWide = uNewWidth;
         break;
   }//end switch

   while(NewPaddedWide % 4)
      NewPaddedWide++;                    
   
   MYASSERT(pTemp == NULL);
   pTemp = new unsigned char[NewPaddedWide * uOrigHeight];

   HorizScale (pOrigImage, 
          uOrigWidth,
          uOrigHeight,
          pTemp,
          uNewWidth,
          uOrigHeight);

   //get buffer for new image
   MYASSERT(pRes == NULL);
   pRes = new unsigned char[NewPaddedWide * uNewHeight];

   // Scale temporary image vertically into result image    
   VertScale ( pTemp,
          uNewWidth,
          uOrigHeight,
          pRes,
          uNewWidth,
          uNewHeight);

   delete [] pTemp;

   return pRes;
} 

/************************************************************************************/
unsigned char* c2PASS_SCALE::Scale(unsigned char* pOrigImage, UINT uOrigWidth, UINT uOrigHeight,
                 unsigned char* pDstImage, UINT uNewWidth, UINT uNewHeight)
{     
    //get memory for temporary buffer new width, orig height    
    
	unsigned char* pTemp = NULL;;
    WORD paddedWide;

    paddedWide = uNewWidth * (m_bpp / 8);
    while(paddedWide % 4)
       paddedWide++;

    //COLORREF *pTemp = new COLORREF [uNewWidth * uOrigHeight];
	MYASSERT(pTemp = NULL);
    pTemp = new unsigned char[paddedWide * uOrigHeight];
    
    //call the horiz scale routine
    HorizScale (pOrigImage,  //orig image is source
                uOrigWidth,
                uOrigHeight,
                pTemp,       //temp image is dest
                uNewWidth,
                uOrigHeight);
        

    //call the vert scale routine
    VertScale ( pTemp,       //temp image is source
                uNewWidth,
                uOrigHeight,
                pDstImage,   //dest image is dest
                uNewWidth,
                uNewHeight);


    //done with temp buffer
    delete pTemp;

    //return same pointer we received
    return pDstImage;
} 
