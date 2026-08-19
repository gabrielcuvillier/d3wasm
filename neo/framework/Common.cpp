/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include <SDL.h>

#include "sys/platform.h"
#include "idlib/containers/HashTable.h"
#include "idlib/LangDict.h"
#include "idlib/MapFile.h"
#include "cm/CollisionModel.h"
#include "framework/async/AsyncNetwork.h"
#include "framework/async/NetworkSystem.h"
#include "framework/BuildVersion.h"
#include "framework/Licensee.h"
#include "framework/Console.h"
#include "framework/Session.h"
#include "framework/Game.h"
#include "framework/KeyInput.h"
#include "framework/EventLoop.h"
#include "renderer/Image.h"
#include "renderer/ModelManager.h"
#include "renderer/RenderSystem.h"
#include "tools/compilers/aas/AASFileManager.h"

#include "framework/Common.h"

#include "GameCallbacks_local.h"
#include "Session_local.h" // DG: For FT_IsDemo/isDemo() hack

#ifdef __EMSCRIPTEN__
#include "emscripten.h"   // emscripten_sleep_with_yield, EM_ASM
#endif

#define  MAX_PRINT_MSG_SIZE  4096
#define MAX_WARNING_LIST  256

typedef enum {
  ERP_NONE,
  ERP_FATAL,                        // exit the entire game with a popup window
  ERP_DROP,                        // print to console and disconnect from game
  ERP_DISCONNECT                    // don't kill server
} errorParm_t;

#if defined( _DEBUG )
#define BUILD_DEBUG "-debug"
#else
#define BUILD_DEBUG ""
#endif

struct version_s {
  version_s(void) {
    sprintf(string, "%s.%d%s %s-%s %s %s", ENGINE_VERSION, BUILD_NUMBER, BUILD_DEBUG, BUILD_OS, BUILD_CPU, __DATE__,
            __TIME__);
  }

  char string[256];
} version;

idCVar com_version("si_version", version.string, CVAR_SYSTEM | CVAR_ROM | CVAR_SERVERINFO, "engine version");
idCVar com_skipRenderer("com_skipRenderer", "0", EM_CVAR_FLAGS(CVAR_BOOL | CVAR_SYSTEM), "skip the renderer completely");
idCVar com_machineSpec("com_machineSpec", EM_CVAR_VAL("-1", "3"), EM_CVAR_FLAGS(CVAR_INTEGER | CVAR_ARCHIVE | CVAR_SYSTEM),
  "hardware classification, -1 = not detected, 0 = low quality, 1 = medium quality, 2 = high quality, 3 = ultra quality");

idCVar com_purgeAll("com_purgeAll", "0", CVAR_BOOL | CVAR_ARCHIVE | CVAR_SYSTEM,
                    "purge everything between level loads");
idCVar com_preciseTic("com_preciseTic", "1", CVAR_BOOL | CVAR_SYSTEM, "run one game tick every async thread update");
idCVar com_asyncInput("com_asyncInput", "0", EM_CVAR_FLAGS(CVAR_BOOL | CVAR_SYSTEM), "sample input from the async thread");
idCVar com_asyncSound("com_asyncSound", "0", EM_CVAR_FLAGS(CVAR_INTEGER | CVAR_SYSTEM),
                      "0: mix sound inline, 1: memory mapped async mix, 2: callback mixing, 3: write async mix");
idCVar com_forceGenericSIMD("com_forceGenericSIMD", EM_CVAR_VAL("0", "1"), EM_CVAR_FLAGS(CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT),
                            "force generic platform independent SIMD");

idCVar com_developer("developer", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "developer mode");
idCVar com_allowConsole("com_allowConsole", "0", EM_CVAR_FLAGS(CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT),
                        "allow toggling console with the tilde key");
idCVar com_speeds("com_speeds", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT, "show engine timings");
idCVar com_showFPS("com_showFPS", "1", CVAR_BOOL | CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_NOCHEAT,
                   "show frames rendered per second");
idCVar com_showMemoryUsage("com_showMemoryUsage", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT,
                           "show total and per frame memory usage");
idCVar com_showAsyncStats("com_showAsyncStats", "0", EM_CVAR_FLAGS(CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT),
                          "show async network stats");
idCVar com_showSoundDecoders("com_showSoundDecoders", "0", CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT,
                             "show sound decoders");
idCVar com_timestampPrints("com_timestampPrints", "0", CVAR_SYSTEM,
                           "print time with each console print, 1 = msec, 2 = sec", 0, 2,
                           idCmdSystem::ArgCompletion_Integer < 0, 2 > );
idCVar com_timescale("timescale", "1", CVAR_SYSTEM | CVAR_FLOAT, "scales the time", 0.1f, 10.0f);
idCVar com_updateLoadSize("com_updateLoadSize", "0", EM_CVAR_FLAGS(CVAR_BOOL | CVAR_SYSTEM | CVAR_NOCHEAT),
                          "update the load size after loading a map");

idCVar com_product_lang_ext("com_product_lang_ext", "1", EM_CVAR_FLAGS(CVAR_INTEGER | CVAR_SYSTEM | CVAR_ARCHIVE),
                            "Extension to use when creating language files.");

// com_speeds times
int time_gameFrame;
int time_gameDraw;
int time_frontend;            // renderSystem frontend time
int time_backend;            // renderSystem backend time

int com_frameTime;            // time for the current frame in milliseconds
int com_frameNumber;        // variable frame number
/*volatile */int com_ticNumber;            // 60 hz tics

#ifdef __DOOM_DLL__
idGame *		game = NULL;
idGameEdit *	gameEdit = NULL;
#endif

// writes si_version to the config file - in a kinda obfuscated way
//#define ID_WRITE_VERSION

class idCommonLocal : public idCommon {
public:
  idCommonLocal(void);

  virtual void Init(int argc, char** argv);

  virtual void Shutdown(void);

  virtual void Quit(void);

  virtual bool IsInitialized(void) const;

  virtual void Frame(void);

  virtual void GUIFrame(bool execCmd, bool network);

  virtual void ForceRefreshScreen(bool vsync = true);

  virtual void Async(void);

  virtual void StartupVariable(const char* match, bool once);

  virtual void WriteConfigToFile(const char* filename);

#ifndef __EMSCRIPTEN__
  virtual void WriteFlaggedCVarsToFile(const char* filename, int flags, const char* setCmd);
#endif

  virtual void BeginRedirect(char* buffer, int buffersize, void (* flush)(const char*));

  virtual void EndRedirect(void);

  virtual void SetRefreshOnPrint(bool set);

  virtual void Printf(const char* fmt, ...)

  id_attribute ((format(printf, 2, 3)));

  virtual void VPrintf(const char* fmt, va_list arg);

  virtual void DPrintf(const char* fmt, ...)

  id_attribute ((format(printf, 2, 3)));

  virtual void Warning(const char* fmt, ...)

  id_attribute ((format(printf, 2, 3)));

  virtual void DWarning(const char* fmt, ...)

  id_attribute ((format(printf, 2, 3)));

  virtual void PrintWarnings(void);

  virtual void ClearWarnings(const char* reason);

  virtual void Error(const char* fmt, ...)

  id_attribute ((format(printf, 2, 3)));

  virtual void FatalError(const char* fmt, ...)

  id_attribute ((format(printf, 2, 3)));

  virtual const idLangDict* GetLanguageDict(void);

  virtual const char* KeysFromBinding(const char* bind);

  virtual const char* BindingFromKey(const char* key);

  virtual int ButtonState(int key);

  virtual int KeyState(int key);

  virtual void TouchEngineData();

  // DG: hack to allow adding callbacks and exporting additional functions without breaking the game ABI
  //     see Common.h for longer explanation...

  // returns true if setting the callback was successful, else false
  // When a game DLL is unloaded the callbacks are automatically removed from the Engine
  // so you usually don't have to worry about that; but you can call this with cb = NULL
  // and userArg = NULL to remove a callback manually (e.g. if userArg refers to an object you deleted)
  virtual bool SetCallback(idCommon::CallbackType cbt, idCommon::FunctionPointer cb, void* userArg);

  // returns true if that function is available in this version of dhewm3
  // *out_fnptr will be the function (you'll have to cast it probably)
  // *out_userArg will be an argument you have to pass to the function, if appropriate (else NULL)
  // NOTE: this doesn't do anything yet, but allows to add ugly mod-specific hacks without breaking the Game interface
  virtual bool
  GetAdditionalFunction(idCommon::FunctionType ft, idCommon::FunctionPointer* out_fnptr, void** out_userArg);

  virtual const idMaterial*	GetCharSetMaterial() { return charsetMaterial; }
  virtual const idMaterial*	GetSplashscreenMaterial() { return splashMaterial; }

#ifdef __EMSCRIPTEN__
  virtual void				SetMediaLoadEnabled( bool enabled ) { com_enableMediaLoad = enabled; }
  virtual bool				IsMediaLoadEnabled( void ) const { return com_enableMediaLoad; }
#endif

  // DG end

  void InitGame(void);

  void ShutdownGame(bool reloading);

  // localization
  void InitLanguageDict(void);

#ifndef __EMSCRIPTEN__
  void LocalizeGui(const char* fileName, idLangDict& langDict);

  void LocalizeMapData(const char* fileName, idLangDict& langDict);

  void LocalizeSpecificMapData(const char* fileName, idLangDict& langDict, const idLangDict& replaceArgs);
#endif

  void SetMachineSpec(void);

private:
  void InitCommands(void);

  void InitRenderSystem(void);

  void InitSIMD(void);

  bool AddStartupCommands(void);

  void ParseCommandLine(int argc, char** argv);

  void ClearCommandLine(void);

  bool SafeMode(void);

  void WriteConfiguration(void);

  void DumpWarnings(void);

  void SingleAsyncTic(void);

  void LoadGameDLL(void);

#ifndef __EMSCRIPTEN__
  void LoadGameDLLbyName(const char* dll, idStr& s);
#endif

  void UnloadGameDLL(void);

  void PrintLoadingMessage(const char* msg);

  void FilterLangList(idStrList* list, idStr lang);

  bool com_fullyInitialized;
  bool com_refreshOnPrint;        // update the screen every print for dmap
  int com_errorEntered;        // 0, ERP_DROP, etc

  char errorMessage[MAX_PRINT_MSG_SIZE];

  char* rd_buffer;
  int rd_buffersize;

  void (* rd_flush)(const char* buffer);

  idStr warningCaption;
  idStrList warningList;
  idStrList errorList;

  uintptr_t gameDLL;

  idLangDict languageDict;

  const idMaterial* splashMaterial;
  const idMaterial* charsetMaterial;

#ifdef __EMSCRIPTEN__
  bool com_enableMediaLoad = true;
#endif

#ifdef ID_WRITE_VERSION
  idCompressor *				config_compressor;
#endif

#ifdef NOMT
  // No async_timer in Single-Threaded mode. Tic updates are done in the main thread.
#else
  SDL_TimerID async_timer;
#endif
};

idCommonLocal commonLocal;
idCommon* common = &commonLocal;

