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

#ifndef INC_SEQUENCER_UTILS_H_
#define INC_SEQUENCER_UTILS_H_

#include "date_time.h"

#define  WILD_CARD_MONTH               (0xFF)
#define  WILD_CARD_YEAR                (0xFF)
#define  WILD_CARD_DAY                 (0xFF)
#define  WILD_CARD_HOUR                (0xFF)
#define  WILD_CARD_MINUTE              (0xFF)
#define  WILD_CARD_SECOND              (0xFF)

/**
 * @brief  Convert seconds to time struct 
 * 
 * @param sTime     pointer to the struct to be filled
 * @param time_in_s time in seconds 
 */
void     UTIL_Uint32_To_Time(Time_t * sTime , uint32_t time_in_s) ;

/**
 * @brief Convert time struct to seconds
 * 
 * @param sTime     pointer to the struct to be converted 
 * @return uint32_t time in seconds
 */
uint32_t UTIL_Time_To_Uint32(Time_t * sTime) ;

/**
 * @brief  Check if an event time is now
 * 
 * @param CurrentTime 
 * @param EventTime 
 * @return true 
 * @return false 
 */
bool     UTIL_IS_Time_Now(Time_t * CurrentTime , Time_t * EventTime) ;

/**
 * @brief Calculate an event next resume time 
 * 
 * @param resume_time   previous resume time (before suspention)
 * @param current_time  current time
 * @param event_period evnt period 
 */
void     UTIL_Calculate_Next_Resume_Time(Time_t * resume_time ,Time_t * current_time, uint32_t event_period) ;

#endif /* INC_SEQUENCER_UTILS_H_ */
