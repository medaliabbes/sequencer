#include "acutest.h"
#include "date_time.h"
#include "sequencer.h"
#include "sequencer_utils.h"

#include <stdio.h>

Time_t GTime = { .hour = 10 , .minute = 12 , .second = 5 }; //global time 
void setAlarm(Time_t * TimeStuct)
{

}

void getTime(Time_t * TimeStuct)
{
    memcpy(TimeStuct , &GTime , sizeof(Time_t));
}

int callback(void * args)
{
    printf("Event trigred\n") ;
    return 0 ;
}

void sequencer_unit_test(void)
{
    SequencerInitConfig_t SequencerInit = { .GetTime = getTime , .SetAlarm = setAlarm } ;
    Sequencer_Init(&SequencerInit) ;

    Time_t StartTime = { .hour = 10 , .minute = 12 , .second = 0 } ;
    Time_t NextTime = { 0} ; 
    uint8_t id = Sequencer_Add_Event_API(callback , &StartTime , 10 , 60 , Priority_High , NULL) ;
    GTime.hour = 10 ; GTime.minute = 12 ; GTime.second = 20 ;
    Sequencer_Get_Next_Event_Time(&NextTime);
    TEST_CHECK_( NextTime.hour == 10 && NextTime.minute == 13 && NextTime.second == 0 ,"nextTime %2d:%2d:%2d" , 10,13,0 ) ;
}

void utilities_time_to_uint32_tunit_test()
{
    Time_t time_struct = { .hour = 1 , .minute = 0 , .second = 0 } ;
	TEST_CHECK_(UTIL_Time_To_Uint32(&time_struct) == 3600 , "UTIL_Time_To_Uint32(%s)==%d"  , "&time_struct",3600);
    Time_t time_struct1 = { .hour = 0 , .minute = 26 , .second = 26 } ;
    TEST_CHECK_(UTIL_Time_To_Uint32(&time_struct1) == 1586 , "UTIL_Time_To_Uint32(%s)==%d"  , "&time_struct",1586);
    Time_t time_struct2 = { .hour = 23 , .minute = 11 , .second = 0 } ;
    TEST_CHECK_(UTIL_Time_To_Uint32(&time_struct2) == 83460 , "UTIL_Time_To_Uint32(%s)==%d" , "&time_struct",83460);
    Time_t time_struct3 = { .hour = 16 , .minute = 50 , .second = 25 } ;
    TEST_CHECK_(UTIL_Time_To_Uint32(&time_struct3) == 60625 , "UTIL_Time_To_Uint32(%s)==%d" , "&time_struct",60625);
    Time_t time_struct4 = { .hour = 6 , .minute = 33 , .second = 40 } ;
    TEST_CHECK_(UTIL_Time_To_Uint32(&time_struct4) == 23620 , "UTIL_Time_To_Uint32(%s)==%d" , "&time_struct",23620);
}

void utilities_uint32_to_time_test()
{
    Time_t   time_struct = {0} ;
    uint32_t time_in_sec = 3620  ;
    UTIL_Uint32_To_Time(&time_struct , time_in_sec) ;
    TEST_CHECK_( ((time_struct.hour == 1) && (time_struct.minute == 0 ) && (time_struct.second == 20)) ,"hour== %d , minute == %d , sec == %d" , 1,0,20 ) ;

    time_in_sec = 93605 ;
    UTIL_Uint32_To_Time(&time_struct , time_in_sec) ;
    TEST_CHECK_( ((time_struct.hour == 2) && (time_struct.minute == 0 ) && (time_struct.second == 5)) ,"hour== %d , minute == %d , sec == %d" , 2,0,5 ) ;

    time_in_sec = 2163024 ;
    UTIL_Uint32_To_Time(&time_struct , time_in_sec) ;
    TEST_CHECK_( ((time_struct.hour == (600 % 24)) && (time_struct.minute == 50 ) && (time_struct.second == 24)) ,"hour== %d , minute == %d , sec == %d" , 600%24,50,24 ) ;

}

