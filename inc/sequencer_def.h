#ifndef INC_SEQUENCER_DEF_H__
#define INC_SEQUENCER_DEF_H__

#include "date_time.h"

typedef int (*EventCallback_t) (Time_t * ExecutionTime , void * args) ;

#define  SEQ_REPETITION_INF            (0xFFFFFFFF)
#define  SEQ_EVERY_MONTH               (0xFF)
#define  SEQ_EVERY_YEAR                (0xFF)
#define  SEQ_EVERY_DAY                 (0xFF)
#define  SEQ_EVERY_HOUR                (0xFF)
#define  SEQ_EVERY_MINUTE              (0xFF)
#define  SEQ_EVERY_SECOND              (0xFF)

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
	Priority_Medium  = 1,
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

#endif /*INC_SEQUENCER_DEF_H__*/