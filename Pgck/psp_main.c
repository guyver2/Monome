#include "psp_main.h"

#define STDOUT_FILE  "stdout.txt"
#define STDERR_FILE  "stderr.txt"

/* If application's main() is redefined as SDL_main, and libSDLmain is
   linked, then this file will create the standard exit callback,
   define the PSP_* macros, add an exception handler, nullify device 
   checks and exit back to the browser when the program is finished. */



extern int PGCKmain();
extern int continuer;
PSP_MODULE_INFO("Monome", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
 
#define printf pspDebugScreenPrintf
 
 
 
int SetupCallbacks(void);
int exit_callback(int arg1, int arg2, void *common);
int CallbackThread(SceSize args, void *argp);
 
 
 
int main() {
SetupCallbacks();
pspfrequence(333);
PGCKmain();
SDL_Quit();
sceKernelSleepThread();
return 0;
}

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common) {
	  continuer = 0;
          sceKernelExitGame();
          return 0;
}
 
/* Callback thread */
int CallbackThread(SceSize args, void *argp) {
          int cbid;
 
          cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
          sceKernelRegisterExitCallback(cbid);
 
          sceKernelSleepThreadCB();
 
          return 0;
}
 
/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void) {
          int thid = 0;
 
          thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
          if(thid >= 0) {
                    sceKernelStartThread(thid, 0, 0);
          }
 
          return thid;
}

void pspfrequence(int mhz)
{
 scePowerSetClockFrequency(mhz, mhz, (mhz/2) );
}
