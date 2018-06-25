//DebugRoutines.cpp
//06/14/06

#include "DebugRoutines.h"

//globaly available
wchar_t debugstring[256];

wchar_t g_outputStrings[4][kPATH_LEN] = {0}; //put a string on the screen

FILE* g_pDebugFile = NULL;

/**********************************************************************/
void DebugHeader(const wchar_t* projName)
{
   SYSTEMTIME systemTime;

   GetLocalTime(&systemTime);
   swprintf_s(debugstring, sizeof(debugstring), L"+--- %s : %d/%d/%d : %02d:%02d ----+\n", projName,
      systemTime.wMonth, systemTime.wDay, systemTime.wYear, systemTime.wHour, systemTime.wMinute);

   debugsay(debugstring);
#ifdef _DEBUG
   debugsay(L" --- Debug Version --\n");
#else
   debugsay(L" --- Release Version --\n");
#endif

   return;
}

/**********************************************************************/
void Assert(bool b, wchar_t const* fileName, int lineNo)
{
   size_t len;
   
   if(!b){      
      if(g_pDebugFile == NULL){
         MessageBox(NULL, L"Hey! No Global Debug File! Probably bad path.", L"No Global Debug File", MB_OK);
         exit(0);
      }

      len = wcslen(fileName);
      swprintf_s(debugstring, sizeof(debugstring), L"Assertion Failed: %s, line %d\n", fileName, lineNo);
      debugsay(debugstring); 

      fclose(g_pDebugFile);

      int count;
      do{
         count = ShowCursor(TRUE);
      }while (count < 0);

      MessageBox(NULL, debugstring, L"Assertion Failed", MB_OK);
      //exit(1);
      count=count;
   }

   return;
}

/**********************************************************************/
void InitDebug(const wchar_t* pProjName)
{
   //build the name of the debug file
   wcscpy_s(debugstring, sizeof(debugstring), pProjName);
   wcscat_s(debugstring, sizeof(debugstring), L"_db.txt");

   if(g_pDebugFile == NULL)
      _wfopen_s(&g_pDebugFile, (const wchar_t*)debugstring, L"wt");

#if WANT_DEBUGFILE
   
   debugHeader(pProjName);

#else
   //don't want a file: make sure it's empty
   //(tho it will still exist!)
   fclose(g_pDebugFile);
   g_pDebugFile = NULL;
#endif

   return;
}

/**********************************************************************/
void CloseDebug(void)
{
   if(g_pDebugFile){
      debugsay(L"Closing Debug File\n");
      fclose(g_pDebugFile);
      g_pDebugFile = NULL;
   }

   return;
}
