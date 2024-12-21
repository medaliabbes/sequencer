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
* file  : scheduler.c
*
*  Created on: Nov 17, 2024
*      Author: M.BEN ABBES
*
*/
#include "scheduler.h"


static int  Scheduler_Get_Next_Event_Time(Time_t * NextEventTime) ;
static Sch_Error_t Scheduler_Add_Event(Event_t * Event) ;
static bool IsEventTimeNow(uint8_t event_id);

/**This function convert only time to uint32_t not the date*/
static uint32_t TimeToUint32(Time_t * sTime);
static void     Uint32ToTime(Time_t * sTime , uint32_t time_in_s);

static Event_t Events[MAX_EVENT_NUMBER] ;

static GetTime_t  GetTime  = NULL ;
static SetAlarm_t SetAlarm = NULL ;
static uint8_t EventCounter = 0 ;

/**NextEventId this should be replaced by a queue ,so when more than
 * one event occurs at the same time they can be executed*/
//static uint8_t NextEventId  = MAX_EVENT_NUMBER ;
static queue_t EventQueue  = {0};

static uint8_t EventQueueBuffer[10] = {0} ;

extern RTC_HandleTypeDef hrtc;

Sch_Error_t Scheduler_Init(SchedulerInitConfig_t * Config)
{
  SCH_ASSERT(Config != NULL , Sch_Error_Null_Pointer) ;

  SCH_ASSERT(Config->GetTime != NULL , Sch_Error_Null_Pointer) ;
  GetTime  =  Config->GetTime ;
  SCH_ASSERT(Config->SetAlarm != NULL , Sch_Error_Null_Pointer) ;
  SetAlarm = Config->SetAlarm ;

  Queue_Init(&EventQueue , EventQueueBuffer , 10) ;

  return Sch_Error_Ok ;
}

uint8_t Scheduler_Add_Event_API(EventCallback_t EventHandler , Time_t * StartTime ,
		                     uint32_t Repetetion , uint32_t Periode , Priority_t Priority ,
							 void * args)
{
	SCH_ASSERT( EventHandler != NULL , Sch_Error_Null_Pointer );
    SCH_ASSERT( StartTime != NULL    , Sch_Error_Null_Pointer );
    SCH_ASSERT( Repetetion != 0      , Sch_Error_Null_Repetition);
    SCH_ASSERT( EventCounter < MAX_EVENT_NUMBER , Sch_Error_Excided_Max_Events) ;

    SCH_LOG("Scheduler_Add_Event_API\n");
    Event_t my_event = { 0} ;

    my_event.id = EventCounter++ ;
    my_event.Priority = Priority ;
    my_event.Callback = EventHandler ;

    my_event.NumRepetetion = Repetetion ;
    my_event.args = args ;
    my_event.Period = Periode ;
    my_event.State     = State_Ready ;
    memcpy( (void*)&my_event.StartTime , (void *)StartTime , sizeof( Time_t));
    my_event.MagicNumber = MAGIC_NUMBER ;
    Scheduler_Add_Event(&my_event) ;
    Time_t NextEventTime = { 0 };
    Scheduler_Get_Next_Event_Time(&NextEventTime) ;
    SCH_LOG("API start time %02d:%02d:%02d\n" , NextEventTime.hour ,
    		NextEventTime.minute ,
			NextEventTime.second) ;
    //set alarm to next event time
    SetAlarm(&NextEventTime) ;
    return  my_event.id ;
}

static int  Scheduler_Get_Next_Event_Time(Time_t * NextEventTime)
{
	Time_t now ;

	GetTime(&now) ;

	uint32_t t_now = TimeToUint32(&now);
    uint8_t selected_event = MAX_EVENT_NUMBER ;
	uint32_t ptime = 0xffffffff ;
	//NextEventId = MAX_EVENT_NUMBER ;
	Queue_Dump(&EventQueue) ;
	for(int i = 0 ; i< EventCounter ; i++ )
	{
		/**When an event is deleted or suspended , it's not considered to
		 * to execute
		 * */
		if( (Events[i].MagicNumber != MAGIC_NUMBER) ||
		     Events[i].State != State_Ready )
		{
			continue ;
		}

		uint32_t ev_time = TimeToUint32(&Events[i].StartTime) ;

		/**inside the if should be replaced or removed **/
		if(ev_time < t_now )
		{
			ev_time += Events[i].Period ;
		}

		if(ev_time < ptime)
		{
			ptime = ev_time ;
			//NextEventId = i ;
			Queue_Dump(&EventQueue) ;
			Queue_Push(&EventQueue , i) ;
			selected_event = i ;
		}
		else if(ev_time == ptime)
		{
			ptime = ev_time ;
			Queue_Push(&EventQueue , i) ;
		}
	}
//	LOG("start time %02d:%02d:%02d\n" , Events[NextEventId].StartTime.hour ,
//			Events[NextEventId].StartTime.minute ,
//			Events[NextEventId].StartTime.second) ;

//	if(NextEventId != MAX_EVENT_NUMBER){
//	  memcpy(NextEventTime , &Events[NextEventId].StartTime , sizeof(Time_t)) ;
//	}
	/**this can be removed **/
	if(selected_event != MAX_EVENT_NUMBER )
	{
		memcpy(NextEventTime , &Events[selected_event].StartTime , sizeof(Time_t)) ;
	}
	return selected_event ;
}

