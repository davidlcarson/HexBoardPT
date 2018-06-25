//cImageBuffer.cpp

#include "cImageBuffer.h"
#include <stdlib.h> //abs
#include "c2PassScale.h"

const dcBGRA defaultClearColor = {64, 64, 0, 0}; //bgra
const dcBGRA defaultDrawColor = {0, 0, 255, 0};  //bgra
const int evenPalette8[] = 
  {0, 36, 72, 108, 144, 180, 216, 255};
const int evenPalette4[] =
  {0, 85, 170, 255};

//Base Class
//****************************************************************************/
cIMAGE_BUFFER::cIMAGE_BUFFER(void)
{
   /*
   Create a single pixel buffer. (1x1).
   Set it to some color.

   If paint is set to stretch, it will paint the entire window
   ..client to that color. (Actually, uses rcpaint for the client
   ..area, which is almost always the entire client area.
   */

   //create a single pixel, padded wide
   m_pBits = new char[4];

   //clear it
   //ZeroMemory(m_pBits, sizeof(char)*4);
   *(dcBGRA*)m_pBits = defaultClearColor;

   //clear the packed dib structure
   ZeroMemory(&m_packedDib, sizeof(m_packedDib));

   //init the packed dib structure
   m_packedDib.bmih.biSize = sizeof(BITMAPINFOHEADER);
   m_packedDib.bmih.biPlanes = 1;
   
   //we don't worry about -1 or 1 for height, since we're only 1 row tall...
   m_packedDib.bmih.biWidth = 1;
   m_packedDib.bmih.biHeight = -1;   
   m_packedDib.bmih.biBitCount = 32;  //we use 32bit as our default
   
   m_paddedWide = 4; 

   m_pFilename = NULL;  //no filename used here
   //m_bStretch = false;  //no stretch by default

   return;
}

//****************************************************************************/
cIMAGE_BUFFER::~cIMAGE_BUFFER(void)
{
	//only membe we care about is the pointer to the buffer
	CLEAN_DELETE(m_pBits);

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER::readFile(const wchar_t* pFilename)
{
   FILE* pFile;  //pointer to source file
   _wfopen_s(&pFile, pFilename, L"rb");
   MYASSERT(pFile > 0);

   //hold the source bmfh
   BITMAPFILEHEADER bmfh;
   fread(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);
      
   fread(&m_packedDib.bmih, 1, sizeof(BITMAPINFOHEADER), pFile);

   if(m_packedDib.bmih.biBitCount == 8){
      //read the palette
      WORD numQuads;
      numQuads = (WORD)m_packedDib.bmih.biClrUsed;
      if(numQuads == 0)
         numQuads = 256;

      fread(&m_packedDib.quads, numQuads, sizeof(RGBQUAD), pFile);      
   }

   //calc padded wide
   m_paddedWide = m_packedDib.bmih.biWidth * (m_packedDib.bmih.biBitCount>>3);
   while(m_paddedWide %4)
      m_paddedWide++;

   //create buffer to hold rgb image
   if (m_pBits != NULL)
	   CLEAN_DELETE(m_pBits);
   m_pBits = new char[m_paddedWide * abs(m_packedDib.bmih.biHeight)];

   //point to the bits in the file
   fseek(pFile, bmfh.bfOffBits, SEEK_SET);

   char* pDst;
   pDst = m_pBits;
   pDst += (m_paddedWide * (abs(m_packedDib.bmih.biHeight)-1));

   for(WORD tall=0; tall<abs(m_packedDib.bmih.biHeight); tall++){

      fread(pDst, 1, m_paddedWide, pFile);

      pDst -= m_paddedWide;      
   }
   
   fclose(pFile);

   m_packedDib.bmih.biHeight *= -1;

   return;   
}

//****************************************************************************/
void cIMAGE_BUFFER::SaveBMP(const char* pFilename)
{
   BITMAPFILEHEADER bmfh;
   DWORD sizeFile;

   /*
   Assume no palette. Assume offset to bits must be modulo 4.
   That means we need a padding of 2 bytes to the pBits
   */      

   //we calc size, but I think can be 0 if non-compressed bmp....
   sizeFile = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

   if(m_packedDib.bmih.biBitCount == 8)
      sizeFile += sizeof(RGBQUAD)*256;

   sizeFile += m_paddedWide * abs(m_packedDib.bmih.biHeight);   

   bmfh.bfType = 'MB';
   bmfh.bfSize = sizeFile;
   bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 

   if(m_packedDib.bmih.biBitCount == 8){
      bmfh.bfOffBits += sizeof(RGBQUAD) * 256;
   }

   bmfh.bfReserved1 = 0;
   bmfh.bfReserved2 = 0;
   
   FILE* pOutput;
   fopen_s(&pOutput, (char*)pFilename, "wb");
   fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pOutput);   
   fwrite(&m_packedDib.bmih, 1, sizeof(BITMAPINFOHEADER), pOutput);   

   //if we're indexed, store pallette
   if(m_packedDib.bmih.biBitCount == 8){
   fwrite(&m_packedDib.quads, 256, sizeof(RGBQUAD), pOutput);
   }

   BYTE* pSrc;
   
   pSrc = (BYTE*)m_pBits;
   BYTE* pSrcwalker;
   pSrcwalker = pSrc;

   for(WORD height=0; height<abs(m_packedDib.bmih.biHeight); height++){
      fwrite(pSrcwalker, 1, m_paddedWide, pOutput);
      pSrcwalker+= m_paddedWide;
   }

   fclose(pOutput);
      
   return;
}

//end base class

