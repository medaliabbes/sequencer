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
#include "sequencer.h"
#include "sequencer_utils.h"

#ifndef ENABLE_UNIT_TEST
static int  Sequencer_Get_Next_Event_Time(Time_t * NextEventTime) ;
#endif 
static Seq_Error_t Sequencer_Add_Event(Event_t * Event) ;

static Event_t Events[MAX_EVENT_NUMBER] ;

static GetTime_t  GetTime  = NULL ;
static SetAlarm_t SetAlarm = NULL ;
static uint8_t EventCounter = 0 ;

volatile uint8_t notification = 0 ;

/**Event Queue hold events that will be executed at the next alarm */
static queue_t EventQueue  = {0};

static uint8_t EventQueueBuffer[10] = {0} ;

Seq_Error_t Sequencer_Init(SequencerInitConfig_t * Config)
{
  SEQ_ASSERT(Config != NULL , Sch_Error_Null_Pointer) ;

  SEQ_ASSERT(Config->SetAlarm != NULL , Sch_Error_Null_Pointer) ;
  SEQ_ASSERT(Config->GetTime  != NULL , Sch_Error_Null_Pointer) ;

  GetTime  = Config->GetTime  ;
  SetAlarm = Config->SetAlarm ;

  Queue_Init(&EventQueue , EventQueueBuffer , 10) ;

  return Seq_Error_Ok ;
}

uint8_t Sequencer_Add_Event_API(EventCallback_t EventHandler , Time_t * StartTime ,
		                     uint32_t Repetetion , uint32_t Periode , Priority_t Priority ,
							 void * args)
{
  SEQ_ASSERT( EventHandler != NULL , Seq_Error_Null_Pointer );
  SEQ_ASSERT( StartTime != NULL    , Seq_Error_Null_Pointer );
  SEQ_ASSERT( Repetetion != 0      , Seq_Error_Null_Repetition);
  SEQ_ASSERT( EventCounter < MAX_EVENT_NUMBER , Seq_Error_Excided_Max_Events) ;

  SEQ_LOG("Sequencer_Add_Event_API\n");
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
  Sequencer_Add_Event(&my_event) ;
  Time_t NextEventTime = { 0 };
  Sequencer_Get_Next_Event_Time(&NextEventTime) ;
  SEQ_LOG("API start time %02d:%02d:%02d\n" , NextEventTime.hour ,
    NextEventTime.minute ,
    NextEventTime.second) ;
  /**set alarm to next event time*/
  SetAlarm(&NextEventTime) ;
  return  my_event.id ;
}

#ifndef ENABLE_UNIT_TEST 
static int  Sequencer_Get_Next_Event_Time(Time_t * NextEventTime)
#else
int  Sequencer_Get_Next_Event_Time(Time_t * NextEventTime)
#endif
{
  Time_t now ;
  GetTime(&now) ;
  Time_t * ptrNow = &now ;
  
  SEQ_LOG_TIME(ptrNow) ;
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
      QEvent.priority = Events[i].Priority ;
      Queue_Push(&EventQueue , QEvent) ;
      selected_event = i ;
    }
    else if(ev_time == ptime)
    {
      ptime = ev_time ;
      QEvent.id = i ;
      QEvent.priority = Events[i].Priority ;
      Queue_Push(&EventQueue , QEvent) ;
    }
  }

  Time_t * event_time  = &Events[selected_event].NextExcTime ; 

  /**this can be removed **/
  if(selected_event != MAX_EVENT_NUMBER )
  {
    memcpy(NextEventTime , &Events[selected_event].NextExcTime , sizeof(Time_t)) ;
  }
  return selected_event ;
}

static Seq_Error_t Sequencer_Add_Event(Event_t * Event)
{
  SEQ_ASSERT( Event != NULL  , Seq_Error_Null_Pointer ) ;
  SEQ_ASSERT(Event->id < MAX_EVENT_NUMBER , Seq_Error_Excided_Max_Events) ;

  memcpy(&Events[Event->id] , Event , sizeof(Event_t)) ;
  return Seq_Error_Ok ;
}

Seq_Error_t Sequencer_Execute_Event(uint8_t id)
{
  SEQ_ASSERT(id < MAX_EVENT_NUMBER , Sch_Error_Invalid_Id);

  /**SCH_ASSERT the event is not suspended before execution*/
  if(Events[id].State == State_Suspended )
  {
    SEQ_LOG("Event Suspended\n\n") ;
    return Seq_Error_Exc_Event_Suspended ;
  }
  /**SCH_ASSERT the event is not disabled before execution*/
  if(Events[id].State   == State_Deleted )
  {
    SEQ_LOG("Event Deleted\n\n") ;
    return Seq_Error_Exc_Event_Deleted ;
  }

  /**Check event still has num rep to execute */
  if(Events[id].NumRepetetion == 0)
  {
    SEQ_LOG("Event End Rep\n\n") ;
    return Seq_Error_Exc_Event_Null_Rep ;
  }
  /**SCH_ASSERT Event CallBack is not NULL*/
  if( Events[id].Callback == NULL)
  {
    SEQ_LOG("Event Null Callback\n\n") ;
    return Seq_Error_Exc_Event_Null_cbk ;
  }
  else{
    Events[id].Callback( Events[id].args ) ;
  }

  return Seq_Error_Ok ;
}

