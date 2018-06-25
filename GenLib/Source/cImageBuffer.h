#pragma once

#include "globdefs.h"

//windows 24bit pixel
typedef struct tag_dcBGR{
   BYTE blue;
   BYTE green;
   BYTE red;
}dcBGR;

//windows 32bit pixel
typedef struct tag_dcARGB{
   BYTE blue;
   BYTE green;
   BYTE red;
   BYTE alpha;   
}dcBGRA;

//----------------------------------------------------------------------+
class cIMAGE_BUFFER{
 protected:

   struct{
     BITMAPINFOHEADER bmih;
     RGBQUAD quads[256];
   }m_packedDib;

   BITMAPFILEHEADER m_bmfh;      
   
   DWORD m_paddedWide;     //image width in bytes incl padding
   char* m_pBits;
   
   //we don't own
   const BYTE* m_pFilename; //our source file (if is one)
      
   //-- protected methods --+
   void readFile(const wchar_t* pFilename);   

   virtual cIMAGE_BUFFER* convertBuffer(WORD bpp)=0;
   virtual void scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight)=0;
   virtual void copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY)=0;

 public:
   cIMAGE_BUFFER(void); //create 1x1 buffer   
   virtual ~cIMAGE_BUFFER(void);

   //pure virtual methods
   virtual void inline PlotPixel(int x, int y)=0;
   virtual dcBGRA GetPixel(WORD x, WORD y)=0;

   virtual void ClearBuffer(void)=0;
   virtual void DrawLine(int x1, int y1, int x2, int y2)=0;
   virtual void DrawHorzLine(int x1, int y1, int width)=0;
   virtual void DrawVertLine(int x1, int y1, int height)=0;

   virtual dcBGRA GetClearColor(void)=0;
   virtual dcBGRA GetDrawColor(void)=0;
   virtual void SetDrawColor(COLORREF cr)=0;
   virtual void SetClearColor(COLORREF cr)=0;

   //virtual methods
   virtual void DrawRect(int x1, int y1, int width, int height){
      DrawHorzLine(x1, y1, width);
      DrawVertLine(x1, y1, height);
      DrawHorzLine(x1, y1+height, width);
      DrawVertLine(x1+width, y1, height);}               

   virtual void StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int width, int height, int srcX, int srcY, int srcWidth, int srcHeight)=0;

   //common
   void SaveBMP(const char* pFilename);
   
   //inlines
   WORD GetPixWide(void){ return (WORD)m_packedDib.bmih.biWidth;}
   WORD GetRowsTall(void){ return (WORD)-m_packedDib.bmih.biHeight;}
   DWORD GetPaddedWide(void){ return m_paddedWide;}      
   const BYTE* GetFileName(void){ return m_pFilename;}
   WORD GetBpp(void){ return (WORD)m_packedDib.bmih.biBitCount;}

   void* GetpBits(void){ return m_pBits;}   
   RGBQUAD* GetpQuads(void){ return (RGBQUAD*)&m_packedDib.quads;}

   BITMAPINFO* GetpBmi(void){ return (BITMAPINFO*)&m_packedDib;}
   BITMAPINFOHEADER* GetpBmih(void) { return &m_packedDib.bmih; }
};

//****************************************************************************/
class cIMAGE_BUFFER24 : public cIMAGE_BUFFER{
 private:
   dcBGR m_drawColor;
   dcBGR m_clearColor;   

   //---private methods
   cIMAGE_BUFFER* convertBuffer(WORD bpp);
   void scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight);
   void copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY);

 public:
   cIMAGE_BUFFER24(WORD width, WORD height);
   cIMAGE_BUFFER24(const wchar_t* pFilename);
   ~cIMAGE_BUFFER24(void);

   void inline PlotPixel(int x, int y);   
   void DrawLine(int x1, int y1, int x2, int y2);
   void DrawHorzLine(int x1, int y1, int width);   
   void DrawVertLine(int x1, int y1, int height);   
  
   dcBGRA GetClearColor(void);   
   dcBGRA GetPixel(WORD x, WORD y);
   dcBGRA GetDrawColor(void);
   void SetDrawColor(COLORREF cr);
   void SetClearColor(COLORREF cr);

   void ClearBuffer(void);
   
   void StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int width, int height, int srcX, int srcY, int srcWidth, int srcHeight);
};

