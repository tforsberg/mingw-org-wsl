/**
 * @file feholdexcept.c
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
#include <fenv.h>
#include "cpu_features.h"

/* 7.6.4.2
   The feholdexcept function saves the current floating-point
   environment in the object pointed to by envp, clears the exception
   flags, and then installs a non-stop (continue on exceptions) mode,
   if available, for all exceptions.  */

int feholdexcept (fenv_t * envp)
{
  __asm__ ("fnstenv %0;" : "=m" (* envp)); /* save current into envp */
 /* fnstenv sets control word to non-stop for all exceptions, so all we
    need to do is clear the exception flags.  */
  __asm__ ("fnclex");

  if (__HAS_SSE)
    {
       unsigned int _csr;
       /* Save the SSE MXCSR register.  */
       __asm__ ("stmxcsr %0" :  "=m" (envp->__mxcsr));  
       /* Clear the exception flags. */
       _csr = envp->__mxcsr & ~FE_ALL_EXCEPT;
       /* Set exception mask to non-stop  */
       _csr |= (FE_ALL_EXCEPT << __MXCSR_EXCEPT_MASK_SHIFT) /*= 0x1f80 */;
       __asm__ volatile ("ldmxcsr %0" : : "m" (_csr));
    } 

  return 0;
}