void Sequencer_Event_Set_Args(uint8_t id , void * arg1, void * arg2)
{

}

/**
 * Brief  This Function calculate and set an event
 *        next execution time
 * param  id of the event
 */
Seq_Error_t Sequencer_Update_Event(uint8_t id )
{
  SEQ_ASSERT(id < MAX_EVENT_NUMBER  , Sch_Error_Invalid_Id) ;

  if(Events[id].NumRepetetion == 0)
  {
    return Seq_Error_Null_Repetition ;
  }

  if(Events[id].NumRepetetion != SEQ_REPETITION_INF){
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
  return Seq_Error_Ok ;
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
  notification =  RTC_INTERRUPT_NOTIF ;
  /* USER CODE END RTC_Alarm_IRQn 1 */
}

Seq_Error_t Sequencer_Process()
{
  notification = 0 ;
  int QueueSize = Queue_Get_Size(&EventQueue) ;
  Queue_Sort(&EventQueue) ;
  Time_t CurrentTime = {0} ;
  GetTime(&CurrentTime) ;
  /**
   * @Note  : Events can be sorted based on they priority before execution
   */
  SEQ_LOG("PROCESS : Number of event to exec : %d\n\n" , QueueSize) ;
  Q_Event_t QEvent = {0} ;
  for(int i = 0 ;i <QueueSize ;i++)
  {
    Queue_Pop(&EventQueue , &QEvent) ;
    /**UTIL_IS_Time_Now Will assure the execution of event at the right time and date */
    if(UTIL_IS_Time_Now(&CurrentTime , &Events[QEvent.id].NextExcTime))
    {
      Sequencer_Execute_Event(QEvent.id) ;
      Sequencer_Update_Event(QEvent.id) ;
    }
  }

  Time_t NextEventTime =  { 0} ;
  int selected_event = Sequencer_Get_Next_Event_Time(&NextEventTime) ;
  if(selected_event == MAX_EVENT_NUMBER)
  {
    SEQ_LOG("No next event is selected \n\n") ;
  }
  
  uint32_t current_time_s = UTIL_Time_To_Uint32(&CurrentTime) ;
  uint32_t next_time_s    = UTIL_Time_To_Uint32(&NextEventTime) ;
  /**if alarm time (NextEventTime) == current time no interrupt will be triggered */
  if(current_time_s >= next_time_s)
  { /**Should add idle task here**/
    SEQ_LOG("Scheduler will Fail\n");
    return Seq_Error_Core_Failed ;
  }
  SetAlarm(&NextEventTime) ;
  return Seq_Error_Ok ;
}

bool Is_Seq_notification() 
{
  if(notification == RTC_INTERRUPT_NOTIF)
	  return true ;
  return false ;
}

Seq_Error_t Sequencer_Delete_Event_API(uint8_t id)
{
  SEQ_ASSERT(id < MAX_EVENT_NUMBER , Sch_Error_Invalid_Id) ;
  Events[id].State   = State_Deleted ;
  return Seq_Error_Ok ;
}

Seq_Error_t Sequencer_Suspend_Event_API(uint8_t id)
{
  SEQ_ASSERT(id < MAX_EVENT_NUMBER , Seq_Error_Invalid_Id) ;
  Events[id].State = State_Suspended ;
  return Seq_Error_Ok ;
}

Seq_Error_t Scheduler_Resume_Event_API(uint8_t id)
{
  SEQ_ASSERT(id < MAX_EVENT_NUMBER , Seq_Error_Invalid_Id) ;
  if(Events[id].State == State_Suspended)
  {
    /**Should calculate the next execution time after suspension*/
    Time_t CurrentTime = { 0} ;
    GetTime(&CurrentTime) ;
    UTIL_Calculate_Next_Resume_Time(&Events[id].NextExcTime ,&CurrentTime ,Events[id].Period ) ;
    /**Event state should be set to ready before process ,so it's be consider for scheduling */
    Events[id].State    = State_Ready ;
    Sequencer_Get_Next_Event_Time(&CurrentTime) ;
  }

  return Seq_Error_Ok ;
}

#ifdef ENABLE_UNIT_TEST
int Get_Number_Events_To_Execute()
{
  return  Queue_Get_Size(&EventQueue) ;
}
#endif /*ENABLE_UNIT_TEST*/

__attribute__((weak)) int Sequencer_Idle(void * args)
{
  SEQ_LOG("Sequencer idle task\n\n");
  return 0 ;
}
