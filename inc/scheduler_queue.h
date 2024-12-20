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
* file  : scheduler_queue.h
*
*  Created on: Nov 17, 2024
*      Author: M.BEN ABBES
*
*/
#ifndef INC_SCHEDULER_QUEUE_H_
#define INC_SCHEDULER_QUEUE_H_


#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct{
	uint8_t * buffer     ;
	uint8_t   size       ;
	uint8_t * WritePtr   ;
}queue_t;

bool Queue_Init(queue_t * Queue , uint8_t * buffer , uint8_t size) ;

bool Queue_Push(queue_t * Queue , uint8_t element) ;

bool Queue_Pop(queue_t * Queue , uint8_t * element)  ;

bool Queue_Dump(queue_t * Queue ) ;

bool Queue_Is_Epmty(queue_t * Queue) ;

int  Queue_Get_Size(queue_t * Queue) ;

bool Queue_Deinit(queue_t * Queue)  ;

#endif /* INC_SCHEDULER_QUEUE_H_ */