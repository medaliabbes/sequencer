#include "../inc/scheduler.h" 
#include <stdio.h>

int log_handler(void * args)  ;
int stack_hanler(void * args) ;
int periodic_handler(void * args);

static void get_time(Time_t * TimeStuct)   ;
static void set_alarm(Time_t * TimeStruct) ;

int main()
{
    SchedulerConfig.GetTime = get_time ;
    SchedulerConfig.SetAlarm = set_alarm ;
    Scheduler_Init(&SchedulerConfig) ;

    Time_t s_time = { .hour = 0 , .minute = 0 , .second = 10 ,.day = SCH_EVERY_DAY ,
  	  	  	  	  	  	  	  	  	  	  	  	  	  	      .month = SCH_EVERY_MONTH ,
  	  	  	  	  	  	  	  	  	  	  	  	  	  	      .year = SCH_EVERY_YEAR } ;
    Time_t s_time1 = { .hour = 0 , .minute = 0 , .second = 10 ,.day = SCH_EVERY_DAY ,
    	  	  	  	  	  	  	  	  	  	  	  	  	  	  	.month = SCH_EVERY_MONTH ,
    	  	  	  	  	  	  	  	  	  	  	  	  	  	    .year = SCH_EVERY_YEAR } ;
    uint8_t log_handler_id = Scheduler_Add_Event_API(log_handler , &s_time ,
		             SCH_REPETITION_INF , 6 , Priority_Low,
  					 NULL);
    uint8_t stack_handler_id = Scheduler_Add_Event_API(stack_hanler, &s_time1, SCH_REPETITION_INF, 10 , Priority_Medium, NULL) ;

    uint8_t p_handler_id = Scheduler_Add_Event_API(periodic_handler ,&s_time1, SCH_REPETITION_INF, 5, Priority_High, NULL) ;

    int i = 0;
    while(1)
    {
        Scheduler_Process() ;
        if( i == 10000)
        {
            Scheduler_Suspend_Event_API(log_handler_id) ;
        }
        if(i == 100000)
        {
            Scheduler_Resume_Event_API(log_handler_id) ;
        }
        if( i == 300000)
        {
            Scheduler_Delete_Event_API(stack_handler_id) ;
        }
        i++ ;
    }
}


static void get_time(Time_t * TimeStruct)
{
/**Platform specific code */
#ifdef STM232
  RTC_DateTypeDef sDate ;
  RTC_TimeTypeDef sTime ;
  HAL_RTC_GetTime(&hrtc, &sTime,RTC_FORMAT_BIN ) ;
  HAL_RTC_GetDate(&hrtc, &sDate,RTC_FORMAT_BIN ) ;

  TimeStruct->year   = sDate.Year ;
  TimeStruct->month  = sDate.Month ;
  TimeStruct->day    = sDate.Date ;
  TimeStruct->hour   = sTime.Hours ;
  TimeStruct->minute = sTime.Minutes ;
  TimeStruct->second = sTime.Seconds ;
#endif
}

static void set_alarm(Time_t * TimeStruct)
{
/**Platform specific code */
#ifdef STM232
  printf("Next alarm %02d:%02d:%02d\n" , TimeStruct->hour ,
		  TimeStruct->minute ,TimeStruct->second);

  RTC_AlarmTypeDef sAlarm = {0};
  sAlarm.AlarmTime.Hours = TimeStruct->hour;
  sAlarm.AlarmTime.Minutes = TimeStruct->minute;
  sAlarm.AlarmTime.Seconds = TimeStruct->second;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY ;// |
		  	  	  	// RTC_ALARMMASK_MINUTES     |
					// RTC_ALARMMASK_HOURS       ; //RTC_ALARMMASK_NONE;

  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  //sAlarm.AlarmDateWeekDay = 0x2;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
	Error_Handler();
  }
#endif
}