//****************************************************************************/
//****************************************************************************/
cIMAGE_BUFFER8::cIMAGE_BUFFER8(WORD width, WORD height) 
{
   //clear the packed dib structure
   ZeroMemory(&m_packedDib, sizeof(m_packedDib));

   //init the packed dib structure
   m_packedDib.bmih.biSize = sizeof(BITMAPINFOHEADER);
   m_packedDib.bmih.biPlanes = 1;
   
   m_packedDib.bmih.biWidth = width;
   m_packedDib.bmih.biHeight = -height;   
   m_packedDib.bmih.biBitCount = 8; 

   //create generic palette  
   WORD curPalette = 0;
 
   for(BYTE red=0; red<8; red++){
      for(BYTE green=0; green<8; green++){
         for(BYTE blue=0; blue<4; blue++){
            m_packedDib.quads[curPalette].rgbBlue = evenPalette4[blue];
            m_packedDib.quads[curPalette].rgbGreen = evenPalette8[green];
            m_packedDib.quads[curPalette].rgbRed = evenPalette8[red];
            m_packedDib.quads[curPalette].rgbReserved = 0;

            curPalette++; 
         }
      }
   }   
   
   m_clearColor = colorToIndex(defaultClearColor);  
   m_drawColor = colorToIndex(defaultDrawColor);
      
   m_paddedWide = width; 
   while(m_paddedWide%4)
      m_paddedWide++;

   m_pFilename = NULL;  //no filename

   if (m_pBits != NULL)
	   CLEAN_DELETE(m_pBits);
   m_pBits = (char*)new BYTE[-m_packedDib.bmih.biHeight * m_paddedWide];
   MYASSERT(m_pBits != NULL);

   //clear
   ClearBuffer();

   return;
}

