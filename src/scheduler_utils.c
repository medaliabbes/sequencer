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
 * scheduler_utils.c
 *
 *  Created on: Jan 1, 2025
 *      Author: moham
 */
#include "scheduler_utils.h"

void     UTIL_Uint32_To_Time(Time_t * sTime , uint32_t time_in_s) 
{
  sTime->hour   = time_in_s / 3600 ;
  sTime->minute = (time_in_s - (sTime->hour  * 3600)) / 60 ;
  sTime->second = time_in_s % 60 ;
  if( time_in_s / 3600 >= 24)
  {
    sTime->hour   = (time_in_s / 3600) % 24 ;
  }
  else{
    
  }
}

uint32_t UTIL_Time_To_Uint32(Time_t * sTime) 
{
  uint32_t ret = sTime->second + (sTime->minute * 60) + (sTime->hour * 60 * 60);
  return ret ;
}

bool     UTIL_IS_Time_Now(Time_t * CurrentTime , Time_t * EventTime) 
{

  if(!(EventTime->year == CurrentTime->year )&& !(EventTime->year == WILD_CARD_YEAR))
  {
    return false ;
  }

  if(!(EventTime->month == CurrentTime->month )&& !(EventTime->month == WILD_CARD_MONTH))
  {
    return false ;
  }

  if(!(EventTime->day == CurrentTime->day )&& !(EventTime->day == WILD_CARD_DAY))
  {
    return false ;
  }

  if(!(EventTime->hour == CurrentTime->hour )&& !(EventTime->hour == WILD_CARD_HOUR))
  {
    return false ;
  }

  if(!(EventTime->minute == CurrentTime->minute )&& !(EventTime->minute == WILD_CARD_MINUTE))
  {
    return false ;
  }

  /**CAN TOLERATE 1 OR 2 SECONDS*/
  if(!(EventTime->second == CurrentTime->second )&& !(EventTime->second == WILD_CARD_SECOND))
  {
    return false ;
  }
  return true ;
}

void     UTIL_Calculate_Next_Resume_Time(Time_t * resume_time ,Time_t * current_time, uint32_t event_period) 
{
  uint32_t current_time_sec = UTIL_Time_To_Uint32(current_time) ;
  uint32_t resume_time_sec  = UTIL_Time_To_Uint32(resume_time)  ;

  uint32_t delta_time = 0 ;
  if(current_time_sec < resume_time_sec)
  { /**This mean that the next resume time will be in the next day */
    delta_time = (24*60*60) - resume_time_sec + current_time_sec ;//(resume_time_sec - current_time_sec ) ;
  }
  else{
    delta_time = current_time_sec - resume_time_sec ;
  }
  uint32_t remaining_to_next_period = event_period - (delta_time % event_period) ;
  resume_time_sec = remaining_to_next_period + current_time_sec ;

  UTIL_Uint32_To_Time(resume_time, resume_time_sec) ;
  //printf("Resume time %d:%d:%d\n\n", resume_time->hour , resume_time->minute , resume_time->second) ;
}