void utilities_calculate_next_resume_time_test()
{
    Time_t current_time  = { .hour = 12 , .minute = 10 , .second = 0 } ;
    Time_t resume_time   = { .hour = 11 , .minute = 30 , .second = 10} ; //last resume time
    uint32_t period = 10 ;//in second
    UTIL_Calculate_Next_Resume_Time(&resume_time , &current_time , period) ;
    TEST_CHECK_( (resume_time.hour == 12 ) && (resume_time.minute == 10) && (resume_time.second == 10), "hour== %d , minute == %d , sec == %d" , 12,10,10 );

    Time_t current_time1  = { .hour = 12 , .minute = 11 , .second = 0 } ;
    Time_t resume_time1   = { .hour = 12 , .minute = 10 , .second = 17} ; //last resume time
    period = 3 ;
    UTIL_Calculate_Next_Resume_Time(&resume_time1 , &current_time1 , period) ;
    TEST_CHECK_( (resume_time1.hour == 12 ) && (resume_time1.minute == 11) && (resume_time1.second == 2), "hour== %d , minute == %d , sec == %d" , 12,11,2 );
    
    Time_t current_time2  = { .hour = 0 , .minute = 0 , .second = 0 } ;
    Time_t resume_time2   = { .hour = 23 , .minute = 10 , .second = 17} ; //last resume time
    period = 3600 ;
    UTIL_Calculate_Next_Resume_Time(&resume_time2 , &current_time2 , period) ;
    TEST_CHECK_( (resume_time2.hour == 0 ) && (resume_time2.minute == 10) && (resume_time2.second == 17), "hour== %d , minute == %d , sec == %d" , 3,10,17 );
    
    Time_t current_time3  = { .hour = 12 , .minute = 0 , .second = 0 } ;
    Time_t resume_time3   = { .hour = 11 , .minute = 50 , .second = 0} ; //last resume time
    period = 30*60 ;
    UTIL_Calculate_Next_Resume_Time(&resume_time3 , &current_time3 , period) ;
    TEST_CHECK_( (resume_time3.hour == 12 ) && (resume_time3.minute == 20) && (resume_time3.second == 0), "hour== %d , minute == %d , sec == %d" , 12,20,0 );
    
    Time_t current_time4  = { .hour = 17 , .minute = 0 , .second = 0 } ;
    Time_t resume_time4   = { .hour = 14 , .minute = 45 , .second = 32} ; //last resume time
    period = 20*60 ;
    UTIL_Calculate_Next_Resume_Time(&resume_time4 , &current_time4 , period) ;
    TEST_CHECK_( (resume_time4.hour == 17 ) && (resume_time4.minute == 05) && (resume_time4.second == 32), "hour== %d , minute == %d , sec == %d" , 17,5,32 );
    
    Time_t current_time5  = { .hour = 8 , .minute = 30 , .second = 0 } ;
    Time_t resume_time5   = { .hour = 8 , .minute = 0 , .second = 0} ; //last resume time
    period = 60*60 ;
    UTIL_Calculate_Next_Resume_Time(&resume_time5 , &current_time5 , period) ;
    TEST_CHECK_( (resume_time5.hour == 9 ) && (resume_time5.minute == 0) && (resume_time5.second == 0), "hour== %d , minute == %d , sec == %d" , 17,5,32 );
    
    Time_t current_time6  = { .hour = 0 , .minute = 3 , .second = 0 } ;
    Time_t resume_time6   = { .hour = 23 , .minute = 55 , .second = 0} ; //last resume time
    period = 10*60 ;
    UTIL_Calculate_Next_Resume_Time(&resume_time6 , &current_time6 , period) ;
    TEST_CHECK_( (resume_time6.hour == 0 ) && (resume_time6.minute == 5) && (resume_time6.second == 0), "hour== %d , minute == %d , sec == %d" , 0,5,0 );
   
}

TEST_LIST = {
	{"uint32_t UTIL_Time_To_Uint32(Time_t * time_struct);" , utilities_time_to_uint32_tunit_test},
    {"uint32_t UTIL_Uint32_To_Time(Time_t * sTime , uint32_t time_in_s)" ,utilities_uint32_to_time_test },
    {"void UTIL_Calculate_Next_Resume_Time(Time_t * resume_time ,Time_t * current_time, uint32_t event_period)" ,utilities_calculate_next_resume_time_test} ,
	{"void sequencer_unit_test(void)" , sequencer_unit_test} ,
    {0},/*   */
};