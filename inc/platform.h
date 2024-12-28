/*
* The MIT License (MIT)
*
* Copyright (c) 2024 Mohamed Ali Ben Abbes
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*  platform.h
*  Created on: Dec 28, 2024
*      Author: M.BEN ABBES
*/
#ifndef SCHEDULER_INC_PLATFORM_H_
#define SCHEDULER_INC_PLATFORM_H_

/**
 * @Note  All platform dependent code should be put here
 */

#include "stm32f7xx_hal.h"

#define  RTC_INTERRUPT_ROUTINE         RTC_Alarm_IRQHandler

#define  RTC_INTERRUPT_HANDLER()       HAL_RTC_AlarmIRQHandler(&hrtc)

extern   RTC_HandleTypeDef hrtc;

#endif /* SCHEDULER_INC_PLATFORM_H_ */
