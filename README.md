# Scheduler 

## Overview
The Scheduler is a lightweight and efficient library designed to provide an easy-to-use API for priority-based execution of events. It is optimized for use on low-power devices, ensuring minimal resource usage while maintaining reliability and flexibility.

## Features 

- **Priority-Based Execution**: Schedule events with different priority levels to ensure critical tasks are executed first.

- **Lightweight**: Minimal resource footprint, ideal for low-power and resource-constrained environments.

- **Easy-to-Use API**: Simple and intuitive API for quick integration.

- **Flexible Event Handling**: Support for various event types and scheduling requirements.

## Use Cases
#### The Scheduler is suitable for :
- Embedded systems.
- IoT devices.
- Low power applications 
## How to use 
scheduler initialisation
``` c
SchedulerInitConfig_t  SchedulerConfig
SchedulerConfig.GetTime = get_time ;
SchedulerConfig.SetAlarm = set_alarm ;
Scheduler_Init(&SchedulerConfig) ;
```
adding event 
``` c
Time_t s_time = { .hour = 0 , .minute = 0 , .second = 10 ,.day = SCH_EVERY_DAY ,
  	  	  	  	  	  	  	  	  	  	  	  	  	  	      .month = SCH_EVERY_MONTH ,
  	  	  	  	  	  	  	  	  	  	  	  	  	  	      .year = SCH_EVERY_YEAR } ;
uint8_t log_handler_id = Scheduler_Add_Event_API(log_handler , &s_time , SCH_REPETITION_INF , 6 , Priority_Low, NULL);
```
suspending event
``` c:examples/main.c [36-36]

```
resuming event
``` c:examples/main.c [40-40]

```
deleting event
``` c:examples/main.c [44-44]

```
## License
This project is licensed under the MIT License. See the **LICENSE** file for details

--- 

Feel free to report issues or suggest improvements via the issue tracker.
Happy scheduling.