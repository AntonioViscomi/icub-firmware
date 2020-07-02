
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// the following code is an extention to the CMSIS-RTOS2 based on RTX
// hence i keep the original copyright of file rtx_kernel.c where i got the code
// i have modified, as well as teh revelant original code

#if 0

/*
 * Copyright (c) 2013-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -----------------------------------------------------------------------------
 *
 * Project:     CMSIS-RTOS RTX
 * Title:       Kernel functions
 *
 * -----------------------------------------------------------------------------
 */




#endif


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "cmsisos2.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "rtx_lib.h"


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


const cmsisos2_reltime_t cmsisos_reltimezero = 0;
const cmsisos2_reltime_t cmsisos_reltime1microsec = 1;
const cmsisos2_reltime_t cmsisos_reltime1millisec = 1000;
const cmsisos2_reltime_t cmsisos_reltime1second = 1000000;
const cmsisos2_reltime_t cmsisos_reltimeforever = 0xffffffff;

  

// was called svcRtxKernelGetSysTimerCount()
static uint32_t svcRtxKernelGetSysTimerCount_U64 (uint32_t *l, uint32_t *m) {
  uint64_t tick;
  uint64_t count;

  tick  = (uint64_t)osRtxInfo.kernel.tick;
  count = OS_Tick_GetCount();
  if (OS_Tick_GetOverflow() != 0U) {
    count = OS_Tick_GetCount();
    tick++;
  }
  count += tick * OS_Tick_GetInterval();
  EvrRtxKernelGetSysTimerCount((uint32_t)count);
  *l = count & 0xffffffff;
  *m = (count >> 32) & 0xffffffff;
  return *l;
}

SVC0_2 (KernelGetSysTimerCount_U64, uint32_t, uint32_t *, uint32_t *)


// was called: 
/// Get the RTOS kernel system timer count.
// uint32_t osKernelGetSysTimerCount (void) { 
cmsisos2_abstime_t cmsisos2_sys_abstime(void) {
  uint32_t count = 0;
  uint32_t c64m = 0;
  uint32_t c64l = 0;
    
  if (IsIrqMode() || IsIrqMasked()) {
    count = svcRtxKernelGetSysTimerCount_U64(&c64l, &c64m);
  } else {
    count =  __svcKernelGetSysTimerCount_U64(&c64l, &c64m);
  }
  cmsisos2_abstime_t r = c64m;
  r = (r << 32) | c64l;
  // r is now the absolute count referred to the clock of the system
  // we should divide by osKernelGetSysTimerFreq() to have the absolute time in seconds
  // and by osKernelGetSysTimerFreq()/1000000 to have it expressed in usec
  // however, I dont call osKernelGetSysTimerFreq and i use SystemCoreClock instead
  
  static uint32_t conversion1 = 0;
  if(0 == conversion1)
  {
      conversion1 = SystemCoreClock / 1000000;
  }
  
  return r/conversion1;
}


uint32_t cmsisos2_sys_reltime2tick(cmsisos2_reltime_t t)
{
    static uint32_t conversion = 0;
    if((0 == conversion))
    {
        conversion = 1000000 / osRtxConfig.tick_freq;
    }
    
    if(cmsisos_reltimeforever == t)
    {
        return osWaitForever;
    }
    
    return t/conversion; 
}


   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

