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
#include "scheduler_utils.h"

static int  Scheduler_Get_Next_Event_Time(Time_t * NextEventTime) ;
static Sch_Error_t Scheduler_Add_Event(Event_t * Event) ;

static Event_t Events[MAX_EVENT_NUMBER] ;

static GetTime_t  GetTime  = NULL ;
static SetAlarm_t SetAlarm = NULL ;
static uint8_t EventCounter = 0 ;

/**NextEventId this should be replaced by a queue ,so when more than
 * one event occurs at the same time they can be executed*/
//static uint8_t NextEventId  = MAX_EVENT_NUMBER ;
static queue_t EventQueue  = {0};

static uint8_t EventQueueBuffer[10] = {0} ;

Sch_Error_t Scheduler_Init(SchedulerInitConfig_t * Config)
{
  SCH_ASSERT(Config != NULL , Sch_Error_Null_Pointer) ;

  SCH_ASSERT(Config->SetAlarm != NULL , Sch_Error_Null_Pointer) ;
  SCH_ASSERT(Config->GetTime  != NULL , Sch_Error_Null_Pointer) ;

  GetTime  = Config->GetTime  ;
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
  /***NextExcTime should be calculated here**/
  Time_t CurrentTime = {0} ;
  GetTime(&CurrentTime) ;

  if( UTIL_Time_To_Uint32(&CurrentTime) > UTIL_Time_To_Uint32(&my_event.StartTime ))
  {  /**Start time in the paste */
    /**this will align the event time*/
    memcpy(&my_event.NextExcTime ,&my_event.StartTime  , sizeof(Time_t) );
    UTIL_Calculate_Next_Resume_Time(&my_event.NextExcTime  , &CurrentTime , Periode );
  }
  else
  {  /**start time in the feature*/
    memcpy(&my_event.NextExcTime , &my_event.StartTime , sizeof(Time_t));
  }

  my_event.MagicNumber = MAGIC_NUMBER ;
  Scheduler_Add_Event(&my_event) ;
  Time_t NextEventTime = { 0 };
  Scheduler_Get_Next_Event_Time(&NextEventTime) ;
  SCH_LOG("API start time %02d:%02d:%02d\n" , NextEventTime.hour ,
    NextEventTime.minute ,
    NextEventTime.second) ;
  /**set alarm to next event time*/
  SetAlarm(&NextEventTime) ;
  return  my_event.id ;
}


static int  Scheduler_Get_Next_Event_Time(Time_t * NextEventTime)
{
  Time_t now ;
  GetTime(&now) ;

  uint32_t t_now = UTIL_Time_To_Uint32(&now);
  uint8_t selected_event = MAX_EVENT_NUMBER ;
  uint32_t ptime = 0xffffffff ;
  Q_Event_t QEvent = {0} ;
  Queue_Dump(&EventQueue) ;
  for(int i = 0 ; i< EventCounter ; i++ )
  {
    /**When an event is deleted or suspended , it's not considered to
     * be execute
     */
    if( (Events[i].MagicNumber != MAGIC_NUMBER) ||
        Events[i].State != State_Ready )
    {
      continue ;
    }
    uint32_t ev_time = UTIL_Time_To_Uint32(&Events[i].NextExcTime) ;

   /**inside the if should be replaced or removed **/
    if(ev_time < t_now )
    {
      ev_time += Events[i].Period ;
    }

    if(ev_time < ptime)
    {
      ptime = ev_time ;
      Queue_Dump(&EventQueue) ;
      QEvent.id = i ;
      QEvent.priority = Events[i].Period ;
      Queue_Push(&EventQueue , QEvent) ;
      selected_event = i ;
    }
    else if(ev_time == ptime)
    {
      ptime = ev_time ;
      QEvent.id = i ;
      QEvent.priority = Events[i].Period ;
      Queue_Push(&EventQueue , QEvent) ;
    }
  }

  /**this can be removed **/
  if(selected_event != MAX_EVENT_NUMBER )
  {
    memcpy(NextEventTime , &Events[selected_event].NextExcTime , sizeof(Time_t)) ;
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
    SCH_LOG("Event Suspended\n\n") ;
    return Sch_Error_Exc_Event_Suspended ;
  }
  /**SCH_ASSERT the event is not disabled before execution*/
  if(Events[id].State   == State_Deleted )
  {
    SCH_LOG("Event Deleted\n\n") ;
    return Sch_Error_Exc_Event_Deleted ;
  }

  /**Check event still has num rep to execute */
  if(Events[id].NumRepetetion == 0)
  {
    SCH_LOG("Event End Rep\n\n") ;
    return Sch_Error_Exc_Event_Null_Rep ;
  }
  /**SCH_ASSERT Event CallBack is not NULL*/
  if( Events[id].Callback == NULL)
  {
    SCH_LOG("Event Null Callback\n\n") ;
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
    uint32_t NextTime_s = UTIL_Time_To_Uint32(&Events[id].NextExcTime) + Events[id].Period ;
    UTIL_Uint32_To_Time(&Events[id].NextExcTime , NextTime_s) ;
  }
  return Sch_Error_Ok ;
}