static Sch_Error_t Scheduler_Add_Event(Event_t * Event)
{
	SCH_ASSERT( Event != NULL  , Sch_Error_Null_Pointer ) ;
	SCH_ASSERT(Event->id < MAX_EVENT_NUMBER , Sch_Error_Excided_Max_Events) ;

	memcpy(&Events[Event->id] , Event , sizeof(Event_t)) ;

	return Sch_Error_Ok ;
}


Sch_Error_t Scheduler_Execute_Event(uint8_t id)
{
	SCH_ASSERT(id < MAX_EVENT_NUMBER , Sch_Error_Invalid_Id);

	/**SCH_ASSERT the event is not suspended before execution*/
	if(Events[id].State == State_Suspended )
	{
		printf("Event Suspended\n\n") ;
		return Sch_Error_Exc_Event_Suspended ;
	}
	/**SCH_ASSERT the event is not disabled before execution*/
	if(Events[id].State   == State_Deleted )
	{
		printf("Event Deleted\n\n") ;
	  return Sch_Error_Exc_Event_Deleted ;
	}

	/**Check event still has num rep to execute */
	if(Events[id].NumRepetetion == 0)
	{
		printf("Event End Rep\n\n") ;
		return Sch_Error_Exc_Event_Null_Rep ;
	}
	/**SCH_ASSERT Event CallBack is not NULL*/
	if( Events[id].Callback == NULL)
	{
		printf("Event Null Callback\n\n") ;
		return Sch_Error_Exc_Event_Null_cbk ;
	}
	else{
		Events[id].Callback( Events[id].args ) ;
	}

	return Sch_Error_Ok ;
}

void Scheduler_Event_Set_Args(uint8_t id , void * arg1, void * arg2)
{

}

Event_t * Scheduler_Get_Event_By_Id(uint8_t id)
{
	return NULL ;
}


/**
 * Brief  This Function calculate and set an event
 *        next execution time
 * param  id of the event
 */
Sch_Error_t Scheduler_Update_Event(uint8_t id )
{
	SCH_ASSERT(id < MAX_EVENT_NUMBER  , Sch_Error_Invalid_Id) ;

	if(Events[id].NumRepetetion == 0)
	{
		return Sch_Error_Null_Repetition ;
	}

	if(Events[id].NumRepetetion != SCH_REPETITION_INF){
		Events[id].NumRepetetion-- ;
	}
	/**When an event reaches it number of repetition
	 * times it will be deleted
	 */
	if(Events[id].NumRepetetion == 0)
	{
		Events[id].State = State_Deleted ;
	}
	else{
	  uint32_t NextTime_s = TimeToUint32(&Events[id].StartTime) + Events[id].Period ;
	  Uint32ToTime(&Events[id].StartTime , NextTime_s) ;
//	  SCH_LOG("Event next Time %02d:%02d:%02d\n\n" , Events[id].StartTime.hour ,
//												   Events[id].StartTime.minute ,
//												   Events[id].StartTime.second);
	}
	return Sch_Error_Ok ;
}

/**
 * Convert Time to seconds
 */
static uint32_t TimeToUint32(Time_t * sTime)
{
	uint32_t ret = sTime->second + (sTime->minute * 60) + (sTime->hour * 60 * 60);
	return ret ;
}

/**
  * @brief This function handles RTC alarms (A and B) interrupt through EXTI line 17.
  */