/*
==================
idCommonLocal::idCommonLocal
==================
*/
idCommonLocal::idCommonLocal(void) {
  com_fullyInitialized = false;
  com_refreshOnPrint = false;
  com_errorEntered = 0;

  strcpy(errorMessage, "");

  rd_buffer = NULL;
  rd_buffersize = 0;
  rd_flush = NULL;

  gameDLL = 0;

  splashMaterial = NULL;
  charsetMaterial = NULL;

  com_fullyInitialized = false;
#ifdef __EMSCRIPTEN__
  com_enableMediaLoad = true;
#endif

#ifdef ID_WRITE_VERSION
  config_compressor = NULL;
#endif

#ifdef NOMT
  // No async_timer in Single-Threaded mode. Tic updates are done in the main thread.
#else
  async_timer = 0;
#endif
}

/*
==================
idCommonLocal::BeginRedirect
==================
*/
void idCommonLocal::BeginRedirect(char* buffer, int buffersize, void (* flush)(const char*)) {
  if ( !buffer || !buffersize || !flush ) {
    return;
  }
  rd_buffer = buffer;
  rd_buffersize = buffersize;
  rd_flush = flush;

  *rd_buffer = 0;
}

/*
==================
idCommonLocal::EndRedirect
==================
*/
void idCommonLocal::EndRedirect(void) {
  if ( rd_flush && rd_buffer[0] ) {
    rd_flush(rd_buffer);
  }

  rd_buffer = NULL;
  rd_buffersize = 0;
  rd_flush = NULL;
}

/*
==================
idCommonLocal::SetRefreshOnPrint
==================
*/
void idCommonLocal::SetRefreshOnPrint(bool set) {
  com_refreshOnPrint = set;
}

/*
==================
idCommonLocal::VPrintf

A raw string should NEVER be passed as fmt, because of "%f" type crashes.
==================
*/
void idCommonLocal::VPrintf(const char* fmt, va_list args) {
  char msg[MAX_PRINT_MSG_SIZE];
  int timeLength;

  // if the cvar system is not initialized
  if ( !cvarSystem->IsInitialized()) {
    return;
  }

  // optionally put a timestamp at the beginning of each print,
  // so we can see how long different init sections are taking
  if ( com_timestampPrints.GetInteger()) {
    int t = Sys_Milliseconds();
    if ( com_timestampPrints.GetInteger() == 1 ) {
      t /= 1000;
    }
    sprintf(msg, "[%i]", t);
    timeLength = strlen(msg);
  }
  else {
    timeLength = 0;
  }

  // don't overflow
  if ( idStr::vsnPrintf(msg + timeLength, MAX_PRINT_MSG_SIZE - timeLength - 1, fmt, args) < 0 ) {
    msg[sizeof(msg) - 2] = '\n';
    msg[sizeof(msg) - 1] = '\0'; // avoid output garbling
    Sys_Printf("idCommon::VPrintf: truncated to %zd characters\n", strlen(msg) - 1);
  }

  if ( rd_buffer ) {
    if ((int) ( strlen(msg) + strlen(rd_buffer)) > ( rd_buffersize - 1 )) {
      rd_flush(rd_buffer);
      *rd_buffer = 0;
    }
    strcat(rd_buffer, msg);
    return;
  }

  // echo to console buffer
  console->Print(msg);

  // remove any color codes
  idStr::RemoveColors(msg);

  // echo to dedicated console and early console
  Sys_Printf("%s", msg);

  // print to script debugger server
  // DebuggerServerPrint( msg );


  // don't trigger any updates if we are in the process of doing a fatal error
  if ( com_errorEntered != ERP_FATAL ) {
    // update the console if we are in a long-running command, like dmap

#ifndef __EMSCRIPTEN__
    if ( com_refreshOnPrint ) {
      session->UpdateScreen();
    }

    // let session redraw the animated loading screen if necessary
    session->PacifierUpdate();
#endif
  }

}

/*
==================
idCommonLocal::Printf

Both client and server can use this, and it will output to the appropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
==================
*/
void idCommonLocal::Printf(const char* fmt, ...) {
  va_list argptr;
  va_start(argptr, fmt);
  VPrintf(fmt, argptr);
  va_end(argptr);
}

/*
==================
idCommonLocal::DPrintf

prints message that only shows up if the "developer" cvar is set
==================
*/
void idCommonLocal::DPrintf(const char* fmt, ...) {
  va_list argptr;
  char msg[MAX_PRINT_MSG_SIZE];

  if ( !cvarSystem->IsInitialized() || !com_developer.GetBool()) {
    return;            // don't confuse non-developers with techie stuff...
  }

  va_start(argptr, fmt);
  idStr::vsnPrintf(msg, sizeof(msg), fmt, argptr);
  va_end(argptr);
  msg[sizeof(msg) - 1] = '\0';

  // never refresh the screen, which could cause reentrency problems
  bool temp = com_refreshOnPrint;
  com_refreshOnPrint = false;

  Printf(S_COLOR_RED
  "%s", msg );

  com_refreshOnPrint = temp;
}

/*
==================
idCommonLocal::DWarning

prints warning message in yellow that only shows up if the "developer" cvar is set
==================
*/
void idCommonLocal::DWarning(const char* fmt, ...) {
  va_list argptr;
  char msg[MAX_PRINT_MSG_SIZE];

  if ( !com_developer.GetBool()) {
    return;            // don't confuse non-developers with techie stuff...
  }

  va_start(argptr, fmt);
  idStr::vsnPrintf(msg, sizeof(msg), fmt, argptr);
  va_end(argptr);
  msg[sizeof(msg) - 1] = '\0';

  Printf(S_COLOR_YELLOW
  "WARNING: %s\n", msg );
}

/*
==================
idCommonLocal::Warning

prints WARNING %s and adds the warning message to a queue to be printed later on
==================
*/
void idCommonLocal::Warning(const char* fmt, ...) {
  va_list argptr;
  char msg[MAX_PRINT_MSG_SIZE];

  va_start(argptr, fmt);
  idStr::vsnPrintf(msg, sizeof(msg), fmt, argptr);
  va_end(argptr);
  msg[sizeof(msg) - 1] = 0;

  Printf(S_COLOR_YELLOW
  "WARNING: "
  S_COLOR_RED
  "%s\n", msg );

  if ( warningList.Num() < MAX_WARNING_LIST ) {
    warningList.AddUnique(msg);
  }
}

/*
==================
idCommonLocal::PrintWarnings
==================
*/
void idCommonLocal::PrintWarnings(void) {
  int i;

  if ( !warningList.Num()) {
    return;
  }

  warningList.Sort();

  Printf("----- Warnings -----\n");
  Printf("during %s...\n", warningCaption.c_str());

  for ( i = 0; i < warningList.Num(); i++ ) {
    Printf(S_COLOR_YELLOW
    "WARNING: "
    S_COLOR_RED
    "%s\n", warningList[i].c_str());
  }
  if ( warningList.Num()) {
    if ( warningList.Num() >= MAX_WARNING_LIST ) {
      Printf("more than %d warnings\n", MAX_WARNING_LIST);
    }
    else {
      Printf("%d warnings\n", warningList.Num());
    }
  }
}

/*
==================
idCommonLocal::ClearWarnings
==================
*/
void idCommonLocal::ClearWarnings(const char* reason) {
  warningCaption = reason;
  warningList.Clear();
}

#ifndef __EMSCRIPTEN__
/*
==================
idCommonLocal::DumpWarnings
==================
*/
void idCommonLocal::DumpWarnings(void) {
  int i;
  idFile* warningFile;

  if ( !warningList.Num()) {
    return;
  }

  warningFile = fileSystem->OpenFileWrite("warnings.txt", "fs_savepath");
  if ( warningFile ) {

    warningFile->Printf("----- Warnings -----\n\n");
    warningFile->Printf("during %s...\n", warningCaption.c_str());
    warningList.Sort();
    for ( i = 0; i < warningList.Num(); i++ ) {
      warningList[i].RemoveColors();
      warningFile->Printf("WARNING: %s\n", warningList[i].c_str());
    }
    if ( warningList.Num() >= MAX_WARNING_LIST ) {
      warningFile->Printf("\nmore than %d warnings!\n", MAX_WARNING_LIST);
    }
    else {
      warningFile->Printf("\n%d warnings.\n", warningList.Num());
    }

    warningFile->Printf("\n\n----- Errors -----\n\n");
    errorList.Sort();
    for ( i = 0; i < errorList.Num(); i++ ) {
      errorList[i].RemoveColors();
      warningFile->Printf("ERROR: %s", errorList[i].c_str());
    }

    warningFile->ForceFlush();

    fileSystem->CloseFile(warningFile);

  }
}
#endif

/*
==================
idCommonLocal::Error
==================
*/
void idCommonLocal::Error(const char* fmt, ...) {
  va_list argptr;
  static int lastErrorTime;
  static int errorCount;
  int currentTime;

  int code = ERP_DROP;

  // always turn this off after an error
  com_refreshOnPrint = false;

  // when we are running automated scripts, make sure we
  // know if anything failed
#ifndef __EMSCRIPTEN__
  if ( cvarSystem->GetCVarInteger("fs_copyfiles")) {
    code = ERP_FATAL;
  }
#endif
  // if we don't have GL running, make it a fatal error
  if ( !renderSystem->IsOpenGLRunning()) {
    code = ERP_FATAL;
  }

  // if we got a recursive error, make it fatal
  if ( com_errorEntered ) {
    // if we are recursively erroring while exiting
    // from a fatal error, just kill the entire
    // process immediately, which will prevent a
    // full screen rendering window covering the
    // error dialog
    if ( com_errorEntered == ERP_FATAL ) {
      Sys_Quit();
    }
    code = ERP_FATAL;
  }

  // if we are getting a solid stream of ERP_DROP, do an ERP_FATAL
  currentTime = Sys_Milliseconds();
  if ( currentTime - lastErrorTime < 100 ) {
    if ( ++errorCount > 3 ) {
      code = ERP_FATAL;
    }
  }
  else {
    errorCount = 0;
  }
  lastErrorTime = currentTime;

  com_errorEntered = code;

  va_start(argptr, fmt);
  idStr::vsnPrintf(errorMessage, sizeof(errorMessage), fmt, argptr);
  va_end(argptr);
  errorMessage[sizeof(errorMessage) - 1] = '\0';

  // copy the error message to the clip board
  Sys_SetClipboardData(errorMessage);

  // add the message to the error list
  errorList.AddUnique(errorMessage);

  session->Stop();

  if ( code == ERP_DISCONNECT ) {
    com_errorEntered = 0;
    return;
    //throw idException(errorMessage);
    // The gui editor doesnt want thing to com_error so it handles exceptions instead
  }
  else if ( code == ERP_DROP ) {
    Printf("********************\nERROR: %s\n********************\n", errorMessage);
    com_errorEntered = 0;
    return;
    //throw idException(errorMessage);
  }
  else {
    Printf("********************\nERROR: %s\n********************\n", errorMessage);
  }

  if ( cvarSystem->GetCVarBool("r_fullscreen")) {
    cmdSystem->BufferCommandText(CMD_EXEC_NOW, "vid_restart partial windowed\n");
  }

  Shutdown();

  Sys_Error("%s", errorMessage);
}