/**
  * @brief This function handles RTC alarms (A and B) interrupt through EXTI line 17.
  */
void RTC_INTERRUPT_ROUTINE(void)
{
  /* USER CODE BEGIN RTC_Alarm_IRQn 0 */

  /* USER CODE END RTC_Alarm_IRQn 0 */
  RTC_INTERRUPT_HANDLER() ;
  /* USER CODE BEGIN RTC_Alarm_IRQn 1 */
  Scheduler_Process() ;
  /* USER CODE END RTC_Alarm_IRQn 1 */
}

Sch_Error_t Scheduler_Process()
{
  int QueueSize = Queue_Get_Size(&EventQueue) ;
  Time_t CurrentTime = {0} ;
  GetTime(&CurrentTime) ;
  /**
   * @Note  : Events can be sorted based on they priority before execution
   */
  SCH_LOG("PROCESS : Number of event to exec : %d\n\n" , QueueSize) ;
  Q_Event_t QEvent = {0} ;
  for(int i = 0 ;i <QueueSize ;i++)
  {
    Queue_Pop(&EventQueue , &QEvent) ;
    /**UTIL_IS_Time_Now Will assure the execution of event at the right time and date */
    if(UTIL_IS_Time_Now(&CurrentTime , &Events[QEvent.id].NextExcTime))
    {
      Scheduler_Execute_Event(QEvent.id) ;
      Scheduler_Update_Event(QEvent.id) ;
    }
  }

  Time_t NextEventTime =  { 0} ;
  int selected_event = Scheduler_Get_Next_Event_Time(&NextEventTime) ;
  if(selected_event == MAX_EVENT_NUMBER)
  {
    SCH_LOG("No next event is selected \n\n") ;
  }
  
  uint32_t current_time_s = UTIL_Time_To_Uint32(&CurrentTime) ;
  uint32_t next_time_s    = UTIL_Time_To_Uint32(&NextEventTime) ;
  /**if alarm time (NextEventTime) == current time no interrupt will be triggered */
  if(current_time_s >= next_time_s)
  { /**Should add idle task here**/
    SCH_LOG("Scheduler will Fail\n");
    return Sch_Error_Core_Failed ;
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
  if(Events[id].State == State_Suspended)
  {
    /**Should calculate the next execution time after suspension*/
    Time_t CurrentTime = { 0} ;
    GetTime(&CurrentTime) ;
    UTIL_Calculate_Next_Resume_Time(&Events[id].NextExcTime ,&CurrentTime ,Events[id].Period ) ;
    /**Event state should be set to ready before process ,so it's be consider for scheduling */
    Events[id].State    = State_Ready ;
    Scheduler_Process();
  }

  return Sch_Error_Ok ;
}

__attribute__((weak)) int Scheduler_Idle(void * args)
{
  SCH_LOG("Scheduler idle task\n\n");
  return 0 ;
}
