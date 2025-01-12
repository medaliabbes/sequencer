# Sequencer 

## Overview
The Sequencer is a lightweight and efficient library designed to provide an easy-to-use API for priority-based execution of events. It is optimized for use on low-power devices, ensuring minimal resource usage while maintaining reliability and flexibility.

## Features 

- **Priority-Based Execution**: Schedule events with different priority levels to ensure critical tasks are executed first.

- **Lightweight**: Minimal resource footprint, ideal for low-power and resource-constrained environments.

- **Easy-to-Use API**: Simple and intuitive API for quick integration.

- **Flexible Event Handling**: Support for various event types and scheduling requirements.

## Use Cases
#### The Sequencer is suitable for :
- Embedded systems.
- IoT devices.
- Low power applications 
## How to use 
#### Implement set_alarm and get_time functions 
``` c
/**Example STM32 HAL implementation */
static void get_time(Time_t * TimeStruct)
{
#if  defined(USE_HAL_DRIVER) && defined(HAL_RTC_MODULE_ENABLED)
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
#if  defined(USE_HAL_DRIVER) && defined(HAL_RTC_MODULE_ENABLED)
  RTC_AlarmTypeDef sAlarm = {0};
  sAlarm.AlarmTime.Hours = TimeStruct->hour;
  sAlarm.AlarmTime.Minutes = TimeStruct->minute;
  sAlarm.AlarmTime.Seconds = TimeStruct->second;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY ;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
	Error_Handler();
  }
#endif
}

```
#### Initialization
``` c
SequencerInitConfig_t  SequencerConfig
SequencerConfig.GetTime = get_time ;
SequencerConfig.SetAlarm = set_alarm ;
Sequencer_Init(&SequencerConfig) ;
```
#### Adding event 
``` c
Time_t start_time = { .hour = 0 , .minute = 0 , .second = 10 ,
                      .day = SEQ_EVERY_DAY ,
                      .month = SEQ_EVERY_MONTH ,
                      .year = SEQ_EVERY_YEAR } ;
uint8_t log_handler_id = Sequencer_Add_Event_API(log_handler , &start_time , 
                                                 SEQ_REPETITION_INF,
                                                 6 ,
                                                 Priority_Low, 
                                                 NULL);
```
#### Suspending event
``` c
Sequencer_Suspend_Event_API(log_handler_id) ;
```

#### Resuming event
``` c
Sequencer_Resume_Event_API(log_handler_id) ;
```

#### Deleting event
``` c
Sequencer_Delete_Event_API(stack_handler_id) ;
```

#### Call scheduler process in the main loop

``` c
while(1)
  {
    Sequencer_Process() ;
  }
```
## License
This project is licensed under the MIT License. See the **LICENSE** file for details

--- 

Feel free to report issues or suggest improvements via the issue tracker.
Happy scheduling.