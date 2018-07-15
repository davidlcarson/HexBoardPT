//cApp.cpp
//Application class for BoardGame_180620
//180613 (dc)

#include "cApp.h"
#include "MyResource.h"
#include "cGreetWnd.h"

//local global
static cAPP* sg_pApp = NULL;  //we don't own
static cGREETWND* sg_pGreetWnd = NULL;

const wchar_t* kEXEC_DIRECTORY = L"../bin";
const wchar_t* kAPP_NAME = L"StarterApp";

//Forward dec
void EmptyLoop(LPARAM lParam);
void GreetLoop(LPARAM lParam);
void InitLoop(LPARAM lParam);

//loop pointer
void(*g_lp)(LPARAM) = EmptyLoop;

//Stuff that may come from .ini or save
//Size of ParWnd Client if Windowed
const int kDEF_PARCLIENT_WIDE = 800;
const int kDEF_PARCLIENT_TALL = 600;
const BOOL kWantWindowed = FALSE;

/*********************************************************************************/
cAPP::cAPP(void)
{
   //provide access to windows procedure
	sg_pApp = this;

   m_pParWnd = NULL;   
   m_pGameMaster = NULL;

   //Start Debug and change to /bin
   #ifdef _DEBUG
   wchar_t curDir[128];   
   GetCurrentDirectory(128, curDir);
   #endif

   BOOL bSuccess;
   bSuccess = SetCurrentDirectory(L"../bin");

   //start a debug log if in debug mode   
   InitDebug(kAPP_NAME);

   //now make sure directory change was successful
   MYASSERT(bSuccess == TRUE);

   return;
}

/******************************************************************************/
cAPP::~cAPP(void)
{  

	CLEAN_DELETE(m_pGameMaster);
	CLEAN_DELETE(m_pParWnd);

	//kill the debug routines
	debugsay(L"\nClosing main window\n");

	CloseDebug();

	return;
}

//Called from WinMain
/******************************************************************************/
void cAPP::InitGame(void)
{
   //debugsay(L"At cAPP::Initializing Game\n");

   m_bWantWindowed = kWantWindowed;

   /********** greet stuff **************/
   //Create the Greet window. Note this is a GDI routine. This may later become a logo cinematic, etc.
   //In theory, we could continue to initialize stuff (DirectX??) while this is up. Maybe via a second thread?
   //For now, there's really not much else to do.

   const wchar_t* kGREET_IMAGE_FILE = L"MeleeGreetSm.bmp";
   sg_pGreetWnd = new cGREETWND(kGREET_IMAGE_FILE);

   sg_pGreetWnd->RegisterGreetWnd();

   //Create the greet window
   sg_pGreetWnd->CreateGreetWnd();

   //Show the greet window (which starts a timer) and drop thru
   //to the message pump
   //Will close on mouse click, or in 8 seconds
   CenterWindow(sg_pGreetWnd->GethWnd());

   sg_pGreetWnd->Show(SW_NORMAL);
   //make it paint right now...
   UpdateWindow(sg_pGreetWnd->GethWnd());

   //--- end greet stuff

   //on return, message pump will start. Since we're in greet mode, set loop
   g_lp = GreetLoop;

   //Will call game loop to continue initialization
   return;
}

/************************************************************************************/
void cAPP::CreateParentWindow(void)
{
   //debugsay(L"Creating Parent Window\n");

   //Create a maximized parent window.
   m_pParWnd = new cPARWND(kAPP_NAME);
   m_pParWnd->Register();
   m_pParWnd->CreateWnd();

   if (m_bWantWindowed) {
      SizeForClient(m_pParWnd->GethWnd(), kDEF_PARCLIENT_WIDE, kDEF_PARCLIENT_TALL);
      m_pParWnd->Show(SW_SHOW);
      CenterWindow(m_pParWnd->GethWnd());
   }
   else {
      //we can still minimize using title bar
      m_pParWnd->Show(SW_MAXIMIZE);

      m_pParWnd->BlockMove(true);
      m_pParWnd->BlockSize(true);
   }

	//debugsay(L"Parent Window Showing - Calling new cGAME_MASTER\n");

	//Create GameMaster class here now that we have window
	 m_pGameMaster = new cGAME_MASTER(m_pParWnd->GethWnd());

   //ParWnd has already painted.
   //Game Master won't be painted until its MapWnd is created.

   return;
}

