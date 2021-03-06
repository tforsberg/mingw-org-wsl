/**
 * @file mthr_init.c
 * Copyright 2012, 2013 MinGW.org project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* FIXME - Is this still used by GCC? */

/*
 * Created by Mumit Khan  <khan@nanotech.wisc.edu>
 *
 * Do the thread-support DLL initialization.
 *
 * This file is used iff the following conditions are met:
 *  - gcc uses -mthreads option 
 *  - user code uses C++ exceptions
 *
 * The sole job of the Mingw thread support DLL (MingwThr) is to catch 
 * all the dying threads and clean up the data allocated in the TLSs 
 * for exception contexts during C++ EH. Posix threads have key dtors, 
 * but win32 TLS keys do not, hence the magic. Without this, there's at 
 * least `24 * sizeof (void*)' bytes leaks for each catch/throw in each
 * thread.
 * 
 * See mthr.c for all the magic.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <stdio.h>

/*
 *----------------------------------------------------------------------
 *
 * DllMain --
 *
 *	This routine is called by the Mingw32, Cygwin32 or VC++ C run 
 *	time library init code, or the Borland DllEntryPoint routine. It 
 *	is responsible for initializing various dynamically loaded 
 *	libraries.
 *
 * Results:
 *      TRUE on sucess, FALSE on failure.
 *
 * Side effects:
 *
 *----------------------------------------------------------------------
 */
BOOL APIENTRY
DllMain (HINSTANCE hDllHandle /* Library instance handle. */,
	 DWORD reason /* Reason this function is being called. */,
	 LPVOID reserved /* Not used. */)
{

  extern CRITICAL_SECTION __mingwthr_cs;
  extern void __mingwthr_run_key_dtors( void );

#ifdef DEBUG
  printf ("%s: reason %d\n", __FUNCTION__, reason );
#endif

  switch (reason)
    {
    case DLL_PROCESS_ATTACH:
       InitializeCriticalSection (&__mingwthr_cs);
       break;

    case DLL_PROCESS_DETACH:
      __mingwthr_run_key_dtors();
       DeleteCriticalSection (&__mingwthr_cs);
      break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      __mingwthr_run_key_dtors();
      break;
    }
  return TRUE;
}
