#include "acutest.h"
#include "date_time.h"
#include "scheduler_utils.h"


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

TEST_LIST = {
	{"uint32_t UTIL_Time_To_Uint32(Time_t * time_struct);" , utilities_time_to_uint32_tunit_test},
    {"uint32_t UTIL_Uint32_To_Time(Time_t * sTime , uint32_t time_in_s)" ,utilities_uint32_to_time_test },
	{0}/*   */
};