/************************************************************************************/
void EmptyLoop(LPARAM lParam)
{
   return;
}

/************************************************************************************/
void InitLoop(LPARAM)
{
   //debugsay(L"Calling for creation of Parent Window from InitLoop\n");

   sg_pApp->CreateParentWindow();

   g_lp = EmptyLoop;

   //returns to frame manager
   return;
}

/************************************************************************************/
void GreetLoop(LPARAM lParam)
{
   //-- nothing happens until greet times out ---_

   //Check if the greet window is done:
   if (sg_pGreetWnd->IsTimedOut() == true) {

      CLEAN_DELETE(sg_pGreetWnd);

      g_lp = InitLoop;
   }

   //else, do nothing...


   return;
}

//Non-message loop items
/*******************************************************************************/
void cAPP::FrameMgr(LPARAM lParam)
{
   //this is called by the message pump

   MYASSERT(g_lp != NULL);

   //re-direct to whichever loop is currently desired
   g_lp(lParam);

   //walked
   return;
}

/************************************************************************************/
LRESULT cAPP::EventHandler(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
   LRESULT lr = 0;

   switch (uMessage) {

   case WM_SIZE:


      break;

   case WM_ERASEBKGND:
      {
      RECT client;
      GetClientRect(hWnd, &client);
      HBRUSH hb = CreateSolidBrush(RGB(200, 200, 200));
      SelectObject((HDC)wParam, hb);

      Rectangle((HDC)wParam, 0, 0, client.right, client.bottom);

      DeleteObject(hb);

      lr = TRUE;
      }

	//This should only be called by the system. No calls from in our App to paint
	//..the parent window
   case WM_PAINT:
      {
      //debugsay(L"At WM_PAINT in cAPP\n")
      PAINTSTRUCT ps;
      
      BeginPaint(hWnd, &ps);
      SaveDC(ps.hdc);

      //skip the event handler.
	  if(m_pGameMaster)
		m_pGameMaster->OnPaint();
     
      RestoreDC(ps.hdc, -1); //-1 means most recent
      EndPaint(hWnd, &ps);

      lr = FALSE; //0 = I processed
      }
      break;


   //--- User Message --------------+
   case UM_MAPWND_LBUTTON_DOWN:
   {
	   //Send Pixel Coordinates!!!
	   SHORT x = GET_X_LPARAM(lParam);
	   SHORT y = GET_Y_LPARAM(lParam);

	   m_pGameMaster->OnMapWndLButtonDown(x, y);
	   
   }
	   break;
   case UM_MAPWND_MOUSEMOVE:
   {
	   SHORT x = GET_X_LPARAM(lParam);
	   SHORT y = GET_Y_LPARAM(lParam);

	   m_pGameMaster->OnMapWndMouseMove(x, y);
   }
	   break;
    default:
       lr = m_pParWnd->EventHandler(hWnd, uMessage, wParam, lParam);      
   }

   return lr;
}

//ParWnd process. Put here for access to sg_pApp.
//(Which also gives it access to cGAME_MASTER)
/************************************************************************************/
LRESULT CALLBACK ParWndProc(HWND hWnd, UINT uMessage,
	WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = FALSE;

	switch (uMessage) {

	case WM_NCCREATE:
      //No need to grab pointer. We use static
		lr = TRUE; //want to create window!
		break;

	default:
      lr = sg_pApp->EventHandler(hWnd, uMessage, wParam, lParam);
		break;
	}

	return lr;
}
