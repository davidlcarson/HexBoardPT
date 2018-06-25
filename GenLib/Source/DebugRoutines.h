#ifndef DEBUGROUTINES_H_
#define DEBUGROUTINES_H_
//06/14/06


#ifndef STRICT
#define STRICT
//#define _WIN32_WINNT 0x0501
#include <windows.h>
#endif

#include <stdio.h>

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define WFILE WIDE1(__FILE__)

const int kPATH_LEN = 200;

extern FILE* g_pDebugFile; //in DebugRoutines.cpp
extern wchar_t debugstring[];

//these are called in debug AND release modes:
void InitDebug(LPCWSTR pProjName);
void CloseDebug(void);

/////////////
// Debug file is generated if in _DEBUG mode, and deleted in Release Mode.
// If I want a debug file when in Release mode, set this to 1
////////////
#define WANT_RELEASE_DB 0

//Now set flag for generating a debug file based on Debug, Release, and want release...
#ifdef _DEBUG
#define WANT_DEBUGFILE 1
#elif WANT_RELEASE_DB
#define WANT_DEBUGFILE 1
#else
#define WANT_DEBUGFILE 0
#endif

///lastly, to force debug file off/on, just uncomment and set:
//define WANT_DEBUGFILE 1 0

#if WANT_DEBUGFILE

//forward reference proto:
void DebugHeader(const LPWSTR pProjName);
void Assert(bool b, const wchar_t* fileName, int lineNo);

#define debugHeader(x) DebugHeader(x)
#define debugsay(x) {fwprintf_s(g_pDebugFile, L"%s", x); fflush(g_pDebugFile);}
//#define debugprintf(x, y) sprintf(debugstring, x, y); debugsay(debugstring)
#define debugprintf(x, y) {swprintf_s(debugstring, 256, x, y); debugsay(debugstring);}
//#define MYASSERT(x) Assert(x, __FILE__, __LINE__)
#define MYASSERT(x) Assert(x, WFILE, __LINE__)

#else  //turn everything off if debug file not wanted

#define debugHeader(x)
#define debugsay(x)
#define debugprintf(x, y)
#define MYASSERT(x)

#endif  //want_debug

extern wchar_t g_outputStrings[4][kPATH_LEN];

#endif
