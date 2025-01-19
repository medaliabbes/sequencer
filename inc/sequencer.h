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
*/
#ifndef INC_SEQUENCER_H_
#define INC_SEQUENCER_H_

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "platform.h"
#include "sequencer_def.h"
#include "sequencer_queue.h"

#define  MAX_EVENT_NUMBER  100

#define  MAGIC_NUMBER      0xf0175896

#define  RTC_INTERRUPT_NOTIF       0x61

#define  DEV


#ifdef DEV
/**DEVELOPEMENT CODE */
#define  SEQ_ASSERT( __condition__ , error)      assert(__condition__)
#define  SEQ_LOG(...)               printf(__VA_ARGS__)
#define  SEQ_LOG_TIME(st)           printf("%02d/%02d/%02d  %02d:%02d:%02d\n" , \
                                      st->year , st->month , st->day , \
								      st->hour , st->minute , st->second )
#else
/**PRODUCTION CODE */
#define  SEQ_ASSERT( __condition__ , error)     if(!(__condition__ )) return error ;
#define  SEQ_LOG(...)
#define  SEQ_LOG_TIME(st)

#endif /*DEV*/


Seq_Error_t    Sequencer_Init(SequencerInitConfig_t * Config) ;

/**
 *
 * return event ID
 */
uint8_t Sequencer_Add_Event_API(EventCallback_t EventHandler , Time_t * StartTime ,
                                uint32_t Repetetion , uint32_t Periode , Priority_t Priority ,
                                void * args);

Seq_Error_t    Sequencer_Delete_Event_API(uint8_t id) ;

Seq_Error_t    Sequencer_Suspend_Event_API(uint8_t id) ;

Seq_Error_t    Sequencer_Resume_Event_API(uint8_t id)  ;

Seq_Error_t    Sequencer_Change_Event_Period_API(uint8_t id , uint32_t period) ;

Seq_Error_t    Sequencer_Process(void) ;

bool           Sequencer_Is_notification() ;

int     Sequencer_Idle(void * args) __attribute__((weak)) ;

#ifdef ENABLE_UNIT_TEST
int      Sequencer_Get_Next_Event_Time(Time_t * NextEventTime) ;
int      Get_Number_Events_To_Execute() ;
#endif /*ENABLE_UNIT_TEST*/

#endif /* INC_SEQUENCER_H_ */
