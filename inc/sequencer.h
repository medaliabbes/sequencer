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
#include "date_time.h"
#include "scheduler_queue.h"

typedef enum {
  Seq_Error_Ok               = 0 , /**<No Error>*/
  Seq_Error_Null_Pointer         , /**<Null Pointer>*/
  Seq_Error_Invalid_Id           , /**<Event id is invalid>*/
  Seq_Error_Excided_Max_Events   , /**<No more vents can be added>*/
  Seq_Error_Null_Repetition      , /**<Number of repetition is 0>*/
  /**<Execution Error>*/
  Seq_Error_Exc_Event_Deleted    , /**<Event Deleted>*/
  Seq_Error_Exc_Event_Suspended  , /**<Event Suspended>*/
  Seq_Error_Exc_Event_Null_Rep   , /**<No more repetition >*/
  Seq_Error_Exc_Event_Null_cbk   , /**<Null Callback>*/
  Seq_Error_Core_Failed          , /**<Scheduler will fail no more interrupts>*/
}Seq_Error_t;

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


typedef int (*EventCallback_t) (void * args) ;

#define  SEQ_REPETITION_INF            (0xFFFFFFFF)
#define  SEQ_EVERY_MONTH               (0xFF)
#define  SEQ_EVERY_YEAR                (0xFF)
#define  SEQ_EVERY_DAY                 (0xFF)
#define  SEQ_EVERY_HOUR                (0xFF)
#define  SEQ_EVERY_MINUTE              (0xFF)
#define  SEQ_EVERY_SECOND              (0xFF)

/**
 * Event States
 */
typedef enum {
  State_Ready     ,
  State_Running   ,
  State_Deleted   ,
  State_Suspended ,
  State_Invalid   ,
}State_t;

/**
 * Event priority
 */
typedef enum
{
	Priority_Low     = 0,
	Priority_Medium  = 3,
	Priority_High    = 2,
}Priority_t;

typedef struct
{
	uint8_t         id            ; // unique id for every event
	EventCallback_t Callback      ; // event callback function
	Time_t          StartTime     ; // event start time
	Time_t          NextExcTime   ; // event next execution time
	uint32_t        NumRepetetion ; // event number of repetition
	uint32_t        Period        ; // event period in seconds
	Priority_t      Priority      ; // event priority
	State_t         State         ; // event state
	void *          args          ; // event Arguments
	uint32_t        MagicNumber   ; // event Mg to assure is valid event
}Event_t;


typedef void (*GetTime_t)(Time_t * TimeStuct)  ;
typedef void (*SetAlarm_t)(Time_t * TimeStuct) ;
typedef bool (*SearchFunction_t)(Event_t * event );

typedef struct
{
	GetTime_t  GetTime  ;
	SetAlarm_t SetAlarm ;
}SequencerInitConfig_t;


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

Seq_Error_t    Sequencer_Process(void) ;

bool          Is_Seq_notification() ;

int     Sequencer_Idle(void * args) __attribute__((weak)) ;

#endif /* INC_SEQUENCER_H_ */