//****************************************************************************/
cIMAGE_BUFFER8::cIMAGE_BUFFER8(const wchar_t* pFilename)
{
   readFile(pFilename);

   m_clearColor = colorToIndex(defaultClearColor);
   m_drawColor = colorToIndex(defaultDrawColor);

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER8::SetDrawColor(COLORREF cr)
{
   dcBGRA bgra;

   bgra.alpha = 0;
   bgra.blue = GetBValue(cr);
   bgra.green = GetGValue(cr);
   bgra.red = GetRValue(cr);

   unsigned char index;

   index = colorToIndex(bgra);

   m_drawColor = index;
   
   return;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER8::GetDrawColor(void)
{
   dcBGRA retColor;

   retColor.alpha = 0;
   retColor.blue = m_packedDib.quads[m_drawColor].rgbBlue;
   retColor.green = m_packedDib.quads[m_drawColor].rgbGreen;
   retColor.red = m_packedDib.quads[m_drawColor].rgbRed;

   return retColor;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER8::GetPixel(WORD x, WORD y)
{
   char* pPixel;

   pPixel = m_pBits + (y * m_paddedWide);
   pPixel += x;

   char index;

   index = *pPixel;

   dcBGRA retColor; 

   retColor.alpha = 0;
   retColor.blue = m_packedDib.quads[index].rgbBlue;
   retColor.green = m_packedDib.quads[index].rgbGreen;
   retColor.red = m_packedDib.quads[index].rgbRed;
   
   return retColor;
}

//****************************************************************************/
void cIMAGE_BUFFER8::SetClearColor(COLORREF cr)
{
   dcBGRA bgra;

   bgra.alpha = 0;
   bgra.blue = GetBValue(cr);
   bgra.green = GetGValue(cr);
   bgra.red = GetRValue(cr);

   unsigned char index;

   index = colorToIndex(bgra);

   m_clearColor = index;
   
   return;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER8::GetClearColor(void)
{
   dcBGRA retColor;

   retColor.alpha = 0;
   retColor.blue = m_packedDib.quads[m_clearColor].rgbBlue;
   retColor.green = m_packedDib.quads[m_clearColor].rgbGreen;
   retColor.red = m_packedDib.quads[m_clearColor].rgbRed;

   return retColor;
}

//****************************************************************************/
void inline cIMAGE_BUFFER8::PlotPixel(int x, int y)
{
   char* pDst;

   pDst = (char*)m_pBits;
   pDst = (char*)pDst + (y*m_paddedWide) + x;

   *pDst = m_drawColor;

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER8::DrawLine(int x1, int y1, int x2, int y2)
{
   //based on linedraw routine downloaded from internet:
   //basically Bresenham's routine

   int i;
   int dx, dy;
   int sdx, sdy;
   int dxabs, dyabs;
   int x, y;
   int px, py;

   dx = x2-x1;  //horizontal distance of line
   dy = y2-y1;  //vertical distance of line

   dxabs = abs(dx);
   dyabs = abs(dy);

   sdx = dx < 0 ? -1 : 1;
   sdy = dy < 0 ? -1 : 1;

   x = dyabs>>1;
   y = dxabs>>1;

   px = x1;
   py = y1;

   PlotPixel(px, py);

   if(dxabs >= dyabs){   
      //line is more horizontal than vertical
      for(i=0; i<dxabs; i++){
         y+=dyabs;
         if(y>=dxabs){
            y-=dxabs;
            py+=sdy;
         }
         px+=sdx;
         PlotPixel(px, py);
      }
   }
   else{
      //line is more vertical than horizontal
      for(i=0; i<dyabs; i++){
        x+=dxabs;
        if(x>=dyabs){
          x-=dyabs;
          px+=sdx;
        } 
        py+=sdy;
        PlotPixel(px, py);
      }
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER8::DrawHorzLine(int x1, int y1, int width)
{   
   char* pDst;

   pDst = (char*)m_pBits;
   pDst += (m_paddedWide * y1) + x1;   
 

   for(WORD i=0; i<width; i++){
      *pDst++ = m_drawColor;
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER8::DrawVertLine(int x1, int y1, int height)
{    
   char* pDst;

   pDst = (char*)m_pBits;
   pDst += (m_paddedWide * y1) + x1;

   for(WORD i=0; i<height; i++){
      *pDst = m_drawColor;
      pDst += m_paddedWide;
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER8::ClearBuffer(void)
{   
     
   char* pDestWalker;

   BYTE* pRow;
   pDestWalker = (char*)m_pBits;
   pRow = (BYTE*)m_pBits;

   for(WORD tall=0; tall<-m_packedDib.bmih.biHeight; tall++){

      for(WORD wide=0; wide<m_packedDib.bmih.biWidth; wide++){
         pDestWalker[wide] = m_clearColor;
      }
      pRow += m_paddedWide;
      pDestWalker = (char*)pRow;
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER8::StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
    int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight)
{
   cIMAGE_BUFFER8* pNewBuffer = NULL;

   if(pDestBuffer->GetBpp() != 8){
      pNewBuffer = (cIMAGE_BUFFER8*)convertBuffer(pDestBuffer->GetBpp());

      pNewBuffer->StretchBlt(pDestBuffer, dstX, dstY,
         dstWidth, dstHeight, srcX, srcY, srcWidth, srcHeight);

      delete pNewBuffer;
   }
      
   else{
      if((srcWidth != dstWidth) || (srcHeight |= dstHeight)){

         scaleBuffer(pDestBuffer, dstX, dstY, dstWidth, dstHeight, 
            srcX, srcY, srcWidth, srcHeight);  
      }
      else{
         copyBuffer(pDestBuffer, dstX, dstY, dstWidth, dstHeight,
            srcX, srcY);
      }
   }

   return;
}

//****************************************************************************/
cIMAGE_BUFFER* cIMAGE_BUFFER8::convertBuffer(WORD bpp)
{
   
   //Create a buffer with param bpp bits per pixel, same image as us   

   cIMAGE_BUFFER* pNewBuffer = NULL;        

   switch(bpp){

      //8bit. Should just have called copy buffer, but let's support anyway
      case 8:
         {
         RGBQUAD* pDestQuads;

         pNewBuffer = new cIMAGE_BUFFER8(GetPixWide(), GetRowsTall());
         pDestQuads = pNewBuffer->GetpQuads();   
         for(int i=0; i<256; i++){
            pDestQuads[i] = m_packedDib.quads[i];
         }
         
         copyBuffer(pNewBuffer, 0, 0, GetPixWide(), GetRowsTall(),
            0, 0);
         }
         break;

      //converting our buffer to a 24-bit buffer
      case 24:
        {
         BYTE* pSrcWalker;
         BYTE* pDstWalker;
         BYTE curIndex;
         dcBGR curPixel;

         pNewBuffer = new cIMAGE_BUFFER24(GetPixWide(), GetRowsTall());

         pSrcWalker = (BYTE*)m_pBits;  //us (src)
         pDstWalker = (BYTE*)pNewBuffer->GetpBits(); //dest

         for(WORD tall = 0; tall < GetRowsTall(); tall++){

            //convert each row
            for(WORD wide = 0; wide < GetPixWide(); wide++){

               //convert each pixel
               curIndex = pSrcWalker[wide];
               
               curPixel.blue = m_packedDib.quads[curIndex].rgbBlue;
               curPixel.green = m_packedDib.quads[curIndex].rgbGreen;
               curPixel.red = m_packedDib.quads[curIndex].rgbRed;
 
               ((dcBGR*)pDstWalker)[wide] = curPixel;
            }
             
            pSrcWalker += m_paddedWide;
            pDstWalker += pNewBuffer->GetPaddedWide();
         }
         }
         break;         
   }

   return pNewBuffer;
}

//****************************************************************************/
void cIMAGE_BUFFER8::copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dxtX, int dxtY,
   int dstWidth, int dstHeight, int srcX, int srcY)
{

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER8::scaleBuffer(cIMAGE_BUFFER* pDeestBuffer, int dstX, int dstY,
   int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight)
{


   return;
}

#if 0
//****************************************************************************/
void cIMAGE_BUFFER8::BitBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int width, int height, int srcX, int srcY)
{

////****************************************************************************/
//void cIMAGE_BUFFER8::Blt(int dx, int dy, WORD width, WORD height, cIMAGE_BUFFER24* pib_dst,
//   int sx, int sy)
//{
#if 0
   /* Copy a section of our 8bit buffer to a same-size rectangle in a
      24bit buffer.
   */

   if((dx+width)<0)
      return;

   if(dx<0){
      width += dx;
      dx=0;
   }

   if((dy+height)<0)
      return;

   if(dy<0){
      height += dy;
      dy = 0;
   }

   int dif;

   if(dx >= pib_dst->GetPixWide())
      return;

   dif = pib_dst->GetPixWide() - (dx+width);
   if(dif<0)
      width += dif;   

   int rowsTall = pib_dst->GetRowsTall();
   if(dy >= rowsTall)
      return;

   dif = pib_dst->GetRowsTall() - (dy+height);
   if(dif<0)
      height += dif;
   
   WORD destPaddedBytesWide; //24bit
   WORD srcPaddedBytesWide;  //8bit

   destPaddedBytesWide = (WORD)pib_dst->GetPaddedWide();
   srcPaddedBytesWide = (WORD)m_paddedWide;

   //pointers
   BYTE* pDest;  //is actually 24bit bgr, but we treat as byte*   
   BYTE* pSrc;

   pDest = (BYTE*)pib_dst->GetpBits();   
   pDest += (dy * destPaddedBytesWide);
   pDest += dx*3;

   pSrc = (BYTE*)m_pBits;
   pSrc += (sy * srcPaddedBytesWide);
   pSrc += sx;

   BYTE* destWalker;
  
   for(WORD tall=0; tall<height; tall++){     

      destWalker = pDest;

      for(WORD wide=0; wide<width; wide++){

         dcBGR bgr;

         BYTE index;
         index = pSrc[wide];

         bgr.red = m_packedDib.quads[index].rgbRed;
         bgr.green = m_packedDib.quads[index].rgbGreen;
         bgr.blue = m_packedDib.quads[index].rgbBlue;

         
         *destWalker++ = bgr.blue;
         *destWalker++ = bgr.green;
         *destWalker++ = bgr.red;         
      }

      pDest += destPaddedBytesWide;
      pSrc += srcPaddedBytesWide;
   }
#endif

   return;  
}
#endif

//24bit classs  
//****************************************************************************/
cIMAGE_BUFFER24::cIMAGE_BUFFER24(WORD width, WORD height) 
{
   //clear the packed dib structure
   ZeroMemory(&m_packedDib, sizeof(m_packedDib));

   //init the packed dib structure
   m_packedDib.bmih.biSize = sizeof(BITMAPINFOHEADER);
   m_packedDib.bmih.biPlanes = 1;
   
   m_packedDib.bmih.biWidth = width;
   m_packedDib.bmih.biHeight = -height;   
   m_packedDib.bmih.biBitCount = 24; 
   
   m_paddedWide = width*3; 
   while(m_paddedWide%4)
      m_paddedWide++;

   m_pFilename = NULL;  //no filename

   //Set colors to default
   m_clearColor.red = defaultClearColor.red;
   m_clearColor.green = defaultClearColor.green;
   m_clearColor.blue = defaultClearColor.blue;

   m_drawColor.red = defaultDrawColor.red;
   m_drawColor.green = defaultDrawColor.green;
   m_drawColor.blue = defaultDrawColor.blue;

   if (m_pBits)
	   CLEAN_DELETE(m_pBits);
   m_pBits = (char*)new BYTE[-m_packedDib.bmih.biHeight * m_paddedWide];
   MYASSERT(m_pBits != NULL);

   //clear to color
   ClearBuffer();

   return;
}

//****************************************************************************/
cIMAGE_BUFFER24::cIMAGE_BUFFER24(const wchar_t* pFilename)
{ 

   readFile(pFilename);

   m_drawColor.red = defaultDrawColor.red;
   m_drawColor.green = defaultDrawColor.green;
   m_drawColor.blue = defaultDrawColor.blue;

   m_clearColor.red = defaultClearColor.red;
   m_clearColor.green = defaultClearColor.green;
   m_clearColor.blue = defaultClearColor.blue;

   return;  
}

//****************************************************************************/
cIMAGE_BUFFER24::~cIMAGE_BUFFER24(void)
{
	//should call cIMAGE_BUFFER destructor!

	return;
}
//****************************************************************************/
dcBGRA cIMAGE_BUFFER24::GetClearColor(void)
{
   dcBGRA retColor;

   retColor.alpha = 0;
   retColor.blue = m_clearColor.blue;
   retColor.green = m_clearColor.green;
   retColor.red = m_clearColor.red;

   return retColor;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER24::GetDrawColor(void)
{

   dcBGRA retColor;

   retColor.alpha = 0;
   retColor.blue = m_drawColor.blue;
   retColor.green = m_drawColor.green;
   retColor.red = m_drawColor.red;

   return retColor;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER24::GetPixel(WORD x, WORD y)
{
   dcBGR* pPixel;
   char* pChar;

   pChar = (char*)m_pBits + (m_paddedWide * y);
   pChar += (x*3);

   pPixel = (dcBGR*)pChar;

   dcBGRA retColor;

   retColor.alpha = 0;
   retColor.blue = pPixel->blue;
   retColor.green = pPixel->green;
   retColor.red = pPixel->red;

   return retColor;
}

//****************************************************************************/
void cIMAGE_BUFFER24::SetDrawColor(COLORREF cr)
{

  dcBGR bgr;
  
  bgr.blue = GetBValue(cr);
  bgr.green = GetGValue(cr);
  bgr.red = GetRValue(cr);

  m_drawColor = bgr;

  return;
}

//****************************************************************************/
void cIMAGE_BUFFER24::SetClearColor(COLORREF cr)
{
  dcBGR bgr;

  bgr.blue = GetBValue(cr);
  bgr.green = GetGValue(cr);
  bgr.red = GetRValue(cr);

  m_clearColor = bgr;

  return;
}

//****************************************************************************/
void cIMAGE_BUFFER24::DrawHorzLine(int x1, int y1, int width)
{   
   dcBGR* pDst;

   //if y too high, return;
   if(y1 < 0)
      return;
   //if y too low, return
   if(y1 > GetRowsTall())
      return;   

   if((x1+width) > GetPixWide())
      width -= (x1+width) - GetPixWide();

   if(x1<0){
      x1 = 0;
      width -= x1;
   }
   
   pDst = (dcBGR*)m_pBits;
   pDst = (dcBGR*)((char*)pDst + m_paddedWide * y1);

   pDst += x1;   

   for(WORD i=0; i<width; i++){
      *pDst++ = m_drawColor;
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER24::DrawVertLine(int x1, int y1, int height)
{   
   dcBGR* pDst;

   if(x1 < 0)
      return;

   if(x1 > GetPixWide())
      return;

   if(y1 < 0){
      y1 = 0;
      height -= y1;
   }
         

   pDst = (dcBGR*)m_pBits;
   pDst = (dcBGR*)((char*)pDst + m_paddedWide * y1);

   pDst += x1;

   for(WORD i=0; i<height; i++){
      *pDst = m_drawColor;
      pDst = (dcBGR*)((char*)pDst + m_paddedWide);
   }

   return;
}


//****************************************************************************/
void cIMAGE_BUFFER24::ClearBuffer(void)
{
          
   dcBGR* pDestWalker;

   BYTE* pRow;
   pDestWalker = (dcBGR*)m_pBits;
   pRow = (BYTE*)m_pBits;

   for(WORD tall=0; tall<-m_packedDib.bmih.biHeight; tall++){

      for(WORD wide=0; wide<m_packedDib.bmih.biWidth; wide++){
         pDestWalker[wide] = m_clearColor; //bgrColor; //m_ClearColor;
      }
      pRow += m_paddedWide;
      pDestWalker = (dcBGR*)pRow;
   }

   return;
}

//****************************************************************************/
void inline cIMAGE_BUFFER24::PlotPixel(int x, int y)
{
   BYTE* pByte = (BYTE*)m_pBits;
   pByte += (m_paddedWide * y) + (sizeof(dcBGR) * x);
   
   *(dcBGR*)pByte = m_drawColor;

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER24::DrawLine(int x1, int y1, int x2, int y2)
{
   //based on linedraw routine downloaded from internet:
   //basically Bresenham's routine

   //x1 = clipMinX(x1);
   if(x1 < 0) x1 = 0;

   //y1 = clipMinY(y1);
   if(y1 < 0) y1 = 0;

   //x2 = clipMaxX(x2);
   if(x2 > GetPixWide()) x2 = GetPixWide();

   //y2 = clipMaxY(y2);
   if(y2 > GetRowsTall()) y2 = GetRowsTall();

   int i;
   int dx, dy;
   int sdx, sdy;
   int dxabs, dyabs;
   int x, y;
   int px, py;

   dx = x2-x1;  //horizontal distance of line
   dy = y2-y1;  //vertical distance of line

   dxabs = abs(dx);
   dyabs = abs(dy);

   sdx = dx < 0 ? -1 : 1;
   sdy = dy < 0 ? -1 : 1;

   x = dyabs>>1;
   y = dxabs>>1;

   px = x1;
   py = y1;

   PlotPixel(px, py);

   if(dxabs >= dyabs){   
      //line is more horizontal than vertical
      for(i=0; i<dxabs; i++){
         y+=dyabs;
         if(y>=dxabs){
            y-=dxabs;
            py+=sdy;
         }
         px+=sdx;
         PlotPixel(px, py);
      }
   }
   else{
      //line is more vertical than horizontal
      for(i=0; i<dyabs; i++){
        x+=dxabs;
        if(x>=dyabs){
          x-=dyabs;
          px+=sdx;
        } 
        py+=sdy;
        PlotPixel(px, py);
      }
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER24::StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
    int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight)
{
   
   cIMAGE_BUFFER24* pNewBuffer = NULL;

   if(pDestBuffer->GetBpp() != 24){
      pNewBuffer = (cIMAGE_BUFFER24*)convertBuffer(pDestBuffer->GetBpp());

      pNewBuffer->StretchBlt(pDestBuffer, dstX, dstY,
         dstWidth, dstHeight, srcX, srcY, srcWidth, srcHeight);

      delete pNewBuffer;
   }
      
   else{
      if((srcWidth != dstWidth) || (srcHeight |= dstHeight)){

         scaleBuffer(pDestBuffer, dstX, dstY, dstWidth, dstHeight, 
            srcX, srcY, srcWidth, srcHeight);  
      }
      else{
         copyBuffer(pDestBuffer, dstX, dstY, dstWidth, dstHeight,
            srcX, srcY);
      }
   }

   return;
}

//****************************************************************************/
cIMAGE_BUFFER* cIMAGE_BUFFER24::convertBuffer(WORD bpp)
{
   //convert our buffer to some other bpp
   cIMAGE_BUFFER* pNewBuffer = NULL;


   return pNewBuffer;
}

//****************************************************************************/
void cIMAGE_BUFFER24::scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
   int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight)
{
   c2PASS_SCALE* pScale;
   
   pScale = new c2PASS_SCALE(24);

   pScale->Scale((BYTE*)m_pBits, srcWidth, srcHeight, 
      (BYTE*)pDestBuffer->GetpBits(), dstWidth, dstHeight);

   delete pScale;
}

//****************************************************************************/
void cIMAGE_BUFFER24::copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY, 
    int dstWidth, int dstHeight, int srcX, int srcY)
{
   //copy section of our buffer to another 24bit buffer, no sizing.

   char* pSrcWalker;
   char* pDstWalker;   

   //dest
   pDstWalker = (char*)pDestBuffer->GetpBits();
   pDstWalker += dstY * pDestBuffer->GetPaddedWide();
   pDstWalker += dstX * 3;

   //us
   pSrcWalker = m_pBits;
   pSrcWalker += srcY * m_paddedWide;
   pSrcWalker += srcX * 3;

   //pixels to copy each line, after clipping
   WORD pixWide;
   
   //clip against dest
   pixWide = dstWidth;
   while((dstX + pixWide) > pDestBuffer->GetPixWide())
      pixWide--;
   //clip against src
   while((srcX + pixWide) > GetPixWide())
      pixWide--;
   
   //width of line to copy in bytes
   WORD bytesWide = pixWide * 3;

   WORD rowsTall = dstHeight;
   while((dstY + rowsTall) > pDestBuffer->GetRowsTall())
      rowsTall--;
   while((srcY + rowsTall) > GetRowsTall())
      rowsTall--;
   
   for(WORD i=0; i<rowsTall; i++){
      memcpy(pDstWalker, pSrcWalker, bytesWide);
      pSrcWalker += GetPaddedWide();
      pDstWalker += pDestBuffer->GetPaddedWide();
   }

   return;
}

#if 0
//****************************************************************************/
void cIMAGE_BUFFER24::BitBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int width, int height, int srcX, int srcY)
{

#if 0
//****************************************************************************/
void cIMAGE_BUFFER24::Blt(int dx, int dy, WORD width, WORD height, 
     cIMAGE_BUFFER24* pib_dst, int sx, int sy)
{
   /* Copy a section of our 24bit buffer to a same-size rectangle in another
      24bit buffer. 
   */
   if((dx+width)<0)
      return;
   if(dx<0){
      width += dx;
      dx = 0;
   }

   if((dy+height)<0)
      return;

   if(dy<0){
      height += dy;
      dy = 0;
   }

   int dif;

   if(dx >= pib_dst->GetPixWide())
      return;
   dif = pib_dst->GetPixWide() - (dx+width);
   if(dif<0)
      width += dif;   

   int rowsTall = pib_dst->GetRowsTall();
   //if(dy > pib_dst->GetRowsTall())
   if(dy >= rowsTall)
      return;
   dif = pib_dst->GetRowsTall() - (dy+height);
   if(dif<0)
      height += dif;
   
   WORD destPaddedBytesWide; //dest
   WORD srcPaddedBytesWide;  //source

   destPaddedBytesWide = (WORD)pib_dst->GetPaddedWide();
   srcPaddedBytesWide = (WORD)m_paddedWide;

   //pointers
   BYTE* pDest;  //is actually 24bit bgr, but we treat as byte*   
   BYTE* pSrc;

   pDest = (BYTE*)pib_dst->GetpBits();   
   pDest += (dy * destPaddedBytesWide);
   pDest += dx*3;

   pSrc = (BYTE*)m_pBits;
   pSrc += (sy * srcPaddedBytesWide);
   pSrc += sx*3;

   //BYTE* destWalker;
   dcBGR* destWalker;
   dcBGR* srcWalker;
  
   for(WORD tall=0; tall<height; tall++){     

      destWalker = (dcBGR*)pDest;
      srcWalker = (dcBGR*)pSrc;

      for(WORD wide=0; wide<width; wide++){      
         destWalker[wide] = srcWalker[wide];                  
      }

      pDest = pDest + destPaddedBytesWide;
      pSrc = pSrc + srcPaddedBytesWide;
   }

#endif

   return;  
}
#endif

//start 8bit class
//****************************************************************************/
cIMAGE_BUFFER16::cIMAGE_BUFFER16(WORD width, WORD height)
{
   //clear the packed dib structure
   ZeroMemory(&m_packedDib, sizeof(m_packedDib));

   //init the packed dib structure
   m_packedDib.bmih.biSize = sizeof(BITMAPINFOHEADER);
   m_packedDib.bmih.biPlanes = 1;
   
   m_packedDib.bmih.biWidth = width;
   m_packedDib.bmih.biHeight = -height;   
   m_packedDib.bmih.biBitCount = 16; 
   
   m_paddedWide = width*2; 
   while(m_paddedWide%4)
      m_paddedWide++;

   m_pFilename = NULL;  //no filename

   //Set to default, even tho we don't use   
   m_clearColor = colorToWord(defaultClearColor);
   m_drawColor = colorToWord(defaultDrawColor);

   if (m_pBits)
	   CLEAN_DELETE(m_pBits);
   m_pBits = (char*)new BYTE[-m_packedDib.bmih.biHeight * m_paddedWide];
   MYASSERT(m_pBits != NULL);

   ClearBuffer();

   return;
}

//****************************************************************************/
cIMAGE_BUFFER16::cIMAGE_BUFFER16(const wchar_t* pFilename)
{
   readFile(pFilename);

   m_drawColor = colorToWord(defaultDrawColor);
   m_clearColor = colorToWord(defaultClearColor);
   
   return;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER16::GetClearColor(void)
{
   dcBGRA retColor;   
   WORD curColor;

   retColor.alpha = 0;
   curColor =  m_clearColor & 0x1f;
   retColor.blue = curColor << 3;
   curColor = (m_clearColor >> 5) & 0x1f;
   retColor.green = curColor << 3;
   curColor = (m_clearColor >> 10) & 0x1f;
   retColor.red = curColor << 3;

   return retColor;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER16::GetDrawColor(void)
{
   dcBGRA retColor;   
   WORD curColor;

   retColor.alpha = 0;
   curColor =  m_drawColor & 0x1f;
   retColor.blue = curColor << 3;
   curColor = (m_drawColor >> 5) & 0x1f;
   retColor.green = curColor << 3;
   curColor = (m_drawColor >> 10) & 0x1f;
   retColor.red = curColor << 3;

   return retColor;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER16::GetPixel(WORD x, WORD y)
{
   WORD* p16;
   char* pchar;

   pchar = (char*)m_pBits + (y * m_paddedWide);
   pchar += (x*2);

   p16 = (WORD*)pchar;

   dcBGRA retColor;

   retColor = wordToBGRA(*p16);

   return retColor;
}

//****************************************************************************/
void cIMAGE_BUFFER16::SetDrawColor(COLORREF cr)
{

   //not currently supported!
   //but call colorToWord to implement!
   
   return;  
}

//****************************************************************************/
void cIMAGE_BUFFER16::SetClearColor(COLORREF cr)
{
   
   dcBGRA bgra;
   bgra.alpha = 0;
   bgra.blue = GetBValue(cr);
   bgra.green = GetGValue(cr);
   bgra.red = GetRValue(cr);

   m_clearColor = colorToWord(bgra);
   
   return;  
}

//****************************************************************************/
void cIMAGE_BUFFER16::DrawLine(int x1, int y1, int x2, int y2)
{
   //based on linedraw routine downloaded from internet:
   //basically Bresenham's routine
   
   if(x1 < 0) x1 = 0;   
   if(y1 < 0) y1 = 0;
   if(x2 > GetPixWide()) x2 = GetPixWide();
   if(y2 > GetRowsTall()) y2 = GetRowsTall();

   int i;
   int dx, dy;
   int sdx, sdy;
   int dxabs, dyabs;
   int x, y;
   int px, py;

   dx = x2-x1;  //horizontal distance of line
   dy = y2-y1;  //vertical distance of line

   dxabs = abs(dx);
   dyabs = abs(dy);

   sdx = dx < 0 ? -1 : 1;
   sdy = dy < 0 ? -1 : 1;

   x = dyabs>>1;
   y = dxabs>>1;

   px = x1;
   py = y1;

   PlotPixel(px, py);

   if(dxabs >= dyabs){   
      //line is more horizontal than vertical
      for(i=0; i<dxabs; i++){
         y+=dyabs;
         if(y>=dxabs){
            y-=dxabs;
            py+=sdy;
         }
         px+=sdx;
         PlotPixel(px, py);
      }
   }
   else{
      //line is more vertical than horizontal
      for(i=0; i<dyabs; i++){
        x+=dxabs;
        if(x>=dyabs){
          x-=dyabs;
          px+=sdx;
        } 
        py+=sdy;
        PlotPixel(px, py);
      }
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER16::DrawHorzLine(int x1, int y1, int width)
{   
   WORD* pDst = (WORD*)m_pBits;
   
   pDst = (WORD*) ((char*)pDst +m_paddedWide * y1) +x1;

   for(WORD i=0; i<width; i++){
      *pDst++ = m_drawColor;
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER16::DrawVertLine(int x1, int y1, int height)
{
  
   WORD* pDst;

   pDst = (WORD*)m_pBits;
   pDst = (WORD*) ((char*)pDst +m_paddedWide * y1) + x1;

   for(WORD i=0; i<height; i++){
      *pDst = m_drawColor;
      pDst = (WORD*) ((char*)pDst + m_paddedWide);
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER16::ClearBuffer(void)
{   

   WORD* pDestWalker;

   BYTE* pRow;

   pDestWalker = (WORD*)m_pBits;
   pRow = (BYTE*)m_pBits;

   for(WORD tall=0; tall<-m_packedDib.bmih.biHeight; tall++){

      for(WORD wide=0; wide<m_packedDib.bmih.biWidth; wide++){
         pDestWalker[wide] = m_clearColor;
      }
      pRow += m_paddedWide;
      pDestWalker = (WORD*)pRow;
   }

   return;
}

//****************************************************************************/
void inline cIMAGE_BUFFER16::PlotPixel(int x, int y)
{
   WORD* pDst;

   pDst = (WORD*)m_pBits;
   pDst = (WORD*)((char*)pDst + (y*m_paddedWide)) + x;

   *pDst = m_drawColor;
   
   return;
}

//****************************************************************************/
void cIMAGE_BUFFER16::StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
    int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int SrcHeight)
{



}

//****************************************************************************/
void cIMAGE_BUFFER16::copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
    int dstWidth, int dstHeight, int srcX, int srcY)
{


   return;
}

//****************************************************************************/
void cIMAGE_BUFFER16::scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
   int dstWidth, int dxtHeight, int srcX, int srcY, int srcWidth, int srcHeight)
{


   return;
}

//****************************************************************************/
cIMAGE_BUFFER* cIMAGE_BUFFER16::convertBuffer(WORD bpp)
{
   cIMAGE_BUFFER* pNewBuffer = NULL;

   return pNewBuffer;
}


//start 32bit class
//****************************************************************************/
cIMAGE_BUFFER32::cIMAGE_BUFFER32(WORD width, WORD height)
{   

   //clear the packed dib structure
   ZeroMemory(&m_packedDib, sizeof(m_packedDib));

   //init the packed dib structure
   m_packedDib.bmih.biSize = sizeof(BITMAPINFOHEADER);
   m_packedDib.bmih.biPlanes = 1;
   
   m_packedDib.bmih.biWidth = width;
   m_packedDib.bmih.biHeight = -height;   
   m_packedDib.bmih.biBitCount = 32; 
   
   //32bit is always padded width
   m_paddedWide = m_packedDib.bmih.biWidth * sizeof(dcBGRA);   

   m_pFilename = NULL;  //no filename

   //Set to default, even tho we don't use   
   m_clearColor = defaultClearColor;
   m_drawColor = defaultDrawColor;

   if (m_pBits)
	   CLEAN_DELETE(m_pBits);
   m_pBits = (char*)new BYTE[-m_packedDib.bmih.biHeight * m_paddedWide];
   MYASSERT(m_pBits != NULL);
   
   ClearBuffer();

   return;
}

//****************************************************************************/
cIMAGE_BUFFER32::cIMAGE_BUFFER32(const wchar_t* pFilename)
{
   readFile(pFilename);

   m_drawColor = defaultDrawColor;
   m_clearColor = defaultClearColor;
   
   return;
}

//****************************************************************************/
void cIMAGE_BUFFER32::DrawLine(int x1, int y1, int x2, int y2)
{
   //based on linedraw routine downloaded from internet:
   //basically Bresenham's routine

   int i;
   int dx, dy;
   int sdx, sdy;
   int dxabs, dyabs;
   int x, y;
   int px, py;

   dx = x2-x1;  //horizontal distance of line
   dy = y2-y1;  //vertical distance of line

   dxabs = abs(dx);
   dyabs = abs(dy);

   sdx = dx < 0 ? -1 : 1;
   sdy = dy < 0 ? -1 : 1;

   x = dyabs>>1;
   y = dxabs>>1;

   px = x1;
   py = y1;

   PlotPixel(px, py);

   if(dxabs >= dyabs){   
      //line is more horizontal than vertical
      for(i=0; i<dxabs; i++){
         y+=dyabs;
         if(y>=dxabs){
            y-=dxabs;
            py+=sdy;
         }
         px+=sdx;
         PlotPixel(px, py);
      }
   }
   else{
      //line is more vertical than horizontal
      for(i=0; i<dyabs; i++){
        x+=dxabs;
        if(x>=dyabs){
          x-=dyabs;
          px+=sdx;
        } 
        py+=sdy;
        PlotPixel(px, py);
      }
   }

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER32::DrawHorzLine(int x1, int y1, int width)
{
 
   dcBGRA* pDst;

   pDst = (dcBGRA*)m_pBits;
  
   pDst += (m_packedDib.bmih.biWidth * y1) + x1;

   for(WORD i=0; i<width; i++){
      *pDst++ = m_drawColor;
   }

   return;
}   

//****************************************************************************/
void cIMAGE_BUFFER32::DrawVertLine(int x1, int y1, int height)
{
  
   dcBGRA* pDst;

   pDst = (dcBGRA*)m_pBits;
 
   pDst += (m_packedDib.bmih.biWidth * y1) + x1;

   for(WORD i=0; i<height; i++){
      *pDst = m_drawColor;
      pDst += m_packedDib.bmih.biWidth;
   }

   return;
}   

//****************************************************************************/
void cIMAGE_BUFFER32::ClearBuffer(void)
{   
   dcBGRA* pDestWalker;
   
   pDestWalker = (dcBGRA*)m_pBits;

   for(WORD tall=0; tall<-m_packedDib.bmih.biHeight; tall++){

      for(WORD wide=0; wide<m_packedDib.bmih.biWidth; wide++){
         pDestWalker[wide] = m_clearColor;
      }
      
      pDestWalker += m_packedDib.bmih.biWidth;
   }

   return;
}

//****************************************************************************/
void inline cIMAGE_BUFFER32::PlotPixel(int x, int y)
{
   dcBGRA* pbgra = (dcBGRA*)m_pBits;
   pbgra += (y * m_packedDib.bmih.biWidth) + x;
   
   *pbgra = m_drawColor;

   return;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER32::GetPixel(WORD x, WORD y)
{
  
   dcBGRA* pValue;
   pValue = (dcBGRA*)m_pBits;
   pValue += (y * m_packedDib.bmih.biWidth) + x;   

   return *pValue;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER32::GetClearColor(void)
{
   dcBGRA retColor;

   retColor.alpha = 0;
   retColor.blue = m_clearColor.blue;
   retColor.green = m_clearColor.green;
   retColor.red = m_clearColor.red;

   return retColor;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER32::GetDrawColor(void)
{
   dcBGRA retColor;

   retColor.alpha = 0;
   retColor.blue = m_drawColor.blue;
   retColor.green = m_drawColor.green;
   retColor.red = m_drawColor.red;

   return retColor;
}

//****************************************************************************/
void cIMAGE_BUFFER32::SetDrawColor(COLORREF cr)
{
   dcBGRA bgra;

   bgra.alpha = 0;
   bgra.blue = GetBValue(cr);
   bgra.green = GetGValue(cr);
   bgra.red = GetRValue(cr);

   m_drawColor = bgra;

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER32::SetClearColor(COLORREF cr)
{
   dcBGRA bgra;

   bgra.alpha = 0;
   bgra.blue = GetBValue(cr);
   bgra.green = GetGValue(cr);
   bgra.red = GetRValue(cr);

   m_clearColor = bgra;

   return;
}

//****************************************************************************/
void cIMAGE_BUFFER32::StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
    int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int SrcHeight)
{



}

//****************************************************************************/
void cIMAGE_BUFFER32::copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
   int dstWidth, int dstHeight, int srcX, int srcY)
{


   return;
}

//****************************************************************************/
void cIMAGE_BUFFER32::scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
   int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight)
{

   return;
}

//****************************************************************************/
cIMAGE_BUFFER* cIMAGE_BUFFER32::convertBuffer(WORD bpp)
{
   cIMAGE_BUFFER* pNewBuffer = NULL;

   return pNewBuffer;
}

//****************************************************************************/
cIMAGE_BUFFER* CreateImageBuffer(const wchar_t* pFilename)
{
   cIMAGE_BUFFER* pImageBuffer = NULL;

   wchar_t curDir[128];
   GetCurrentDirectory(128, curDir);

   FILE* pFile;  //pointer to source file
   _wfopen_s(&pFile, pFilename, L"rb");
   MYASSERT(pFile > 0);

   //hold the source bmfh
   BITMAPFILEHEADER bmfh;
   fread(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);

   BITMAPINFOHEADER bmih;      
   fread(&bmih, 1, sizeof(BITMAPINFOHEADER), pFile);

   fclose(pFile);

   switch(bmih.biBitCount){
      case 8:
         pImageBuffer = new cIMAGE_BUFFER8(pFilename);         
         break;
      case 16:
         pImageBuffer = new cIMAGE_BUFFER16(pFilename);
         break;
      case 24:
         pImageBuffer = new cIMAGE_BUFFER24(pFilename);
         break;
      case 32:
         pImageBuffer = new cIMAGE_BUFFER32(pFilename);
         break;

      default:
         break;
   }

   return pImageBuffer;
}

//****************************************************************************/
WORD cIMAGE_BUFFER16::colorToWord(dcBGRA inColor)
{
   WORD color;

   color = ((inColor.red) & 0xF8) <<7;
   color |= ((inColor.green) & 0xF8) <<2;
   color |= ((inColor.blue) & 0xF8) >>3;

   return color;
}

//****************************************************************************/
BYTE cIMAGE_BUFFER8::colorToIndex(dcBGRA inColor)
{
   unsigned int curScore = 0xFFFFFFFF;
   BYTE nearestIndex = 0;

   for(WORD i=0; i<256; i++){
      unsigned int thisScore;

      thisScore = abs(m_packedDib.quads[i].rgbRed - inColor.red) +
        abs(m_packedDib.quads[i].rgbGreen - inColor.green) +
        abs(m_packedDib.quads[i].rgbBlue - inColor.blue);
      
      //see if exact match
      if(thisScore == 0){         
         nearestIndex = (BYTE)i;
         goto done;
      }

      if(thisScore < curScore){
         curScore = thisScore;
         nearestIndex = (BYTE)i;
      }
   }

done:
   return nearestIndex;
}

//****************************************************************************/
dcBGRA cIMAGE_BUFFER16::wordToBGRA(WORD word)
{
   dcBGRA retValue;

   WORD curValue;
   
   retValue.alpha = 0;
   
   curValue = word >>10;
   retValue.blue = (unsigned char)curValue;

   curValue = word >>5;
   curValue &= 0x1F;
   retValue.green = (unsigned char)curValue;

   curValue = word &0x1F;
   retValue.red = (unsigned char)curValue;
  
   return retValue;
}