//****************************************************************************/
class cIMAGE_BUFFER8 : public cIMAGE_BUFFER{
 private:      
   unsigned char m_drawColor;
   unsigned char m_clearColor;

   //private methods   
   BYTE colorToIndex(dcBGRA inColor);
   cIMAGE_BUFFER* convertBuffer(WORD bpp);
   void scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight);
   void copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY);

 public:
   cIMAGE_BUFFER8(WORD width, WORD height);
   cIMAGE_BUFFER8(const wchar_t* pFilename);
   //~cIMAGE_BUFFER8(void);
      
   void DrawLine(int x1, int y1, int x2, int y2);
   void DrawHorzLine(int x1, int y1, int width);
   void DrawVertLine(int x1, int y1, int height);

   void inline PlotPixel(int x, int y);   
   dcBGRA GetPixel(WORD x, WORD y);

   void ClearBuffer(void);

   void SetDrawColor(COLORREF cr);
   dcBGRA GetDrawColor(void);
   void SetClearColor(COLORREF cr);
   dcBGRA GetClearColor(void);   

   void StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int width, int height, int srcX, int srcY, int srcWidth, int srcHeight);
};

//****************************************************************************/
class cIMAGE_BUFFER16 : public cIMAGE_BUFFER{
 private:   
    WORD m_drawColor;
    WORD m_clearColor;

   //private methods:   
   cIMAGE_BUFFER* convertBuffer(WORD bpp);
   void scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight);
   void copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY);
   WORD colorToWord(dcBGRA inColor);
   dcBGRA wordToBGRA(WORD word);

 public:
   cIMAGE_BUFFER16(WORD width, WORD height);
   cIMAGE_BUFFER16(const wchar_t* pFilename);
   //~cIMAGE_BUFFER16(void);

   void inline PlotPixel(int x, int y);   
   dcBGRA GetPixel(WORD x, WORD y);

   void DrawLine(int x1, int y1, int x2, int y2);
   void DrawHorzLine(int x1, int y1, int width);
   void DrawVertLine(int x1, int y1, int height);   
 
   void ClearBuffer(void);

   void SetDrawColor(COLORREF cr);
   dcBGRA GetDrawColor(void);
   void SetClearColor(COLORREF cr);
   dcBGRA GetClearColor(void);   

   void StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int width, int height, int srcX, int srcY, int srcWidth, int srcHeight);
};

//****************************************************************************/
class cIMAGE_BUFFER32 : public cIMAGE_BUFFER{
 private:    
   dcBGRA m_drawColor;
   dcBGRA m_clearColor;

   //private methods:   
   cIMAGE_BUFFER* convertBuffer(WORD bpp);
   void scaleBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight);
   void copyBuffer(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int dstWidth, int dstHeight, int srcX, int srcY);

 public:
   cIMAGE_BUFFER32(WORD width, WORD height);
   cIMAGE_BUFFER32(const wchar_t* pFilename);
   //~cIMAGE_BUFFER32(void);

   void inline PlotPixel(int x, int y);   
   dcBGRA GetPixel(WORD x, WORD y);

   void DrawLine(int x1, int y1, int x2, int y2);
   void DrawHorzLine(int x1, int y1, int width);
   void DrawVertLine(int x1, int y1, int height);
   
   void ClearBuffer(void);

   void SetDrawColor(COLORREF cr);
   dcBGRA GetDrawColor(void);
   void SetClearColor(COLORREF cr);
   dcBGRA GetClearColor(void);   

   void StretchBlt(cIMAGE_BUFFER* pDestBuffer, int dstX, int dstY,
      int width, int height, int srcX, int srcY, int srcWidth, int srcHeight);
};

//----------------------------------------------------------------------+
cIMAGE_BUFFER* CreateImageBuffer(const wchar_t* pFilename);