void RTC_Alarm_IRQHandler(void)
{
  /* USER CODE BEGIN RTC_Alarm_IRQn 0 */

  /* USER CODE END RTC_Alarm_IRQn 0 */
  HAL_RTC_AlarmIRQHandler(&hrtc);
  /* USER CODE BEGIN RTC_Alarm_IRQn 1 */
  Scheduler_Process() ;
  /* USER CODE END RTC_Alarm_IRQn 1 */
}

static void     Uint32ToTime(Time_t * sTime , uint32_t time_in_s)
{
	sTime->hour   = time_in_s / 3600 ;
	sTime->minute = (time_in_s - (sTime->hour  * 3600)) / 60 ;
	sTime->second = time_in_s % 60 ;
}

static bool IsEventTimeNow(uint8_t event_id)
{
	Time_t t_Now = { 0 } ;

	Event_t * ev = &Events[event_id] ;

	GetTime(&t_Now) ;
	Time_t * t = &t_Now ;
	SCH_LOG_TIME(t) ;

	if(!(ev->StartTime.year == t_Now.year )&& !(ev->StartTime.year == SCH_EVERY_YEAR))
	{
		return false ;
	}

	if(!(ev->StartTime.month == t_Now.month )&& !(ev->StartTime.month == SCH_EVERY_MONTH))
	{
		return false ;
	}

	if(!(ev->StartTime.day == t_Now.day )&& !(ev->StartTime.day == SCH_EVERY_DAY))
	{
		return false ;
	}

	if(!(ev->StartTime.hour == t_Now.hour )&& !(ev->StartTime.hour == SCH_EVERY_HOUR))
	{
		return false ;
	}

	if(!(ev->StartTime.minute == t_Now.minute )&& !(ev->StartTime.minute == SCH_EVERY_MINUTE))
	{
		return false ;
	}

	/**CAN TOLERATE 1 OR 2 SECONDS*/
	if(!(ev->StartTime.second == t_Now.second )&& !(ev->StartTime.second == SCH_EVERY_SECOND))
	{
		return false ;
	}

	return true ;
}


Sch_Error_t Scheduler_Process()
{

#ifdef MULTIPLE_EVENTS_SUPPORT
  int QueueSize = Queue_Get_Size(&EventQueue) ;

  /**
   * @Note  : Events can be sorted based on they priority before execution
   */
  printf("PROCESS : Number of event to exec : %d\n\n" , QueueSize) ;
  for(int i = 0 ;i <QueueSize ;i++)
  {
	  uint8_t event_id = MAX_EVENT_NUMBER ;
	  Queue_Pop(&EventQueue , &event_id) ;
	  /**IsEventTimeNow Will assure the execution of event at the right time and date */
	  if(IsEventTimeNow(event_id))
	  {
	    Scheduler_Execute_Event(event_id) ;
	    Scheduler_Update_Event(event_id) ;
	  }
  }
#else
  if(IsEventTimeNow(NextEventId))
   {
 	  Scheduler_Execute_Event(NextEventId) ;
 	  Scheduler_Update_Event(NextEventId) ;
 	  //printf("Event Executed\n\n") ;
   }
   else{
 	  printf("Missed Event Time\n") ;
   }
#endif

  Time_t NextEventTime =  { 0} ;
  int selected_event = Scheduler_Get_Next_Event_Time(&NextEventTime) ;
  if(selected_event == MAX_EVENT_NUMBER)
  {
	  printf("No next event is selected \n\n") ;
  }
  SetAlarm(&NextEventTime) ;
  return Sch_Error_Ok ;
}

Sch_Error_t Scheduler_Delete_Event_API(uint8_t id)
{
	SCH_ASSERT(id < MAX_EVENT_NUMBER , Sch_Error_Invalid_Id) ;
	Events[id].State   = State_Deleted ;
	return Sch_Error_Ok ;
}

Sch_Error_t Scheduler_Suspend_Event_API(uint8_t id)
{
	SCH_ASSERT(id < MAX_EVENT_NUMBER , Sch_Error_Invalid_Id) ;
	Events[id].State = State_Suspended ;
	return Sch_Error_Ok ;
}

Sch_Error_t Scheduler_Resume_Event_API(uint8_t id)
{
	SCH_ASSERT(id < MAX_EVENT_NUMBER , Sch_Error_Invalid_Id) ;
	Events[id].State    = State_Ready ;
	return Sch_Error_Ok ;
}

__attribute__((weak)) int Scheduler_Idle(void * args)
{
	printf("Scheduler idle task\n\n");
	return 0 ;
}