/*
==================
idCommonLocal::FatalError

Dump out of the game to a system dialog
==================
*/
void idCommonLocal::FatalError(const char* fmt, ...) {
  va_list argptr;

  // if we got a recursive error, make it fatal
  if ( com_errorEntered ) {
    // if we are recursively erroring while exiting
    // from a fatal error, just kill the entire
    // process immediately, which will prevent a
    // full screen rendering window covering the
    // error dialog

    Sys_Printf("FATAL: recursed fatal error:\n%s\n", errorMessage);

    va_start(argptr, fmt);
    idStr::vsnPrintf(errorMessage, sizeof(errorMessage), fmt, argptr);
    va_end(argptr);
    errorMessage[sizeof(errorMessage) - 1] = '\0';

    Sys_Printf("%s\n", errorMessage);

    // write the console to a log file?
    Sys_Quit();
  }
  com_errorEntered = ERP_FATAL;

  va_start(argptr, fmt);
  idStr::vsnPrintf(errorMessage, sizeof(errorMessage), fmt, argptr);
  va_end(argptr);
  errorMessage[sizeof(errorMessage) - 1] = '\0';

  if ( cvarSystem->GetCVarBool("r_fullscreen")) {
    cmdSystem->BufferCommandText(CMD_EXEC_NOW, "vid_restart partial windowed\n");
  }

  Sys_Printf("shutting down: %s\n", errorMessage);

  Shutdown();

  Sys_Error("%s", errorMessage);
}

/*
==================
idCommonLocal::Quit
==================
*/
void idCommonLocal::Quit(void) {

  // don't try to shutdown if we are in a recursive error
  if ( !com_errorEntered ) {
    Shutdown();
  }

  Sys_Printf("Quitting program...\n");

  Sys_Quit();
}


/*
============================================================================

COMMAND LINE FUNCTIONS

+ characters separate the commandLine string into multiple console
command lines.

All of these are valid:

doom +set test blah +map test
doom set test blah+map test
doom set test blah + map test

============================================================================
*/

#define        MAX_CONSOLE_LINES    32
int com_numConsoleLines;
idCmdArgs com_consoleLines[MAX_CONSOLE_LINES];

/*
==================
idCommonLocal::ParseCommandLine
==================
*/
void idCommonLocal::ParseCommandLine(int argc, char** argv) {
  int i;

  com_numConsoleLines = 0;
  // API says no program path
  for ( i = 0; i < argc; i++ ) {
    if ( argv[i][0] == '+' ) {
      com_numConsoleLines++;
      com_consoleLines[com_numConsoleLines - 1].AppendArg(argv[i] + 1);
    }
    else {
      if ( !com_numConsoleLines ) {
        com_numConsoleLines++;
      }
      com_consoleLines[com_numConsoleLines - 1].AppendArg(argv[i]);
    }
  }
}

/*
==================
idCommonLocal::ClearCommandLine
==================
*/
void idCommonLocal::ClearCommandLine(void) {
  com_numConsoleLines = 0;
}

/*
==================
idCommonLocal::SafeMode

Check for "safe" on the command line, which will
skip loading of config file (DoomConfig.cfg)
==================
*/
bool idCommonLocal::SafeMode(void) {
  int i;

  for ( i = 0; i < com_numConsoleLines; i++ ) {
    if ( !idStr::Icmp(com_consoleLines[i].Argv(0), "safe")
         || !idStr::Icmp(com_consoleLines[i].Argv(0), "cvar_restart")) {
      com_consoleLines[i].Clear();
      return true;
    }
  }
  return false;
}

/*
==================
idCommonLocal::StartupVariable

Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets should
be after execing the config and default.
==================
*/
void idCommonLocal::StartupVariable(const char* match, bool once) {
  int i;
  const char* s;

  i = 0;
  while ( i < com_numConsoleLines ) {
    if ( strcmp(com_consoleLines[i].Argv(0), "set")) {
      i++;
      continue;
    }

    s = com_consoleLines[i].Argv(1);

    if ( !match || !idStr::Icmp(s, match)) {
      cvarSystem->SetCVarString(s, com_consoleLines[i].Argv(2));
      if ( once ) {
        // kill the line
        int j = i + 1;
        while ( j < com_numConsoleLines ) {
          com_consoleLines[j - 1] = com_consoleLines[j];
          j++;
        }
        com_numConsoleLines--;
        continue;
      }
    }
    i++;
  }
}

/*
==================
idCommonLocal::AddStartupCommands

Adds command line parameters as script statements
Commands are separated by + signs

Returns true if any late commands were added, which
will keep the demoloop from immediately starting
==================
*/
bool idCommonLocal::AddStartupCommands(void) {
  int i;
  bool added;

  added = false;
  // quote every token, so args with semicolons can work
  for ( i = 0; i < com_numConsoleLines; i++ ) {
    if ( !com_consoleLines[i].Argc()) {
      continue;
    }

    // set commands won't override menu startup
    if ( idStr::Icmpn(com_consoleLines[i].Argv(0), "set", 3)) {
      added = true;
    }
    // directly as tokenized so nothing gets screwed
    cmdSystem->BufferCommandArgs(CMD_EXEC_APPEND, com_consoleLines[i]);
  }

  return added;
}

#ifndef __EMSCRIPTEN__
/*
==================
idCommonLocal::WriteFlaggedCVarsToFile
==================
*/
void idCommonLocal::WriteFlaggedCVarsToFile(const char* filename, int flags, const char* setCmd) {
  idFile* f;

  f = fileSystem->OpenFileWrite(filename, "fs_configpath");
  if ( !f ) {
    Printf("Couldn't write %s.\n", filename);
    return;
  }
  cvarSystem->WriteFlaggedVariables(flags, setCmd, f);
  fileSystem->CloseFile(f);
}
#endif

/*
==================
idCommonLocal::WriteConfigToFile
==================
*/
void idCommonLocal::WriteConfigToFile(const char* filename) {
#ifdef ID_WRITE_VERSION
  ID_TIME_T t;
char *curtime;
idStr runtag;
idFile_Memory compressed( "compressed" );
idBase64 out;
#endif

#ifndef __EMSCRIPTEN__
  idFile* f = fileSystem->OpenFileWrite(filename, "fs_configpath");
  if ( !f ) {
    Printf("Couldn't write %s.\n", filename);
    return;
  }
#else
  idFile_Memory* f = new idFile_Memory();
#endif

#ifdef ID_WRITE_VERSION
  assert( config_compressor );
t = time( NULL );
curtime = ctime( &t );
sprintf( runtag, "%s - %s", cvarSystem->GetCVarString( "si_version" ), curtime );
config_compressor->Init( &compressed, true, 8 );
config_compressor->Write( runtag.c_str(), runtag.Length() );
config_compressor->FinishCompress( );
out.Encode( (const byte *)compressed.GetDataPtr(), compressed.Length() );
f->Printf( "// %s\n", out.c_str() );
#endif

  idKeyInput::WriteBindings(f);
  cvarSystem->WriteFlaggedVariables(CVAR_ARCHIVE, "seta", f);

#ifndef __EMSCRIPTEN__
#else
  idFile* realf = fileSystem->OpenFileWrite(filename, "fs_configpath");
  if ( !realf ) {
    Printf("Couldn't write %s.\n", filename);
    fileSystem->CloseFile(f);
    return;
  }
  realf->Write(f->GetDataPtr(), f->Length());
  fileSystem->CloseFile(realf);
#endif

  fileSystem->CloseFile(f);
}

/*
===============
idCommonLocal::WriteConfiguration

Writes key bindings and archived cvars to config file if modified
===============
*/
void idCommonLocal::WriteConfiguration(void) {
  // if we are quiting without fully initializing, make sure
  // we don't write out anything
  if ( !com_fullyInitialized ) {
    return;
  }

  if ( !( cvarSystem->GetModifiedFlags() & CVAR_ARCHIVE )) {
    return;
  }
  cvarSystem->ClearModifiedFlags(CVAR_ARCHIVE);

  // disable printing out the "Writing to:" message
	bool developer = com_developer.GetBool();
  com_developer.SetBool(false);

  WriteConfigToFile(CONFIG_FILE);
  session->WriteCDKey();

  // restore the developer cvar
  com_developer.SetBool(developer);

}

/*
===============
KeysFromBinding()
Returns the key bound to the command
===============
*/
const char* idCommonLocal::KeysFromBinding(const char* bind) {
  return idKeyInput::KeysFromBinding(bind);
}

/*
===============
BindingFromKey()
Returns the binding bound to key
===============
*/
const char* idCommonLocal::BindingFromKey(const char* key) {
  return idKeyInput::BindingFromKey(key);
}

/*
===============
ButtonState()
Returns the state of the button
===============
*/
int idCommonLocal::ButtonState(int key) {
  return usercmdGen->ButtonState(key);
}

/*
===============
ButtonState()
Returns the state of the key
===============
*/
int idCommonLocal::KeyState(int key) {
  return usercmdGen->KeyState(key);
}

//============================================================================

/*
============
idCmdSystemLocal::PrintMemInfo_f

This prints out memory debugging data
============
*/
#ifdef __EMSCRIPTEN__
#include "emscripten/heap.h"
#include "malloc.h"
#endif
static void PrintMemInfo_f(const idCmdArgs& args) {
  MemInfo_t mi;

  memset(&mi, 0, sizeof(mi));
  mi.filebase = session->GetCurrentMapName();

  renderSystem->PrintMemInfo(&mi);            // textures and models
  soundSystem->PrintMemInfo(&mi);            // sounds

  common->Printf(" Used image memory: %.2f MB\n",mi.imageAssetsTotal * 1.0 / (1024.0*1024.0));
  mi.assetTotals += mi.imageAssetsTotal;

  common->Printf(" Used model memory: %.2f MB\n", mi.modelAssetsTotal * 1.0 / (1024.0*1024.0));
  mi.assetTotals += mi.modelAssetsTotal;

  common->Printf(" Used sound memory: %.2f MB\n", mi.soundAssetsTotal * 1.0 / (1024.0*1024.0));
  mi.assetTotals += mi.soundAssetsTotal;

  common->Printf(" Used asset memory: %.2f MB\n", mi.assetTotals * 1.0 / (1024.0*1024.0));

#ifndef __EMSCRIPTEN__
  // write overview file
  idFile* f;

  f = fileSystem->OpenFileAppend("maps/printmeminfo.txt");
  if ( !f ) {
    return;
  }

  f->Printf("total(%s ) image(%s ) model(%s ) sound(%s ): %s\n", idStr::FormatNumber(mi.assetTotals).c_str(),
            idStr::FormatNumber(mi.imageAssetsTotal).c_str(),
            idStr::FormatNumber(mi.modelAssetsTotal).c_str(), idStr::FormatNumber(mi.soundAssetsTotal).c_str(),
            mi.filebase.c_str());

  fileSystem->CloseFile(f);
#endif

#ifdef __EMSCRIPTEN__
  common->Printf("\nEmscripten Heap Info:\n");

  {
    const size_t wasmHeapSize = emscripten_get_heap_size();
    const uintptr_t programBreak = reinterpret_cast<uintptr_t>(sbrk(0));

    struct mallinfo mi = mallinfo();

    const size_t freeInsideMalloc = static_cast<size_t>(mi.fordblks);
    const size_t notYetGivenToMalloc =
        wasmHeapSize > programBreak ? wasmHeapSize - programBreak : 0;

    const size_t estimatedAvailable =
        freeInsideMalloc + notYetGivenToMalloc;

    common->Printf("WASM heap size        : %.2f MB\n", wasmHeapSize / 1024.0 / 1024.0);
    common->Printf("Program break / sbrk  : %.2f MB\n", programBreak / 1024.0 / 1024.0);
    common->Printf("malloc used           : %.2f MB\n", mi.uordblks / 1024.0 / 1024.0);
    common->Printf("malloc free blocks    : %.2f MB\n", freeInsideMalloc / 1024.0 / 1024.0);
    common->Printf("not yet given malloc  : %.2f MB\n", notYetGivenToMalloc / 1024.0 / 1024.0);
    common->Printf("estimated available   : %.2f MB\n", estimatedAvailable / 1024.0 / 1024.0);
  }
#endif
}

