//StarterApp_Main.cpp
//D. Carson : 180624

//memory leak
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> //maps debug malloc to DEBUG version

#include "ProjDefs.h"

#ifdef _DEBUG
#pragma comment(lib, "GenLibd.lib")
#else
#pragma comment(lib, "GenLib.lib")
#endif

#pragma comment(lib, "Comctl32.lib")  //if using Status bar

#include "cApp.h"

/******************************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{    
   //Enable leak detection for debug builds
   #if defined(_DEBUG)
      //put in watch {,,ucrtbased.dll}_crtBreakAlloc 
      #define _CRTDBG_MAP_ALLOC
	   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   #endif
   
   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);
   UNREFERENCED_PARAMETER(nCmdShow);
   UNREFERENCED_PARAMETER(hInstance);

   //Create the Game Ap
   cAPP* pGameApp = new cAPP();
   
   //Want Windowed?
   pGameApp->InitGame();
     
   // Start the message loop
   MSG msg;     

   bool bContinue = true;

   while(bContinue){

      //Check if a message is waiting for processing
      if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){

         //is it quit?
         if(msg.message == WM_QUIT)
            //exit
            break;

         // Modify key messages
         TranslateMessage( &msg );

         // Send the message to WndProc() the event handler
         DispatchMessage( &msg );
      }
      else{         
         //call the current loop pointer
         pGameApp->FrameMgr(NULL);
      }
   }

   CLEAN_DELETE(pGameApp);

#ifdef _DEBUG
   //call for the leak report
   _CrtDumpMemoryLeaks();
#endif
   
   return 1;
}