/*
==================
Com_Error_f

Just throw a fatal error to test error shutdown procedures.
==================
*/
static void Com_Error_f(const idCmdArgs& args) {
  if ( !com_developer.GetBool()) {
    commonLocal.Printf("error may only be used in developer mode\n");
    return;
  }

  if ( args.Argc() > 1 ) {
    commonLocal.FatalError("Testing fatal error");
  }
  else {
    commonLocal.Error("Testing drop error");
  }
}

/*
==================
Com_Freeze_f

Just freeze in place for a given number of seconds to test error recovery.
==================
*/
static void Com_Freeze_f(const idCmdArgs& args) {
  float s;
  int start, now;

  if ( args.Argc() != 2 ) {
    commonLocal.Printf("freeze <seconds>\n");
    return;
  }

  if ( !com_developer.GetBool()) {
    commonLocal.Printf("freeze may only be used in developer mode\n");
    return;
  }

  s = atof(args.Argv(1));

  start = eventLoop->Milliseconds();

  while ( 1 ) {
    now = eventLoop->Milliseconds();
    if (( now - start ) * 0.001f > s ) {
      break;
    }
  }
}

/*
=================
Com_Crash_f

A way to force a bus error for development reasons
=================
*/
static void Com_Crash_f(const idCmdArgs& args) {
  if ( !com_developer.GetBool()) {
    commonLocal.Printf("crash may only be used in developer mode\n");
    return;
  }

  __builtin_trap();
}

/*
=================
Com_Quit_f
=================
*/
static void Com_Quit_f(const idCmdArgs& args) {
  commonLocal.Quit();
}

/*
===============
Com_WriteConfig_f

Write the config file to a specific name
===============
*/
void Com_WriteConfig_f(const idCmdArgs& args) {
  idStr filename;

  if ( args.Argc() != 2 ) {
    commonLocal.Printf("Usage: writeconfig <filename>\n");
    return;
  }

  filename = args.Argv(1);
  filename.DefaultFileExtension(".cfg");
  commonLocal.Printf("Writing %s.\n", filename.c_str());
  commonLocal.WriteConfigToFile(filename);
}

/*
=================
Com_SetMachineSpecs_f
=================
*/
void Com_SetMachineSpec_f(const idCmdArgs& args) {
  commonLocal.SetMachineSpec();
}

/*
=================
Com_ExecMachineSpecs_f
=================
*/
void Com_ExecMachineSpec_f(const idCmdArgs& args) {
#ifndef __EMSCRIPTEN__
	if ( com_machineSpec.GetInteger() == 3 ) { // ultra
		  cvarSystem->SetCVarInteger( "image_lodbias", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE) );
      cvarSystem->SetCVarInteger("image_forceDownSize", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarInteger("image_roundDown", 1, EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarInteger("image_preload", 1, EM_CVAR_FLAGS(CVAR_ARCHIVE));
	  	cvarSystem->SetCVarInteger( "image_useAllFormats", 1, EM_CVAR_FLAGS(CVAR_ARCHIVE) );
      cvarSystem->SetCVarInteger("image_downSizeSpecular", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarInteger("image_downSizeBump", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarInteger("image_downSizeSpecularLimit", 64, EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarInteger("image_downSizeBumpLimit", 256, EM_CVAR_FLAGS(CVAR_ARCHIVE));
		  cvarSystem->SetCVarInteger( "image_usePrecompressedTextures", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE) );
      cvarSystem->SetCVarInteger("image_downsize", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarString("image_filter", "GL_LINEAR_MIPMAP_LINEAR", EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarInteger("image_anisotropy", 8, EM_CVAR_FLAGS(CVAR_ARCHIVE));
		  cvarSystem->SetCVarInteger( "image_useCompression", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE) );
		  cvarSystem->SetCVarInteger( "image_ignoreHighQuality", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE) );
      cvarSystem->SetCVarInteger("s_maxSoundsPerShader", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE));
      cvarSystem->SetCVarInteger("r_mode", 5, EM_CVAR_FLAGS(CVAR_ARCHIVE));
		  cvarSystem->SetCVarInteger( "image_useNormalCompression", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE) );
      cvarSystem->SetCVarInteger("r_multiSamples", 0, EM_CVAR_FLAGS(CVAR_ARCHIVE));
	} else if ( com_machineSpec.GetInteger() == 2 ) { // high
      cvarSystem->SetCVarString("image_filter", "GL_LINEAR_MIPMAP_LINEAR", CVAR_ARCHIVE);
	  	cvarSystem->SetCVarInteger( "image_lodbias", 0, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("image_forceDownSize", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_roundDown", 1, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_preload", 1, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useAllFormats", 1, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("image_downSizeSpecular", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeBump", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeSpecularLimit", 64, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeBumpLimit", 256, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_usePrecompressedTextures", 1, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("image_downsize", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_anisotropy", 8, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useCompression", 1, CVAR_ARCHIVE );
		  cvarSystem->SetCVarInteger( "image_ignoreHighQuality", 0, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("s_maxSoundsPerShader", 0, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useNormalCompression", 0, CVAR_ARCHIVE );
		  cvarSystem->SetCVarInteger("r_mode", 4, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("r_multiSamples", 0, CVAR_ARCHIVE);
	} else if ( com_machineSpec.GetInteger() == 1 ) { // medium
      cvarSystem->SetCVarString("image_filter", "GL_LINEAR_MIPMAP_LINEAR", CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_anisotropy", 1, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_lodbias", 0, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("image_downSize", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_forceDownSize", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_roundDown", 1, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_preload", 1, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useCompression", 1, CVAR_ARCHIVE );
		  cvarSystem->SetCVarInteger( "image_useAllFormats", 1, CVAR_ARCHIVE );
		  cvarSystem->SetCVarInteger( "image_usePrecompressedTextures", 1, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("image_downSizeSpecular", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeBump", 0, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeSpecularLimit", 64, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeBumpLimit", 256, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useNormalCompression", 2, CVAR_ARCHIVE );
		  cvarSystem->SetCVarInteger("r_mode", 3, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("r_multiSamples", 0, CVAR_ARCHIVE);
	} else { // low
      cvarSystem->SetCVarString("image_filter", "GL_LINEAR_MIPMAP_LINEAR", CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_anisotropy", 1, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_lodbias", 0, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("image_roundDown", 1, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_preload", 1, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useAllFormats", 1, CVAR_ARCHIVE );
		  cvarSystem->SetCVarInteger( "image_usePrecompressedTextures", 1, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("image_downSize", 1, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_anisotropy", 0, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useCompression", 1, CVAR_ARCHIVE );
		  cvarSystem->SetCVarInteger( "image_ignoreHighQuality", 1, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("s_maxSoundsPerShader", 1, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeSpecular", 1, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeBump", 1, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeSpecularLimit", 64, CVAR_ARCHIVE);
      cvarSystem->SetCVarInteger("image_downSizeBumpLimit", 256, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger("r_mode", 3, CVAR_ARCHIVE);
		  cvarSystem->SetCVarInteger( "image_useNormalCompression", 2, CVAR_ARCHIVE );
      cvarSystem->SetCVarInteger("r_multiSamples", 0, CVAR_ARCHIVE);
    }
#endif

    cvarSystem->SetCVarBool("com_purgeAll", false, CVAR_ARCHIVE);
#ifndef __EMSCRIPTEN__
    cvarSystem->SetCVarBool("r_forceLoadImages", false, CVAR_ARCHIVE);
#endif

    cvarSystem->SetCVarBool("g_decals", true, CVAR_ARCHIVE);
    cvarSystem->SetCVarBool("g_projectileLights", true, CVAR_ARCHIVE);
    cvarSystem->SetCVarBool("g_doubleVision", true, CVAR_ARCHIVE);
    cvarSystem->SetCVarBool("g_muzzleFlash", true, CVAR_ARCHIVE);
}

/*
=================
Com_ReloadEngine_f
=================
*/
void Com_ReloadEngine_f(const idCmdArgs& args) {
  bool menu = false;

  if ( !commonLocal.IsInitialized()) {
    return;
  }

  if ( args.Argc() > 1 && idStr::Icmp(args.Argv(1), "menu") == 0 ) {
    menu = true;
  }

  common->Printf("============= ReloadEngine start =============\n");
  if ( !menu ) {
    Sys_ShowConsole(1, false);
  }
  commonLocal.ShutdownGame(true);
  commonLocal.InitGame();
  if ( !menu /*&& !idAsyncNetwork::serverDedicated.GetBool()*/) {
    Sys_ShowConsole(0, false);
  }
  common->Printf("============= ReloadEngine end ===============\n");

  if ( !cmdSystem->PostReloadEngine()) {
    if ( menu ) {
      session->StartMenu();
    }
  }
}

/*
===============
idCommonLocal::GetLanguageDict
===============
*/
const idLangDict* idCommonLocal::GetLanguageDict(void) {
  return &languageDict;
}

/*
===============
idCommonLocal::FilterLangList
===============
*/
void idCommonLocal::FilterLangList(idStrList* list, idStr lang) {

  idStr temp;
  for ( int i = 0; i < list->Num(); i++ ) {
    temp = ( *list )[i];
    temp = temp.Right(temp.Length() - strlen("strings/"));
    temp = temp.Left(lang.Length());
    if ( idStr::Icmp(temp, lang) != 0 ) {
      list->RemoveIndex(i);
      i--;
    }
  }
}

/*
===============
idCommonLocal::InitLanguageDict
===============
*/
void idCommonLocal::InitLanguageDict(void) {
  idStr fileName;
  languageDict.Clear();

  //D3XP: Instead of just loading a single lang file for each language
  //we are going to load all files that begin with the language name
  //similar to the way pak files work. So you can place english001.lang
  //to add new strings to the english language dictionary
  idFileList* langFiles;
  langFiles = fileSystem->ListFilesTree("strings", ".lang", true);

  idStrList langList = langFiles->GetList();

  StartupVariable("sys_lang",
                  false);    // let it be set on the command line - this is needed because this init happens very early
  idStr langName = cvarSystem->GetCVarString("sys_lang");

  //Loop through the list and filter
  idStrList currentLangList = langList;
  FilterLangList(&currentLangList, langName);

  if ( currentLangList.Num() == 0 ) {
    // reset cvar to default and try to load again
    cmdSystem->BufferCommandText(CMD_EXEC_NOW, "reset sys_lang");
    langName = cvarSystem->GetCVarString("sys_lang");
    currentLangList = langList;
    FilterLangList(&currentLangList, langName);
  }

  for ( int i = 0; i < currentLangList.Num(); i++ ) {
    //common->Printf("%s\n", currentLangList[i].c_str());
    languageDict.Load(currentLangList[i], false);
  }

  fileSystem->FreeFileList(langFiles);

  Sys_InitScanTable();
}

#ifndef __EMSCRIPTEN__
/*
===============
idCommonLocal::LocalizeSpecificMapData
===============
*/
void idCommonLocal::LocalizeSpecificMapData(const char* fileName, idLangDict& langDict, const idLangDict& replaceArgs) {
  idStr out, ws, work;

  idMapFile map;
  if ( map.Parse(fileName, false, false)) {
    int count = map.GetNumEntities();
    for ( int i = 0; i < count; i++ ) {
      idMapEntity* ent = map.GetEntity(i);
      if ( ent ) {
        for ( int j = 0; j < replaceArgs.GetNumKeyVals(); j++ ) {
          const idLangKeyValue* kv = replaceArgs.GetKeyVal(j);
          const char* temp = ent->epairs.GetString(kv->key);
          if ( temp && *temp ) {
            idStr val = kv->value;
            if ( val == temp ) {
              ent->epairs.Set(kv->key, langDict.AddString(temp));
            }
          }
        }
      }
    }
    map.Write(fileName, ".map");
  }
}

/*
===============
idCommonLocal::LocalizeMapData
===============
*/
void idCommonLocal::LocalizeMapData(const char* fileName, idLangDict& langDict) {
  const char* buffer = NULL;
  idLexer src(LEXFL_NOFATALERRORS | LEXFL_NOSTRINGCONCAT | LEXFL_ALLOWMULTICHARLITERALS |
              LEXFL_ALLOWBACKSLASHSTRINGCONCAT);

  common->SetRefreshOnPrint(true);

  if ( fileSystem->ReadFile(fileName, (void**) &buffer) > 0 ) {
    src.LoadMemory(buffer, strlen(buffer), fileName);
    if ( src.IsLoaded()) {
      common->Printf("Processing %s\n", fileName);
      idStr mapFileName;
      idToken token, token2;
      idLangDict replaceArgs;
      while ( src.ReadToken(&token)) {
        mapFileName = token;
        replaceArgs.Clear();
        src.ExpectTokenString("{");
        while ( src.ReadToken(&token)) {
          if ( token == "}" ) {
            break;
          }
          if ( src.ReadToken(&token2)) {
            if ( token2 == "}" ) {
              break;
            }
            replaceArgs.AddKeyVal(token, token2);
          }
        }
        common->Printf("  localizing map %s...\n", mapFileName.c_str());
        LocalizeSpecificMapData(mapFileName, langDict, replaceArgs);
      }
    }
    fileSystem->FreeFile((void*) buffer);
  }

  common->SetRefreshOnPrint(false);
}

/*
===============
idCommonLocal::LocalizeGui
===============
*/
void idCommonLocal::LocalizeGui(const char* fileName, idLangDict& langDict) {
  idStr out, ws, work;
  const char* buffer = NULL;
  out.Empty();
  int k;
  char ch;
  char slash = '\\';
  char tab = 't';
  char nl = 'n';
  idLexer src(LEXFL_NOFATALERRORS | LEXFL_NOSTRINGCONCAT | LEXFL_ALLOWMULTICHARLITERALS |
              LEXFL_ALLOWBACKSLASHSTRINGCONCAT);
  if ( fileSystem->ReadFile(fileName, (void**) &buffer) > 0 ) {
    src.LoadMemory(buffer, strlen(buffer), fileName);
    if ( src.IsLoaded()) {
      idFile* outFile = fileSystem->OpenFileWrite(fileName);
      common->Printf("Processing %s\n", fileName);
      session->UpdateScreen();
      idToken token;
      while ( src.ReadToken(&token)) {
        src.GetLastWhiteSpace(ws);
        out += ws;
        if ( token.type == TT_STRING ) {
          out += va("\"%s\"", token.c_str());
        }
        else {
          out += token;
        }
        if ( out.Length() > 200000 ) {
          outFile->Write(out.c_str(), out.Length());
          out = "";
        }
        work = token.Right(6);
        if ( token.Icmp("text") == 0 || work.Icmp("::text") == 0 || token.Icmp("choices") == 0 ) {
          if ( src.ReadToken(&token)) {
            // see if already exists, if so save that id to this position in this file
            // otherwise add this to the list and save the id to this position in this file
            src.GetLastWhiteSpace(ws);
            out += ws;
            token = langDict.AddString(token);
            out += "\"";
            for ( k = 0; k < token.Length(); k++ ) {
              ch = token[k];
              if ( ch == '\t' ) {
                out += slash;
                out += tab;
              }
              else if ( ch == '\n' || ch == '\r' ) {
                out += slash;
                out += nl;
              }
              else {
                out += ch;
              }
            }
            out += "\"";
          }
        }
        else if ( token.Icmp("comment") == 0 ) {
          if ( src.ReadToken(&token)) {
            // need to write these out by hand to preserve any \n's
            // see if already exists, if so save that id to this position in this file
            // otherwise add this to the list and save the id to this position in this file
            src.GetLastWhiteSpace(ws);
            out += ws;
            out += "\"";
            for ( k = 0; k < token.Length(); k++ ) {
              ch = token[k];
              if ( ch == '\t' ) {
                out += slash;
                out += tab;
              }
              else if ( ch == '\n' || ch == '\r' ) {
                out += slash;
                out += nl;
              }
              else {
                out += ch;
              }
            }
            out += "\"";
          }
        }
      }
      outFile->Write(out.c_str(), out.Length());
      fileSystem->CloseFile(outFile);
    }
    fileSystem->FreeFile((void*) buffer);
  }
}
#endif

/*
=================
ReloadLanguage_f
=================
*/
void Com_ReloadLanguage_f(const idCmdArgs& args) {
  commonLocal.InitLanguageDict();
}

typedef idHashTable <idStrList> ListHash;

#ifndef __EMSCRIPTEN__
void LoadMapLocalizeData(ListHash& listHash) {

  idStr fileName = "map_localize.cfg";
  const char* buffer = NULL;
  idLexer src(LEXFL_NOFATALERRORS | LEXFL_NOSTRINGCONCAT | LEXFL_ALLOWMULTICHARLITERALS |
              LEXFL_ALLOWBACKSLASHSTRINGCONCAT);

  if ( fileSystem->ReadFile(fileName, (void**) &buffer) > 0 ) {
    src.LoadMemory(buffer, strlen(buffer), fileName);
    if ( src.IsLoaded()) {
      idStr classname;
      idToken token;


      while ( src.ReadToken(&token)) {
        classname = token;
        src.ExpectTokenString("{");

        idStrList list;
        while ( src.ReadToken(&token)) {
          if ( token == "}" ) {
            break;
          }
          list.Append(token);
        }

        listHash.Set(classname, list);
      }
    }
    fileSystem->FreeFile((void*) buffer);
  }

}

void LoadGuiParmExcludeList(idStrList& list) {

  idStr fileName = "guiparm_exclude.cfg";
  const char* buffer = NULL;
  idLexer src(LEXFL_NOFATALERRORS | LEXFL_NOSTRINGCONCAT | LEXFL_ALLOWMULTICHARLITERALS |
              LEXFL_ALLOWBACKSLASHSTRINGCONCAT);

  if ( fileSystem->ReadFile(fileName, (void**) &buffer) > 0 ) {
    src.LoadMemory(buffer, strlen(buffer), fileName);
    if ( src.IsLoaded()) {
      idStr classname;
      idToken token;


      while ( src.ReadToken(&token)) {
        list.Append(token);
      }
    }
    fileSystem->FreeFile((void*) buffer);
  }
}

bool TestMapVal(idStr& str) {
  //Already Localized?
  if ( str.Find("#str_") != -1 ) {
    return false;
  }

  return true;
}

bool TestGuiParm(const char* parm, const char* value, idStrList& excludeList) {

  idStr testVal = value;

  //Already Localized?
  if ( testVal.Find("#str_") != -1 ) {
    return false;
  }

  //Numeric
  if ( testVal.IsNumeric()) {
    return false;
  }

  //Contains ::
  if ( testVal.Find("::") != -1 ) {
    return false;
  }

  //Contains /
  if ( testVal.Find("/") != -1 ) {
    return false;
  }

  if ( excludeList.Find(testVal)) {
    return false;
  }

  return true;
}

void GetFileList(const char* dir, const char* ext, idStrList& list) {

  //Recurse Subdirectories
  idStrList dirList;
  Sys_ListFiles(dir, "/", dirList);
  for ( int i = 0; i < dirList.Num(); i++ ) {
    if ( dirList[i] == "." || dirList[i] == ".." ) {
      continue;
    }
    idStr fullName = va("%s/%s", dir, dirList[i].c_str());
    GetFileList(fullName, ext, list);
  }

  idStrList fileList;
  Sys_ListFiles(dir, ext, fileList);
  for ( int i = 0; i < fileList.Num(); i++ ) {
    idStr fullName = va("%s/%s", dir, fileList[i].c_str());
    list.Append(fullName);
  }
}

int LocalizeMap(const char* mapName, idLangDict& langDict, ListHash& listHash, idStrList& excludeList, bool writeFile) {

  common->Printf("Localizing Map '%s'\n", mapName);

  int strCount = 0;

  idMapFile map;
  if ( map.Parse(mapName, false, false)) {
    int count = map.GetNumEntities();
    for ( int j = 0; j < count; j++ ) {
      idMapEntity* ent = map.GetEntity(j);
      if ( ent ) {

        idStr classname = ent->epairs.GetString("classname");

        //Hack: for info_location
        bool hasLocation = false;

        idStrList* list;
        listHash.Get(classname, &list);
        if ( list ) {

          for ( int k = 0; k < list->Num(); k++ ) {

            idStr val = ent->epairs.GetString(( *list )[k], "");

            if ( val.Length() && classname == "info_location" && ( *list )[k] == "location" ) {
              hasLocation = true;
            }

            if ( val.Length() && TestMapVal(val)) {

              if ( !hasLocation || ( *list )[k] == "location" ) {
                //Localize it!!!
                strCount++;
                ent->epairs.Set(( *list )[k], langDict.AddString(val));
              }
            }
          }
        }

        listHash.Get("all", &list);
        if ( list ) {
          for ( int k = 0; k < list->Num(); k++ ) {
            idStr val = ent->epairs.GetString(( *list )[k], "");
            if ( val.Length() && TestMapVal(val)) {
              //Localize it!!!
              strCount++;
              ent->epairs.Set(( *list )[k], langDict.AddString(val));
            }
          }
        }

        //Localize the gui_parms
        const idKeyValue* kv = ent->epairs.MatchPrefix("gui_parm");
        while ( kv ) {
          if ( TestGuiParm(kv->GetKey(), kv->GetValue(), excludeList)) {
            //Localize It!
            strCount++;
            ent->epairs.Set(kv->GetKey(), langDict.AddString(kv->GetValue()));
          }
          kv = ent->epairs.MatchPrefix("gui_parm", kv);
        }
      }
    }
    if ( writeFile && strCount > 0 ) {
      //Before we write the map file lets make a backup of the original
      idStr file = fileSystem->RelativePathToOSPath(mapName);
      idStr bak = file.Left(file.Length() - 4);
      bak.Append(".bak_loc");
      fileSystem->CopyFile(file, bak);

      map.Write(mapName, ".map");
    }
  }

  common->Printf("Count: %d\n", strCount);
  return strCount;
}

/*
=================
LocalizeMaps_f
=================
*/
void Com_LocalizeMaps_f(const idCmdArgs& args) {
  if ( args.Argc() < 2 ) {
    common->Printf("Usage: localizeMaps <count | dictupdate | all> <map>\n");
    return;
  }

  int strCount = 0;

  bool count = false;
  bool dictUpdate = false;
  bool write = false;

  if ( idStr::Icmp(args.Argv(1), "count") == 0 ) {
    count = true;
  }
  else if ( idStr::Icmp(args.Argv(1), "dictupdate") == 0 ) {
    count = true;
    dictUpdate = true;
  }
  else if ( idStr::Icmp(args.Argv(1), "all") == 0 ) {
    count = true;
    dictUpdate = true;
    write = true;
  }
  else {
    common->Printf("Invalid Command\n");
    common->Printf("Usage: localizeMaps <count | dictupdate | all>\n");
    return;

  }

  idLangDict strTable;
  idStr filename = va("strings/english%.3i.lang", com_product_lang_ext.GetInteger());
  if ( strTable.Load(filename) == false ) {
    //This is a new file so set the base index
    strTable.SetBaseID(com_product_lang_ext.GetInteger() * 100000);
  }

  common->SetRefreshOnPrint(true);

  ListHash listHash;
  LoadMapLocalizeData(listHash);

  idStrList excludeList;
  LoadGuiParmExcludeList(excludeList);

  if ( args.Argc() == 3 ) {
    strCount += LocalizeMap(args.Argv(2), strTable, listHash, excludeList, write);
  }
  else {
    idStrList files;
    GetFileList("z:/d3xp/d3xp/maps/game", "*.map", files);
    for ( int i = 0; i < files.Num(); i++ ) {
      idStr file = fileSystem->OSPathToRelativePath(files[i]);
      strCount += LocalizeMap(file, strTable, listHash, excludeList, write);
    }
  }

  if ( count ) {
    common->Printf("Localize String Count: %d\n", strCount);
  }

  common->SetRefreshOnPrint(false);

  if ( dictUpdate ) {
    strTable.Save(filename);
  }
}

/*
=================
LocalizeGuis_f
=================
*/
void Com_LocalizeGuis_f(const idCmdArgs& args) {

  if ( args.Argc() != 2 ) {
    common->Printf("Usage: localizeGuis <all | gui>\n");
    return;
  }

  idLangDict strTable;

  idStr filename = va("strings/english%.3i.lang", com_product_lang_ext.GetInteger());
  if ( strTable.Load(filename) == false ) {
    //This is a new file so set the base index
    strTable.SetBaseID(com_product_lang_ext.GetInteger() * 100000);
  }

  idFileList* files;
  if ( idStr::Icmp(args.Argv(1), "all") == 0 ) {
    idStr game = cvarSystem->GetCVarString("fs_game");
    if ( game.Length()) {
      files = fileSystem->ListFilesTree("guis", "*.gui", true, game);
    }
    else {
      files = fileSystem->ListFilesTree("guis", "*.gui", true);
    }
    for ( int i = 0; i < files->GetNumFiles(); i++ ) {
      commonLocal.LocalizeGui(files->GetFile(i), strTable);
    }
    fileSystem->FreeFileList(files);

    if ( game.Length()) {
      files = fileSystem->ListFilesTree("guis", "*.pd", true, game);
    }
    else {
      files = fileSystem->ListFilesTree("guis", "*.pd", true, "d3xp");
    }

    for ( int i = 0; i < files->GetNumFiles(); i++ ) {
      commonLocal.LocalizeGui(files->GetFile(i), strTable);
    }
    fileSystem->FreeFileList(files);

  }
  else {
    commonLocal.LocalizeGui(args.Argv(1), strTable);
  }
  strTable.Save(filename);
}
#endif

/*
=================
idCommonLocal::InitCommands
=================
*/
void idCommonLocal::InitCommands(void) {
  cmdSystem->AddCommand("error", Com_Error_f, CMD_FL_SYSTEM | CMD_FL_CHEAT, "causes an error");
  cmdSystem->AddCommand("crash", Com_Crash_f, CMD_FL_SYSTEM | CMD_FL_CHEAT, "causes a crash");
  cmdSystem->AddCommand("freeze", Com_Freeze_f, CMD_FL_SYSTEM | CMD_FL_CHEAT,
                        "freezes the game for a number of seconds");
  cmdSystem->AddCommand("quit", Com_Quit_f, CMD_FL_SYSTEM, "quits the game");
  cmdSystem->AddCommand("exit", Com_Quit_f, CMD_FL_SYSTEM, "exits the game");
  cmdSystem->AddCommand("writeConfig", Com_WriteConfig_f, CMD_FL_SYSTEM, "writes a config file");
  cmdSystem->AddCommand("reloadEngine", Com_ReloadEngine_f, CMD_FL_SYSTEM,
                        "reloads the engine down to including the file system");

#ifndef __EMSCRIPTEN__
  cmdSystem->AddCommand("setMachineSpec", Com_SetMachineSpec_f, CMD_FL_SYSTEM,
                        "detects system capabilities and sets com_machineSpec to appropriate value");

  cmdSystem->AddCommand("execMachineSpec", Com_ExecMachineSpec_f, CMD_FL_SYSTEM,
                        "execs the appropriate config files and sets cvars based on com_machineSpec");

  // idLib commands
  cmdSystem->AddCommand("memoryDump", Mem_Dump_f, CMD_FL_SYSTEM | CMD_FL_CHEAT, "creates a memory dump");
  cmdSystem->AddCommand("memoryDumpCompressed", Mem_DumpCompressed_f, CMD_FL_SYSTEM | CMD_FL_CHEAT,
                        "creates a compressed memory dump");
#endif
  cmdSystem->AddCommand("printMemInfo", PrintMemInfo_f, CMD_FL_SYSTEM, "prints memory debugging data");

  cmdSystem->AddCommand("showStringMemory", idStr::ShowMemoryUsage_f, CMD_FL_SYSTEM, "shows memory used by strings");
  cmdSystem->AddCommand("showDictMemory", idDict::ShowMemoryUsage_f, CMD_FL_SYSTEM,
                        "shows memory used by dictionaries");
  cmdSystem->AddCommand("listDictKeys", idDict::ListKeys_f, CMD_FL_SYSTEM | CMD_FL_CHEAT,
                        "lists all keys used by dictionaries");
  cmdSystem->AddCommand("listDictValues", idDict::ListValues_f, CMD_FL_SYSTEM | CMD_FL_CHEAT,
                        "lists all values used by dictionaries");

#ifndef __EMSCRIPTEN__
  cmdSystem->AddCommand("testSIMD", idSIMD::Test_f, CMD_FL_SYSTEM | CMD_FL_CHEAT, "test SIMD code");

  // localization
  cmdSystem->AddCommand("localizeGuis", Com_LocalizeGuis_f, CMD_FL_SYSTEM | CMD_FL_CHEAT, "localize guis");
  cmdSystem->AddCommand("localizeMaps", Com_LocalizeMaps_f, CMD_FL_SYSTEM | CMD_FL_CHEAT, "localize maps");
#endif
  cmdSystem->AddCommand("reloadLanguage", Com_ReloadLanguage_f, CMD_FL_SYSTEM, "reload language dict");
}

/*
=================
idCommonLocal::InitRenderSystem
=================
*/
void idCommonLocal::InitRenderSystem( void ) {
#ifndef __EMSCRIPTEN__
  if ( com_skipRenderer.GetBool()) {
    return;
  }
#endif

  renderSystem->InitOpenGL();
  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04343"));
}

/*
=================
idCommonLocal::PrintLoadingMessage
=================
*/
void idCommonLocal::PrintLoadingMessage(const char* msg) {
  if ( !( msg && *msg )) {
    return;
  }
  renderSystem->BeginFrame(renderSystem->GetScreenWidth(), renderSystem->GetScreenHeight());
  renderSystem->DrawStretchPic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1, 1, splashMaterial);
	int len = strlen( msg );
  renderSystem->DrawSmallStringExt(( 640 - len * SMALLCHAR_WIDTH ) / 2, 410, msg, idVec4(0.0f, 0.81f, 0.94f, 1.0f), true, charsetMaterial);
  renderSystem->EndFrame(NULL, NULL);
  ForceRefreshScreen(0);
}

/*
=================
idCommonLocal::InitSIMD
=================
*/
void idCommonLocal::InitSIMD(void) {
  idSIMD::InitProcessor("doom", com_forceGenericSIMD.GetBool());
  com_forceGenericSIMD.ClearModified();
}

/*
=================
idCommonLocal::Frame
=================
*/
void idCommonLocal::Frame(void) {
#ifdef __EMSCRIPTEN__
// Exceptions are disabled on Emscripten
#else
  try {
#endif

#ifdef __EMSCRIPTEN__
  // Just in case...
  com_enableMediaLoad = true;
#endif

  // pump all the events
  Sys_GenerateEvents();

  // write config file if anything changed
  WriteConfiguration();

#ifdef __EMSCRIPTEN__
  // No SIMD support in emscripten, no need change SIMD backend
#else
  // change SIMD implementation if required
  if ( com_forceGenericSIMD.IsModified() ) {
    InitSIMD();
  }
#endif

  eventLoop->RunEventLoop();               // EMTERPRETIFY function (might yields)

#ifdef NOMT
    // In single threaded mode, manually call the async timer update code at each frame
    common->Async();

    // D3WASM: Disable background download thread for now (not really used)
    //fileSystem->RunThread();
#endif

  com_frameTime = com_ticNumber * USERCMD_MSEC;

  idAsyncNetwork::RunFrame();

  if ( idAsyncNetwork::IsActive()) {
    //if (idAsyncNetwork::serverDedicated.GetInteger() != 1) {
    session->GuiFrameEvents();            // EMTERPRETIFY function (might yields)
    session->UpdateScreen(false);
    //}
  }
  else {
    session->Frame();                     // EMTERPRETIFY function (might yields)
    session->UpdateScreen(false);
  }

  // report timing information
  if ( com_speeds.GetBool()) {
    static int lastTime;
    int nowTime = Sys_Milliseconds();
    int com_frameMsec = nowTime - lastTime;
    lastTime = nowTime;
    Printf("frame:%i all:%3i gfr:%3i rf:%3i bk:%3i\n", com_frameNumber, com_frameMsec, time_gameFrame, time_frontend,
           time_backend);
    time_gameFrame = 0;
    time_gameDraw = 0;
  }

  com_frameNumber++;

  // set idLib frame number for frame based memory dumps
  idLib::frameNumber = com_frameNumber;

#ifdef __EMSCRIPTEN__
// Exceptions are disabled on Emscripten
#else
  }
	catch( idException & ) {
		return;			// an ERP_DROP was thrown
	}
#endif
}

/*
=================
idCommonLocal::GUIFrame
=================
*/
void idCommonLocal::GUIFrame(bool execCmd, bool network) {
  Sys_GenerateEvents();
  eventLoop->RunEventLoop(execCmd);    // EMTERPRETIFY function (might yields)

#ifdef NOMT
  // In Single Threaded mode manually update the async timer here, as com_ticNumber is used locally
  // This is to try to get things correctly in sync
  common->Async();
#endif

  com_frameTime = com_ticNumber * USERCMD_MSEC;
  if ( network ) {
    idAsyncNetwork::RunFrame();
  }

  session->Frame();
  session->UpdateScreen(false);
  ForceRefreshScreen();
}

void idCommonLocal::ForceRefreshScreen(bool vsync) {
#ifdef __EMSCRIPTEN__
  emscripten_sleep(vsync ? USERCMD_MSEC : 0);
#endif
}

/*
=================
idCommonLocal::SingleAsyncTic

The system will asyncronously call this function 60 times a second to
handle the time-critical functions that we don't want limited to
the frame rate:

sound mixing
user input generation (conditioned by com_asyncInput)
packet server operation
packet client operation

We are not using thread safe libraries, so any functionality put here must
be VERY VERY careful about what it calls.
=================
*/

typedef struct {
  int milliseconds;            // should always be incremeting by 60hz
  int deltaMsec;                // should always be 16
  int timeConsumed;            // msec spent in Com_AsyncThread()
  int clientPacketsReceived;
  int serverPacketsReceived;
  int mostRecentServerPacketSequence;
} asyncStats_t;

static const int MAX_ASYNC_STATS = 1024;
asyncStats_t com_asyncStats[MAX_ASYNC_STATS];        // indexed by com_ticNumber
int prevAsyncMsec;
int lastTicMsec;

void idCommonLocal::SingleAsyncTic(void) {
  // main thread code can prevent this from happening while modifying
  // critical data structures
  Sys_EnterCriticalSection();

  asyncStats_t* stat = &com_asyncStats[com_ticNumber & ( MAX_ASYNC_STATS - 1 )];
  memset(stat, 0, sizeof(*stat));
  stat->milliseconds = Sys_Milliseconds();
  stat->deltaMsec = stat->milliseconds - com_asyncStats[( com_ticNumber - 1 ) & ( MAX_ASYNC_STATS - 1 )].milliseconds;

#ifndef __EMSCRIPTEN__
  if ( usercmdGen && com_asyncInput.GetBool()) {
    usercmdGen->UsercmdInterrupt();
  }

  switch ( com_asyncSound.GetInteger()) {
    case 1:
      soundSystem->AsyncUpdate(stat->milliseconds);
      break;
    case 3:
      soundSystem->AsyncUpdateWrite(stat->milliseconds);
      break;
  }
#endif

  // we update com_ticNumber after all the background tasks
  // have completed their work for this tic
  com_ticNumber++;

  stat->timeConsumed = Sys_Milliseconds() - stat->milliseconds;

  Sys_LeaveCriticalSection();
}

/*
=================
idCommonLocal::Async
=================
*/
void idCommonLocal::Async(void) {
  const int msec = Sys_Milliseconds();
  if ( !lastTicMsec ) {
    lastTicMsec = msec - USERCMD_MSEC;
  }

  if ( !com_preciseTic.GetBool()) {
    // just run a single tic, even if the exact msec isn't precise
    SingleAsyncTic();
    return;
  }

  int ticMsec = USERCMD_MSEC;

  // the number of msec per tic can be varies with the timescale cvar
  const float timescale = com_timescale.GetFloat();
  if ( timescale != 1.0f ) {
    ticMsec /= timescale;
    if ( ticMsec < 1 ) {
      ticMsec = 1;
    }
  }

  // don't skip too many
  if ( timescale == 1.0f ) {
    if ( lastTicMsec + 10 * USERCMD_MSEC < msec ) {
      lastTicMsec = msec - 10 * USERCMD_MSEC;
    }
  }

  while ( lastTicMsec + ticMsec <= msec ) {
    SingleAsyncTic();
    lastTicMsec += ticMsec;
  }
}

#ifndef __EMSCRIPTEN__
/*
=================
idCommonLocal::LoadGameDLLbyName

Helper for LoadGameDLL() to make it less painfull to try different dll names.
=================
*/
void idCommonLocal::LoadGameDLLbyName(const char* dll, idStr& s) {
  s.CapLength(0);
  // try next to the binary first (build tree)
  if ( Sys_GetPath(PATH_EXE, s)) {
    // "s = " seems superfluous, but works around g++ 4.7 bug else StripFilename()
    // (and possibly even CapLength()) seems to be "optimized" away and the string contains garbage
    s = s.StripFilename();
    s.AppendPath(dll);
    gameDLL = sys->DLL_Load(s);
  }

  // then the install folder on *nix
  if ( !gameDLL ) {
    s = BUILD_LIBDIR;
    s.AppendPath(dll);
    gameDLL = sys->DLL_Load(s);
  }
}
#endif

/*
=================
idCommonLocal::LoadGameDLL
=================
*/
void idCommonLocal::LoadGameDLL(void) {
#ifdef __DOOM_DLL__
  const char		*fs_game;
char			dll[MAX_OSPATH];
idStr			s;

gameImport_t	gameImport;
gameExport_t	gameExport;
GetGameAPI_t	GetGameAPI;

fs_game = cvarSystem->GetCVarString("fs_game");
if (!fs_game || !fs_game[0])
fs_game = BASE_GAMEDIR;

gameDLL = 0;

sys->DLL_GetFileName(fs_game, dll, sizeof(dll));
LoadGameDLLbyName(dll, s);

// there was no gamelib for this mod, use default one from base game
if (!gameDLL) {
common->Warning( "couldn't load mod-specific %s, defaulting to base game's library!", dll );
sys->DLL_GetFileName(BASE_GAMEDIR, dll, sizeof(dll));
LoadGameDLLbyName(dll, s);
}

if ( !gameDLL ) {
common->FatalError( "couldn't load game dynamic library" );
return;
}

common->Printf("loaded game library '%s'.\n", s.c_str());

GetGameAPI = (GetGameAPI_t) Sys_DLL_GetProcAddress( gameDLL, "GetGameAPI" );
if ( !GetGameAPI ) {
Sys_DLL_Unload( gameDLL );
gameDLL = 0;
common->FatalError( "couldn't find game DLL API" );
return;
}

gameImport.version					= GAME_API_VERSION;
gameImport.sys						= ::sys;
gameImport.common					= ::common;
gameImport.cmdSystem				= ::cmdSystem;
gameImport.cvarSystem				= ::cvarSystem;
gameImport.fileSystem				= ::fileSystem;
gameImport.networkSystem			= ::networkSystem;
gameImport.renderSystem				= ::renderSystem;
gameImport.soundSystem				= ::soundSystem;
gameImport.renderModelManager		= ::renderModelManager;
gameImport.uiManager				= ::uiManager;
gameImport.declManager				= ::declManager;
gameImport.AASFileManager			= ::AASFileManager;
gameImport.collisionModelManager	= ::collisionModelManager;

gameExport							= *GetGameAPI( &gameImport );

if ( gameExport.version != GAME_API_VERSION ) {
Sys_DLL_Unload( gameDLL );
gameDLL = 0;
common->FatalError( "wrong game DLL API version" );
return;
}

game								= gameExport.game;
gameEdit							= gameExport.gameEdit;

#endif

  // initialize the game object
  if ( game != NULL ) {
    game->Init();
  }
}

/*
=================
idCommonLocal::UnloadGameDLL
=================
*/
void idCommonLocal::UnloadGameDLL(void) {

  // shut down the game object
  if ( game != NULL ) {
    game->Shutdown();
  }

#ifdef __DOOM_DLL__

  if ( gameDLL ) {
Sys_DLL_Unload( gameDLL );
gameDLL = 0;
}
game = NULL;
gameEdit = NULL;

#endif

  gameCallbacks.Reset(); // DG: these callbacks are invalid now because DLL has been unloaded
}

/*
=================
idCommonLocal::IsInitialized
=================
*/
bool idCommonLocal::IsInitialized(void) const {
  return com_fullyInitialized;
}

/*
=================
idCommonLocal::SetMachineSpec
=================
*/
void idCommonLocal::SetMachineSpec(void) {
  int sysRam = Sys_GetSystemRam();

  Printf("Detected\n\t%i MB of System memory\n\n", sysRam);
#ifndef __EMSCRIPTEN__
  if (sysRam >= 1024) {
#endif
    Printf("This system qualifies for Ultra quality!\n");
#ifndef __EMSCRIPTEN__
    com_machineSpec.SetInteger(3);
  } else if (sysRam >= 512) {
    Printf("This system qualifies for High quality!\n");
    com_machineSpec.SetInteger(2);
  } else if (sysRam >= 384) {
    Printf("This system qualifies for Medium quality.\n");
   com_machineSpec.SetInteger(1);
  } else {
    Printf("This system qualifies for Low quality.\n");
    com_machineSpec.SetInteger(0);
  }
#endif
}

#ifdef NOMT
#else
static unsigned int AsyncTimer(unsigned int interval, void *) {
  common->Async();
  Sys_TriggerEvent(TRIGGER_EVENT_ONE);

  // calculate the next interval to get as close to 60fps as possible
  unsigned int now = SDL_GetTicks();
  unsigned int tick = com_ticNumber * USERCMD_MSEC;

  if (now >= tick)
    return 1;

  return tick - now;
}
#endif

/*
=================
idCommonLocal::Init
=================
*/
void idCommonLocal::Init(int argc, char** argv) {

#ifdef __EMSCRIPTEN__
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO ))
#else
  if ( SDL_Init(
    SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) // init joystick to work around SDL 2.0.9 bug #4391
#endif
  {
    Sys_Error("Error while initializing SDL: %s", SDL_GetError());
  }

  Sys_InitThreads();

#if !defined(__EMSCRIPTEN__)
  try {
#endif

    // set interface pointers used by idLib
    idLib::sys = sys;
    idLib::common = common;
    idLib::cvarSystem = cvarSystem;
    idLib::fileSystem = fileSystem;

    // initialize idLib
    idLib::Init();

    // clear warning buffer
    ClearWarnings(GAME_NAME
    " initialization" );

    // parse command line options
    ParseCommandLine(argc, argv);

    // init console command system
    cmdSystem->Init();

    // init CVar system
    cvarSystem->Init();

    // start file logging right away, before early console or whatever
    StartupVariable("win_outputDebugString", false);

    // register all static CVars
    idCVar::RegisterStaticVars();

    // print engine version
    SDL_version sdlv;
    SDL_GetVersion(&sdlv);

    Printf("%s using SDL v%u.%u.%u\n",
           version.string, sdlv.major, sdlv.minor, sdlv.patch);

    // initialize key input/binding, done early so bind command exists
    idKeyInput::Init();

    // init the console so we can take prints
    console->Init();

    // get architecture info
    Sys_Init();

    // initialize networking
    Sys_InitNetworking();

    // override cvars from command line
    StartupVariable(NULL, false);

    // set fpu double extended precision
    Sys_FPU_SetPrecision();

    // initialize processor specific SIMD implementation
    InitSIMD();

    // init commands
    InitCommands();

#ifdef ID_WRITE_VERSION
    config_compressor = idCompressor::AllocArithmetic();
#endif

    // game specific initialization
    InitGame();

    if ( !AddStartupCommands()) {
      // if the user didn't give any commands, run default action
      session->StartMenu(true);
    }

    // print all warnings queued during initialization
    PrintWarnings();

    // remove any prints from the notify lines
    console->ClearNotifyLines();

    ClearCommandLine();

    // load the persistent console history
    console->LoadHistory();

    com_fullyInitialized = true;
    common->DPrintf("Engine fully initialized.\n");

#if !defined(__EMSCRIPTEN__)
  }
  catch ( idException& ) {
    Sys_Error("Error during initialization");
  }
#endif

#ifdef NOMT
#else
  async_timer = SDL_AddTimer(USERCMD_MSEC, AsyncTimer, NULL);

  if ( !async_timer ) {
    Sys_Error("Error while starting the async timer: %s", SDL_GetError());
  }
#endif
}


/*
=================
idCommonLocal::Shutdown
=================
*/
void idCommonLocal::Shutdown(void) {
#ifdef NOMT
#else
  if ( async_timer ) {
    SDL_RemoveTimer(async_timer);
    async_timer = 0;
  }
#endif

  //idAsyncNetwork::server.Kill();
  idAsyncNetwork::client.Shutdown();

  if (com_errorEntered == ERP_NONE) {
    // save persistent console history
    console->SaveHistory();
  }

  // game specific shut down
  ShutdownGame(false);

  // shut down non-portable system services
  Sys_Shutdown();

  // shut down the console
  console->Shutdown();

  // shut down the key system
  idKeyInput::Shutdown();

  // shut down the cvar system
  cvarSystem->Shutdown();

  // shut down the console command system
  cmdSystem->Shutdown();

#ifdef ID_WRITE_VERSION
  delete config_compressor;
config_compressor = NULL;
#endif

  // free any buffered warning messages
  ClearWarnings(GAME_NAME
  " shutdown" );
  warningCaption.Clear();
  errorList.Clear();

  // free language dictionary
  languageDict.Clear();

  // enable leak test
  Mem_EnableLeakTest("doom");

  // shutdown idLib
  idLib::ShutDown();

  Sys_ShutdownThreads();

  SDL_Quit();
}

/*
=================
idCommonLocal::InitGame
=================
*/
void idCommonLocal::InitGame(void) {
  // initialize the file system
  fileSystem->Init();

  // initialize the declaration manager
  declManager->Init();

  idFile* file = fileSystem->OpenExplicitFileRead(fileSystem->RelativePathToOSPath(CONFIG_SPEC, "fs_configpath"));
  bool sysDetect = ( file == NULL );
  if ( file ) {
    fileSystem->CloseFile(file);
  }
  else {
    file = fileSystem->OpenFileWrite(CONFIG_SPEC, "fs_configpath");
    fileSystem->CloseFile(file);
  }

  idCmdArgs args;
  if ( sysDetect ) {
    SetMachineSpec();
    Com_ExecMachineSpec_f(args);
  }

  // initialize the renderSystem data structures, but don't start OpenGL yet
  renderSystem->Init();

  // initialize string database right off so we can use it for loading messages
  InitLanguageDict();

  splashMaterial = declManager->FindMaterial("splashScreen");
  charsetMaterial = declManager->FindMaterial("textures/bigchars");

  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04344"));

  // load the font, etc
  console->LoadGraphics();

  // init journalling, etc
  eventLoop->Init();

  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04345"));

  // exec the startup scripts
  cmdSystem->BufferCommandText(CMD_EXEC_APPEND, "exec editor.cfg\n");
  cmdSystem->BufferCommandText(CMD_EXEC_APPEND, "exec default.cfg\n");

  // skip the config file if "safe" is on the command line
  if ( !SafeMode()) {
    cmdSystem->BufferCommandText(CMD_EXEC_APPEND, "exec "
    CONFIG_FILE
    "\n" );
  }
  cmdSystem->BufferCommandText(CMD_EXEC_APPEND, "exec autoexec.cfg\n");

  // reload the language dictionary now that we've loaded config files
  cmdSystem->BufferCommandText(CMD_EXEC_APPEND, "reloadLanguage\n");

  // run cfg execution
  cmdSystem->ExecuteCommandBuffer();

  // re-override anything from the config files with command line args
  StartupVariable(NULL, false);

  // if any archived cvars are modified after this, we will trigger a writing of the config file
  cvarSystem->ClearModifiedFlags(CVAR_ARCHIVE);

  // init the user command input code
  usercmdGen->Init();

  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04346"));

  // start the sound system, but don't do any hardware operations yet
  soundSystem->Init();

  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04347"));

  // init async network
  idAsyncNetwork::Init();

  // Init rendersystem
  InitRenderSystem();

  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04349"));

  // initialize the user interfaces
  uiManager->Init();

  // startup the script debugger
  // DebuggerServerInit();

#ifdef __EMSCRIPTEN__
  // Check if we are loading data in "chunks"
  FILE* f = NULL;
  f = fopen( "/usr/local/share/d3wasm/base/demo_bootstrap.pk4", "r");
  if (f) {
    common->Printf("Check if data is correctly fetched\n");
    // Yes
    fclose(f);
    f = NULL;

    static bool bPakLoaded = false;

    // Does the chunks are already loaded ?
    f = fopen("/usr/local/share/d3wasm/base/demo_game00.pk4", "r");

    if (f && bPakLoaded) {
      // Yes
      fclose(f);
      f = NULL;
    } else {
      if (!f) {
        common->Printf("Fetching base demo data...\n");
        PrintLoadingMessage("Fetching base demo data (15MB)...");

        while (f == NULL) {
          // Wait for the next chunk to be loaded
				  // Yield case: local graphics update outside of main loop
          //emscripten_sleep(333);

          f = fopen("/usr/local/share/d3wasm/base/demo_game00.pk4", "r");
        }
      }
      fclose(f);
      f = NULL;

      common->Printf("Loading base demo data...\n");
      PrintLoadingMessage("Loading base demo data...");

      fileSystem->Restart();

      declManager->Init();

      bPakLoaded = true;
    }
  }
#endif

  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04350"));

  // load the game dll
  LoadGameDLL();

  PrintLoadingMessage(common->GetLanguageDict()->GetString("#str_04351"));

  // init the session
  session->Init();

  // have to do this twice.. first one sets the correct r_mode for the renderer init
  // this time around the backend is all setup correct.. a bit fugly but do not want
  // to mess with all the gl init at this point.. an old vid card will never qualify for
  if ( sysDetect ) {
    SetMachineSpec();
    Com_ExecMachineSpec_f(args);
    cvarSystem->SetCVarInteger("s_numberOfSpeakers", 2);
    cmdSystem->BufferCommandText(CMD_EXEC_NOW, "s_restart\n");
    cmdSystem->ExecuteCommandBuffer();
  }
}

/*
=================
idCommonLocal::ShutdownGame
=================
*/
void idCommonLocal::ShutdownGame(bool reloading) {

  // kill sound first
  idSoundWorld* sw = soundSystem->GetPlayingSoundWorld();
  if ( sw ) {
    sw->StopAllSounds();
  }

  // shutdown the script debugger
  // DebuggerServerShutdown();

  idAsyncNetwork::client.Shutdown();

  // shut down the session
  session->Shutdown();

  // shut down the user interfaces
  uiManager->Shutdown();

  // shut down the sound system
  soundSystem->Shutdown();

  // shut down async networking
  idAsyncNetwork::Shutdown();

  // shut down the user command input code
  usercmdGen->Shutdown();

  // shut down the event loop
  eventLoop->Shutdown();

  splashMaterial = NULL;
  charsetMaterial = NULL;

  // shut down the renderSystem
  renderSystem->Shutdown();

  // shutdown the decl manager
  declManager->Shutdown();

  // unload the game dll
  UnloadGameDLL();

  // dump warnings to "warnings.txt"
#ifdef DEBUG
  DumpWarnings();
#endif

  // shut down the file system
  fileSystem->Shutdown(reloading);
}

// DG: below here are hacks to allow adding callbacks and exporting additional functions to the
//     Game DLL without breaking the ABI. See Common.h for longer explanation...


// returns true if setting the callback was successful, else false
// When a game DLL is unloaded the callbacks are automatically removed from the Engine
// so you usually don't have to worry about that; but you can call this with cb = NULL
// and userArg = NULL to remove a callback manually (e.g. if userArg refers to an object you deleted)
bool idCommonLocal::SetCallback(idCommon::CallbackType cbt, idCommon::FunctionPointer cb, void* userArg) {
  switch ( cbt ) {
    case idCommon::CB_ReloadImages:
      gameCallbacks.reloadImagesCB = (idGameCallbacks::ReloadImagesCallback) cb;
      gameCallbacks.reloadImagesUserArg = userArg;
      return true;

    default:
      Warning("Called idCommon::SetCallback() with unknown CallbackType %d!\n", cbt);
      return false;
  }
}

static bool isDemo(void) {
  return sessLocal.IsDemoVersion();
}

// returns true if that function is available in this version of dhewm3
// *out_fnptr will be the function (you'll have to cast it probably)
// *out_userArg will be an argument you have to pass to the function, if appropriate (else NULL)
bool idCommonLocal::GetAdditionalFunction(idCommon::FunctionType ft, idCommon::FunctionPointer* out_fnptr, void** out_userArg)
{
	if(out_userArg != NULL)
    *out_userArg = NULL;

  if ( out_fnptr == NULL ) {
    Warning("Called idCommon::GetAdditionalFunction() with out_fnptr == NULL!\n");
    return false;
  }
  switch ( ft ) {
    case idCommon::FT_IsDemo:
      *out_fnptr = (idCommon::FunctionPointer) isDemo;
      // don't set *out_userArg, this function takes no arguments
      return true;

    default:
      *out_fnptr = NULL;
      Warning("Called idCommon::SetCallback() with unknown FunctionType %d!\n", ft);
      return false;
  }
}


idGameCallbacks gameCallbacks;

idGameCallbacks::idGameCallbacks()
  : reloadImagesCB(NULL), reloadImagesUserArg(NULL) { }

void idGameCallbacks::Reset() {
  reloadImagesCB = NULL;
  reloadImagesUserArg = NULL;
}

/*
===============
idCommonLocal::TouchEngineData
===============
*/
void idCommonLocal::TouchEngineData() {
  if (splashMaterial) {
    splashMaterial->Touch();
  }
  if (charsetMaterial) {
    charsetMaterial->Touch();
  }
}